#ifndef TRAY_WIDGET_HPP
#define TRAY_WIDGET_HPP

#include "TaskBar.hpp"

#include <QWidget>


class TrayWidget : public TaskBarQWidget<QWidget> {
	Q_OBJECT
	
	private:
            
		
	public:
		TrayWidget( TaskBar& task_bar );
		
};

#endif
