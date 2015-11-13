#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include "TaskBar.hpp"
#include "TaskGroup.hpp"

#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

#include <kwindowsystem.h>
#include <kwindowinfo.h>

#include <algorithm>
#include <map>
#include <vector>


class TaskManager : public TaskBarQWidget<>{
	Q_OBJECT
	
	private:
		QBoxLayout* boxlayout;
	
	private:
		std::vector<TaskGroup*> tasks;
		
		void add( TaskGroup* group ){
			tasks.push_back( group );
			layout()->addWidget( group );
			connect( group, SIGNAL(pinnedChanged()), this, SLOT(savePinned()) );
		}
		
	private slots:
		void addWindow( WId id );
		void removeWindow( WId id );
		void refresh(){
			for( auto& task : tasks )
				task->refresh();
		}
		void savePinned();
		
	public:
		TaskManager( TaskBar& task_bar );
		
		const std::vector<TaskGroup*>& getTasks() const{ return tasks; }
		
		void showWindowList( TaskGroup* group );
		
		void activate( unsigned pos, bool shift = false );
};



#endif
