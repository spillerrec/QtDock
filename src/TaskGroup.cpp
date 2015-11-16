
#include "TaskGroup.hpp"
#include "TaskManager.hpp"

#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QProcess>


Application::Application( WId id ){
	KWindowInfo info( id, 0, NET::WM2WindowClass );
	class_name = info.windowClassName();
	app_path = class_name; //TODO: do better!
	//TODO: working dir to?
}


Window::Window( WId id ) : id(id) {
	KWindowInfo info( id, NET::WMState | NET::WMWindowType );
	auto type = info.windowType( NET::AllTypesMask );
	auto view_type = type == NET::Normal || type == NET::Unknown;
	visible = !info.hasState( NET::SkipTaskbar ) && view_type;
//	qDebug() << getTitle() << " ------ " << visible << "---";
}

QString Window::getTitle() const{
	return KWindowInfo( id, NET::WMVisibleIconName ).visibleIconName();
}

TaskGroup::TaskGroup( TaskManager& manager ) : TaskBarQWidget<>( manager.taskBar() ), manager(manager){
	setMinimumSize( 36,36 );
	setMaximumSize( 36,36 );
	setAttribute( Qt::WA_Hover, true );
}


TaskGroup::TaskGroup( WId window, TaskManager& manager ) : TaskGroup( manager ) {
	addWindow( window );
	app = Application( window );
}

TaskGroup::TaskGroup( Application application, TaskManager& manager )
	:	TaskGroup( manager ) {
	app = application;
	pinned = true;
}

QPixmap TaskGroup::getIcon(){
	if( app.icon.isNull() ){
		if( windows.size() > 0 )
			app.icon = windows.front().icon()
				.scaled( 32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation );
			//TODO: clear icon when changing size
		//TODO: find application icon
	}
	return app.icon;
}

void TaskGroup::refresh(){
	if( areVisible() > 0 || pinned ){
		show();
		update();
	}
	else
		hide();
}

void TaskGroup::startApplication(){
	//TODO: ClassName seems to usually be the name of the executable, but use something more reliable!
	if( !QProcess::startDetached( app.app_path ) )
		QMessageBox::warning( this, tr("Could not start program"), tr("Could not start executable: ") + app.app_path );
}


void TaskGroup::paintEvent( QPaintEvent* ) {
	//TODO: calculate positions dynamically
	auto amount = areVisible();
	
	QPainter painter(this);
	painter.setRenderHint( QPainter::SmoothPixmapTransform );
	
	//Pinned, but not running applications
	if( amount == 0 )
		painter.setOpacity( 0.3 );
	
	//Draw app icon centered
	auto icon = getIcon();
	auto icon_pos = (size() - icon.size()) / 2;
	painter.drawPixmap( icon_pos.width(), icon_pos.height(), getIcon() );
	
	painter.setOpacity( 1.0 );
	
	//TODO: properly align it, and draw a circle or something as background
	if( amount > 1 )
		painter.drawText( 24, 32, QString::number( amount ) );
	
	if( underMouse() )
		painter.drawRect( 0,0, 35,35 );
}

void TaskGroup::activate( Qt::KeyboardModifiers mods ){
	if( mods & Qt::ShiftModifier )
		startApplication();
	else
		switch( areVisible() ){
			case 0: startApplication(); break;
			case 1:
					for( auto& window : windows )
						if( window.isVisible() ){
							if( window.isActive() )
								window.minimize();
							else
								window.activate();
							break;
						}
				break;
			default: manager.showWindowList( *this ); break; //TODO: show selection menu
		}
}

void TaskGroup::mousePressEvent( QMouseEvent* event ) {
	if( event->button() == Qt::LeftButton || event->button() == Qt::RightButton )
		event->accept();
}
void TaskGroup::mouseReleaseEvent( QMouseEvent* event ) {
	if( event->button() == Qt::LeftButton )
		activate( event->modifiers() );
	else if( event->button() == Qt::RightButton ){
		pinned = !pinned;
		emit pinnedChanged();
	}
	else
		event->ignore();
}

