
#include "extraWidgets.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QCalendarWidget>
#include <QApplication>
#include <QDesktopWidget>


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

static void positionPopup( QWidget& parent, QWidget& popup ){
	//TODO: Check if this is correct way of getting the global position
	auto top_widget = qApp->topLevelAt( parent.pos() );
	auto parent_pos = top_widget->mapToGlobal( parent.pos() );
	
	//Center window on the parent
	QPoint pos(
			 parent_pos.x() + parent.width() /2 - popup.width() /2
		,	 parent_pos.y() + parent.height()/2 - popup.height()/2 );
	
	//Make sure it keeps within the available space
	//NOTE: we already reserved the space, so it will position itself beside the parent
	auto space = qApp->desktop()->availableGeometry( &parent );
	pos.setX( std::min( pos.x(), space.x() + space.width()  - popup.width()  ) );
	pos.setY( std::min( pos.y(), space.y() + space.height() - popup.height() ) );
	pos.setX( std::max( pos.x(), space.x() ) );
	pos.setY( std::max( pos.y(), space.y() ) );
	
	popup.move( pos );
	
}

void ClockWidget::mousePressEvent( QMouseEvent* ){
	Calendar cal( this );
	cal.show();
	positionPopup( *this, cal );
	cal.exec();
}