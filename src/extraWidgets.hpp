#ifndef EXTRA_WIDGETS_HPP
#define EXTRA_WIDGETS_HPP

#include "TaskBar.hpp"

#include <QLabel>
#include <QTime>
#include <QTimer>


class ClockWidget : public TaskBarQWidget<QLabel> {
	Q_OBJECT
	
	private:
		QTimer t;
		
	public:
		ClockWidget( TaskBar& task_bar ) : TaskBarQWidget<QLabel>(task_bar), t(this) {
			connect( &t, SIGNAL(timeout()), this, SLOT(updateTime()) );
			t.start( 1000 );
			updateTime();
		}
		
	private slots:
		void updateTime(){ setText( QTime::currentTime().toString( "hh:mm" ) ); }
};

#endif
