
#include "TaskManager.hpp"

#include "WindowList.hpp"

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QVariant>
#include <QProcess>


Application::Application( WId id ){
	KWindowInfo info( id, 0, NET::WM2WindowClass );
	class_name = info.windowClassName();
	app_path = class_name; //TODO: do better!
	//TODO: working dir to?
}

QDataStream& operator<<( QDataStream& stream, const Application& app ){
	stream << app.app_path << app.class_name << app.working_dir;
	return stream;
}
QDataStream& operator>>( QDataStream& stream, Application& app ){
	stream >> app.app_path >> app.class_name >> app.working_dir;
	return stream;
}


Window::Window( WId id ) : id(id) {
	KWindowInfo info( id, NET::WMVisibleIconName | NET::WMState | NET::WMWindowType );
	title = info.visibleIconName();
	auto type = info.windowType( NET::NormalMask );
	visible = !info.hasState( NET::SkipTaskbar ) && type == 0;
//	qDebug() << title << " ------ " << visible;
}

TaskGroup::TaskGroup( TaskManager& manager ) : TaskBarQWidget<>( manager.taskBar() ), manager(manager){
	setMinimumSize( 32,32 );
	setMaximumSize( 32,32 );
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
	if( icon.isNull() ){
		if( windows.size() > 0 )
			icon = windows.front().icon()
				.scaled( 32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
			//TODO: clear icon when changing size
		//TODO: find application icon
	}
	return icon;
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
	
	painter.drawPixmap( 0,0, getIcon() );
	
	if( hover )
		painter.drawRect( 1,1, 30,30 );
	
	//TODO: properly align it, and draw a circle or something as background
	if( amount > 1 )
		painter.drawText( 20, 28, QString::number( amount ) );
	else if( amount == 0 ){
		painter.setBrush( QBrush( QColor( 0,0,0,127 ) ) );
		painter.setPen( QPen() );
		painter.drawRect( 0,0, 32,32 );
	}
}

void TaskGroup::mouseReleaseEvent( QMouseEvent* event ) {
	if( event->button() == Qt::LeftButton ){
		if( event->modifiers() & Qt::ShiftModifier )
			startApplication();
		else
			switch( areVisible() ){
				case 0: startApplication(); break;
				case 1:
						for( auto& window : windows )
							if( window.isVisible() ){
								window.activate();
							}
					break;
				default: manager.showWindowList( this ); break; //TODO: show selection menu
			}
	}
	else if( event->button() == Qt::RightButton ){
		pinned = !pinned;
		emit pinnedChanged();
	}
	else
		event->ignore();
}


TaskManager::TaskManager( TaskBar& task_bar ) : TaskBarQWidget<>( task_bar ) {
	boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
	boxlayout->setContentsMargins( 0,0,0,0 );
	setLayout( boxlayout );
	
	list = new WindowList( &task_bar );
	
	qRegisterMetaTypeStreamOperators<Application>( "Application" );
	qRegisterMetaTypeStreamOperators<QList<Application>>( "QList<Application>" );
	
	auto apps = taskBar().getSettings().value( "TaskManager/pinned" ).value<QList<Application>>();
	for( auto& app : apps )
		add( app.class_name, new TaskGroup( app, *this ) );
	
	for( auto wid : KWindowSystem::windows() )
		addWindow( wid );
	
	auto system = KWindowSystem::self();
	connect( system, SIGNAL(windowAdded  (WId)), this, SLOT(addWindow   (WId)) );
	connect( system, SIGNAL(windowRemoved(WId)), this, SLOT(removeWindow(WId)) );
	connect( system, SIGNAL(currentDesktopChanged(int)), this, SLOT(refresh()) );
}
	

void TaskManager::addWindow( WId id ){
	KWindowInfo info( id, 0, NET::WM2WindowClass );
	//TODO: determine the difference between class and name
	
	auto task_name = info.windowClassName();
	auto task = tasks.find( task_name );
	if( task != tasks.end() )
		task->second->addWindow( id );
	else
		add( task_name, new TaskGroup( id, *this ) );
};


void TaskManager::removeWindow( WId id ){
	//NOTE: we cannot get the key, so we have to iterate though them all
	for( auto& task : tasks )
		if( task.second->removeWindow( id ) ){
			layout()->removeWidget( task.second );
			task.second->deleteLater();
			tasks.erase( tasks.find( task.first ) );
			break;
		}
}

void TaskManager::savePinned(){
	QList<Application> apps;
	
	for( auto& task : tasks )
		if( task.second->isPinned() )
			apps << task.second->getApp();
	
	auto apps_var = QVariant::fromValue<QList<Application>>(apps);
	taskBar().getSettings().setValue( "TaskManager/pinned", apps_var );
}

void TaskManager::showWindowList( TaskGroup* group ){
	list->changeGroup( group );
	list->show();
//	list->move( 50, 200 );
	//TODO: position
}

void TaskManager::activate( unsigned pos ){
	unsigned i=0;
	for( auto& task : tasks )
		if( task.second->isVisible() ){
			if( i == pos )
				task.second->activate();
			i++;
		}
}

