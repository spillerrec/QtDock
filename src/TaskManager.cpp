
#include "TaskManager.hpp"

#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QProcess>



Window::Window( WId id ) : id(id) {
	KWindowInfo info( id, NET::WMVisibleIconName | NET::WMState | NET::WMWindowType );
	title = info.visibleIconName();
	auto type = info.windowType( NET::NormalMask );
	visible = !info.hasState( NET::SkipTaskbar ) && type == 0;
//	qDebug() << title << " ------ " << visible;
}

TaskGroup::TaskGroup( TaskBar& task_bar ) : TaskBarQWidget<>( task_bar ){
	setMinimumSize( 32,32 );
	setMaximumSize( 32,32 );
}


TaskGroup::TaskGroup( WId window, TaskBar& task_bar ) : TaskGroup( task_bar ) {
	addWindow( window );
	
	KWindowInfo info( window, 0, NET::WM2WindowClass );
	name = info.windowClassName();
}

TaskGroup::TaskGroup( QByteArray application, TaskBar& task_bar )
	:	TaskGroup( task_bar ) {
	name = application;
	pinned = true;
}

QPixmap TaskGroup::getIcon(){
	if( icon.isNull() ){
		if( windows.size() > 0 )
			icon = windows.front().icon();
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
	if( !QProcess::startDetached( name ) )
		QMessageBox::warning( this, tr("Could not start program"), tr("Could not start executable: ") + name );
}


void TaskGroup::paintEvent( QPaintEvent* ) {
	//TODO: calculate positions dynamically
	QPainter painter(this);
	painter.setRenderHint( QPainter::SmoothPixmapTransform );
	painter.drawPixmap( 0,0, 32,32, getIcon() );
	
	if( hover )
		painter.drawRect( 1,1, 30,30 );
	
	auto amount = areVisible();
	//TODO: properly align it, and draw a circle or something as background
	if( amount > 1 )
		painter.drawText( 20, 28, QString::number( amount ) );
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
				default: break; //TODO: show selection menu
			}
	}
	else
		event->ignore();
}

TaskManager::TaskManager( TaskBar& task_bar ) : TaskBarQWidget<>( task_bar ) {
	boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
	boxlayout->setContentsMargins( 0,0,0,0 );
	setLayout( boxlayout );
	
	add( "opera", new TaskGroup( "opera", taskBar() ) );
	add( "dolphin", new TaskGroup( "dolphin", taskBar() ) );
	add( "konsole", new TaskGroup( "konsole", taskBar() ) );
	add( "kate", new TaskGroup( "kate", taskBar() ) );
	add( "vlc", new TaskGroup( "vlc", taskBar() ) );
	add( "gimp", new TaskGroup( "gimp", taskBar() ) );
	
	for( auto wid : KWindowSystem::windows() )
		addWindow( wid );
	
	auto system = KWindowSystem::self();
	connect( system, SIGNAL(windowAdded(WId)), this, SLOT(addWindow(WId)) );
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
		add( task_name, new TaskGroup( id, taskBar() ) );
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
