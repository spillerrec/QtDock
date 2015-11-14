
#include "TaskBar.hpp"

#include "extraWidgets.hpp"
#include "TrayWidget.hpp"
#include "TaskManager.hpp"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QMenu>
#include <QMouseEvent>

#include <KGlobalAccel>


static bool registerShortcut( TaskBar& bar, int index ){
	//TODO: do shift as well
	QKeySequence keys( Qt::META + Qt::Key_1 + index );
	
	auto shortcut = new QAction( &bar );
	shortcut->setShortcut( keys );
	shortcut->setObjectName( "QtDockShortcut" + QString::number(index+1) );
	
	QObject::connect( shortcut, &QAction::triggered, [&,index](){ bar.activate( index ); } );
	return KGlobalAccel::setGlobalShortcut( shortcut, keys );
}

TaskBar::TaskBar( QWidget* parent ) : QWidget(parent), settings( "spillerrec", "QtDock" ) {
	manager = new TaskManager( *this );
	auto clock = new ClockWidget( *this );
        auto tray = new TrayWidget( *this );
	widgets.push_back( manager );
	widgets.push_back( clock );
        widgets.push_back( tray );
	
	auto boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
	boxlayout->setContentsMargins( 0,0,0,0 );
	setLayout( boxlayout );
	boxlayout->addWidget( manager );
	boxlayout->addStretch();
        boxlayout->addWidget( tray );
	boxlayout->addWidget( clock );
	
	setWindowFlags( Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
	KWindowSystem::setOnAllDesktops( winId(), true );
	
	auto screen = QApplication::desktop()->availableGeometry();
	move( screen.topLeft() );
	resize( 36, screen.height() );
	
	//TODO: figure out which edge widget is on, or pass it along
	KWindowSystem::setExtendedStrut( winId()
		, width(), x(), x()+height()
		, 0,0,0, 0,0,0, 0,0,0 );
	KWindowSystem::setType( effectiveWinId(), NET::Dock );
	
	//Register global short-cuts
	for( unsigned i=0; i<9; ++i )
		if( !registerShortcut( *this, i ) )
			qDebug() << "Error registering shortcut META:" << i;
}

void TaskBar::activate( int pos, bool shift ){
	//TODO:
	manager->activate( pos, shift );
}

void TaskBar::mousePressEvent( QMouseEvent* event ){
	if( event->button() == Qt::RightButton ){
		QMenu menu( this );
		connect( menu.addAction( "Exit" ), &QAction::triggered
			, [this](){ this->close(); } );
		menu.exec( event->globalPos() );
	}
	else
		event->ignore();
}
