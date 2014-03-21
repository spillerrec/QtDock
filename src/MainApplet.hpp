#ifndef MAIN_APPLET
#define MAIN_APPLET

//stl
#include <vector>

//Plasma
#include <Plasma/Applet>

#include<QGraphicsLinearLayout>

//Plasma libtaskmanager
#include <taskmanager/taskmanager.h>

//Own
class TaskGroup;

class MainApplet : public Plasma::Applet{
	Q_OBJECT
	private:
		TaskManager::TaskManager manager;
		std::vector<TaskGroup*> groups;
		QGraphicsLinearLayout* layout;
		
	public:
		MainApplet(QObject *parent, const QVariantList& args);
		
		void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect& contentsRect);
		
		void init();
		
		virtual Plasma::FormFactor formFactor() const override{ return Plasma::Horizontal; }
		
	private slots:
		void addTask(::TaskManager::Task* task);
		void removeTask(::TaskManager::Task* task);
};

#endif
