#include "extraWidgets.hpp"
#include "TaskManager.hpp"

#include <QDesktopWidget>


class TaskBar : public QWidget{
	private:
		TaskManager manager;
		ClockWidget clock;
		
	public:
		TaskBar( QWidget* parent = nullptr ) : QWidget(parent), manager(this), clock(this) {
			auto boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
			boxlayout->setContentsMargins( 0,0,0,0 );
			setLayout( boxlayout );
 			boxlayout->addWidget( &manager );
 			boxlayout->addStretch();
 			boxlayout->addWidget( &clock );
			
			setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
			setStyleSheet( "* {background: black;}" );
			KWindowSystem::setOnAllDesktops( winId(), true );
			
			auto screen = QApplication::desktop()->availableGeometry();
			move( screen.topLeft() );
			resize( 32, screen.height() );
		}
};


int main( int argc, char* argv[] ){
	QApplication app( argc, argv );
	
	TaskBar tasks;
	tasks.show();
	
	return app.exec();
}
