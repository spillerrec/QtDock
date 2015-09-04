#ifndef EXTRA_WIDGETS_HPP
#define EXTRA_WIDGETS_HPP

#include "TaskBar.hpp"

#include <QLabel>
#include <QDateTime>
#include <QTimer>


class ClockWidget : public TaskBarQWidget<QLabel> {
	Q_OBJECT
	
	private:
		QTimer t;
		
	public:
		ClockWidget( TaskBar& task_bar ) : TaskBarQWidget<QLabel>(task_bar), t(this) {
			setAlignment( Qt::AlignHCenter );
			connect( &t, SIGNAL(timeout()), this, SLOT(updateTime()) );
			t.start( 1000 );
			updateTime();
		}
		
		virtual void mousePressEvent( QMouseEvent* event ) override;
		
	private slots:
		void updateTime(){
			setText( QDateTime::currentDateTime().toString( "hh:mm\nM/dd" ) );
		}
};

#endif
