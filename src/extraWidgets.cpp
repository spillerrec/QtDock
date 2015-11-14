
#include "extraWidgets.hpp"
#include "WindowList.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QCalendarWidget>


class Calendar : public QDialog{
	private:
		QCalendarWidget cal;
	public:
		Calendar( QWidget* parent ) : QDialog( parent ), cal(this){
			setWindowFlags( Qt::Popup );
			setLayout( new QVBoxLayout( this ) );
			layout()->addWidget( &cal );
		}
};

void ClockWidget::mousePressEvent( QMouseEvent* ){
	Calendar cal( this );
	cal.show();
	positionPopup( *this, cal, {width()/2, height()/2} );
	cal.exec();
}