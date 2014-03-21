#include "TaskGroup.hpp"
#include <taskmanager/taskmanager.h>
#include <QDebug>

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>

#include <algorithm>

TaskGroup::TaskGroup( TaskManager::Task* task, QGraphicsItem* parent ) : QGraphicsWidget(parent){
	setAcceptHoverEvents(true);
	
	tasks.push_back( task );
	active = false;
	classClass = task->classClass();
	icon = task->icon(64, 64);
	setMinimumSize( QSizeF(16,16) );
	setMaximumSize( QSizeF(64,64) );
}

bool TaskGroup::addTask( TaskManager::Task* task ){
	if( classClass == task->classClass() ){
		tasks.push_back( task );
		return true;
	}
	return false;
}

bool TaskGroup::removeTask( TaskManager::Task* task ){
	unsigned size_before = tasks.size();
	tasks.erase(std::remove(tasks.begin(), tasks.end(), task), tasks.end() );
	update();
	return size_before != tasks.size();
}

void TaskGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget){
	//Magic here!
	painter->setPen( QPen( QColor( Qt::red ) ) );
	if( active )
		painter->setBrush( QBrush( Qt::green ) );
	else
		painter->setBrush( QBrush( Qt::blue ) );
	
	painter->drawRect( rect() );
	painter->drawPixmap( rect().toRect(), icon );
	
	painter->drawText( rect().bottomLeft(), QString::number(tasks.size()) );
}

void TaskGroup::mousePressEvent(QGraphicsSceneMouseEvent*){
	tasks[0]->raise();
}
