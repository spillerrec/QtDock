#ifndef TASK_GROUP
#define TASK_GROUP

#include <QGraphicsWidget>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <vector>

namespace TaskManager{
	class Task;
}


class TaskGroup : public QGraphicsWidget{
	Q_OBJECT
	private:
		std::vector<TaskManager::Task*> tasks;
		QString classClass;
		QPixmap icon;
		bool active;
		
	public:
		TaskGroup( TaskManager::Task* task, QGraphicsItem* parent );
		
		bool addTask( TaskManager::Task* task );
		bool removeTask( TaskManager::Task* task );
		bool removeGroup() const{ return tasks.size() == 0; }
		
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget=nullptr);
		
		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*) override{ active = true; update(); }
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override{ active = false; update(); }
		virtual void mousePressEvent(QGraphicsSceneMouseEvent*) override;
};

#endif
