#ifndef EXTRA_WIDGETS_HPP
#define EXTRA_WIDGETS_HPP

#include <QLabel>
#include <QTime>
#include <QTimer>


class ClockWidget : public QLabel {
	Q_OBJECT
	
	private:
		QTimer t;
		
	public:
		ClockWidget( QWidget* parent ) : QLabel(parent), t(this) {
			connect( &t, SIGNAL(timeout()), this, SLOT(updateTime()) );
			t.start( 1000 );
			updateTime();
		}
		
	private slots:
		void updateTime(){ setText( QTime::currentTime().toString( "hh:mm" ) ); }
};

#endif
