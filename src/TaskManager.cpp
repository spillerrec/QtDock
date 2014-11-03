
#include "TaskManager.hpp"




QPixmap TaskGroup::getIcon(){
	if( icon.isNull() ){
		if( windows.size() > 0 )
			icon = windows.front().icon();
		//TODO: find application icon
	}
	return icon;
}

void TaskManager::addWindow( WId id ){
	KWindowInfo info( id, 0, NET::WM2WindowClass );
	//TODO: determine the difference between class and name
	
	auto task_name = info.windowClassName();
	auto task = tasks.find( task_name );
	if( task != tasks.end() )
		task->second->addWindow( id );
	else{
		auto new_task = new TaskGroup( info.windowClassClass(), id, this );
		tasks.insert( { task_name, new_task } );
		layout()->addWidget( new_task );
		if( new_task->areVisible() <= 0 )
			new_task->hide();
	}
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
