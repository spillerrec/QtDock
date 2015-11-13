
#include "TaskManager.hpp"

#include "WindowList.hpp"

#include <QVariant>


QDataStream& operator<<( QDataStream& stream, const Application& app ){
	stream << app.app_path << app.class_name << app.working_dir << app.icon;
	return stream;
}
QDataStream& operator>>( QDataStream& stream, Application& app ){
	stream >> app.app_path >> app.class_name >> app.working_dir >> app.icon;
	return stream;
}

TaskManager::TaskManager( TaskBar& task_bar ) : TaskBarQWidget<>( task_bar ) {
	boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
	boxlayout->setContentsMargins( 0,0,0,0 );
	setLayout( boxlayout );
	
	qRegisterMetaTypeStreamOperators<Application>( "Application" );
	qRegisterMetaTypeStreamOperators<QList<Application>>( "QList<Application>" );
	
	auto apps = taskBar().getSettings().value( "TaskManager/pinned" ).value<QList<Application>>();
	for( auto& app : apps )
		add( new TaskGroup( app, *this ) );
	
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
	
	for( auto& task : tasks )
		if( task->getApp().class_name == task_name ){
			task->addWindow( id );
			return;
		}
	
	add( new TaskGroup( id, *this ) );
};


void TaskManager::removeWindow( WId id ){
	//NOTE: we cannot get the key, so we have to iterate though them all
	for( auto& task : tasks )
		if( task->removeWindow( id ) ){
			layout()->removeWidget( task );
			task->deleteLater();
			tasks.erase( std::find_if( tasks.begin(), tasks.end()
				,	[task]( TaskGroup* p ){ return p == task; }
				) );
			break;
		}
}

void TaskManager::savePinned(){
	QList<Application> apps;
	
	for( auto& task : tasks )
		if( task->isPinned() )
			apps << task->getApp();
	
	auto apps_var = QVariant::fromValue<QList<Application>>(apps);
	taskBar().getSettings().setValue( "TaskManager/pinned", apps_var );
}

void TaskManager::showWindowList( TaskGroup* group ){
	auto list = new WindowList( task_bar );
	list->show();
	list->changeGroup( group );
	positionPopup( *this, *list, {width()/2, group->pos().y() + group->width()/2} );
}

void TaskManager::activate( unsigned pos, bool shift ){
	unsigned i=0;
	for( auto& task : tasks )
		if( task->isVisible() ){
			if( i == pos )
				task->activate( shift ? Qt::ShiftModifier : Qt::NoModifier );
			i++;
		}
}

