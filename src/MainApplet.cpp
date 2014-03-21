#include "MainApplet.hpp"
#include "TaskGroup.hpp"

#include <QDebug>
#include <QPainter>

#include <algorithm>

using namespace Plasma;


MainApplet::MainApplet(QObject *parent, const QVariantList& args) : Applet(parent, args){
	setAspectRatioMode( Plasma::IgnoreAspectRatio );layout = new QGraphicsLinearLayout(Qt::Horizontal, this);
	connect(&manager, SIGNAL(taskAdded(::TaskManager::Task*)), this, SLOT(addTask(::TaskManager::Task*)));
	connect(&manager, SIGNAL(taskRemoved(::TaskManager::Task*)), this, SLOT(removeTask(::TaskManager::Task*)));
}

void MainApplet::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect& contentsRect){
	
	
}

void MainApplet::init(){
	for( auto task : manager.tasks() ){
		addTask( task );
	}
}


void MainApplet::addTask(::TaskManager::Task* task){
	bool added = false;
	for( auto group : groups )
		if( group->addTask( task ) ){
			added = true;
			break;
		}
	
	if( !added ){
		TaskGroup* group = new TaskGroup( task, this );
		groups.push_back( group );
		layout->addItem( group );
	}
}
void MainApplet::removeTask(::TaskManager::Task* task){
	for( auto group : groups ){
		group->removeTask( task );
		if( group->removeGroup() ){
			layout->removeItem( group );
			delete group; //ops...
		}
	}
	groups.erase(std::remove_if(groups.begin(), groups.end(), [](TaskGroup* group){ return group->removeGroup(); } ), groups.end() );
	update();
}


K_EXPORT_PLASMA_APPLET(dockbartest, MainApplet)

#include "MainApplet.moc"

