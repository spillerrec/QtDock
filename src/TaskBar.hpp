
#ifndef TASK_BAR_HPP
#define TASK_BAR_HPP

#include <QDesktopWidget>
#include <QSettings>

#include <vector>

class TaskBar;

class TaskBarWidget{
	private:
		TaskBar* task_bar;
	
	public:
		TaskBarWidget( TaskBar& task_bar ) : task_bar(&task_bar) { }
		
		TaskBar& taskBar(){ return *task_bar; }
};

class TaskBar : public QWidget{
	private:
		std::vector<TaskBarWidget*> widgets;
		class TaskManager* manager;
		QSettings settings;
		
	public:
		TaskBar( QWidget* parent = nullptr );
		
		QSettings& getSettings(){ return settings; }
		
	public slots:
		void activate( int pos, bool shift=false );
};

///Convenience function when using QWidget
template<class T=QWidget>
class TaskBarQWidget : public T, public TaskBarWidget{
	public:
		TaskBarQWidget( TaskBar& task_bar ) : T(&task_bar), TaskBarWidget(task_bar) { }
};



#endif
