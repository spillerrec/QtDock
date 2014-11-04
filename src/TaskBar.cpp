
#include "TaskBar.hpp"

#include "extraWidgets.hpp"
#include "TaskManager.hpp"

#include <QApplication>
#include <QBoxLayout>

TaskBar::TaskBar( QWidget* parent ) : QWidget(parent) {
	auto manager = new TaskManager( *this );
	auto clock = new ClockWidget( *this );
	widgets.push_back( manager );
	widgets.push_back( clock );
	
	auto boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
	boxlayout->setContentsMargins( 0,0,0,0 );
	setLayout( boxlayout );
	boxlayout->addWidget( manager );
	boxlayout->addStretch();
	boxlayout->addWidget( clock );
	
	setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
	setStyleSheet( "* {background: black;}" );
	KWindowSystem::setOnAllDesktops( winId(), true );
	
	auto screen = QApplication::desktop()->availableGeometry();
	move( screen.topLeft() );
	resize( 32, screen.height() );
}

	