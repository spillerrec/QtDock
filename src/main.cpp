#include "TaskBar.hpp"

#include <QApplication>

int main( int argc, char* argv[] ){
	QApplication app( argc, argv );
	
	TaskBar tasks;
	tasks.show();
	
	return app.exec();
}
