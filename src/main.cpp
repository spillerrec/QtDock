#include "extraWidgets.hpp"
#include "TaskManager.hpp"


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
		}
};


int main( int argc, char* argv[] ){
	QApplication app( argc, argv );
	
	TaskBar tasks;
	tasks.show();
	
	return app.exec();
}
