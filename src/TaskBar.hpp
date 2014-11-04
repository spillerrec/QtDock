
#ifndef TASK_BAR_HPP
#define TASK_BAR_HPP

#include "extraWidgets.hpp"
#include "TaskManager.hpp"

#include <QDesktopWidget>


class TaskBar : public QWidget{
	private:
		TaskManager manager;
		ClockWidget clock;
		
	public:
		TaskBar( QWidget* parent = nullptr );
};


#endif
