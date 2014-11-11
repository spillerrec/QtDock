
#include "TaskBar.hpp"

#include "extraWidgets.hpp"
#include "TaskManager.hpp"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QX11Info>

#include <KGlobalAccel>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_atom.h>

#include <string>

class StructPartial {
	private:
		uint32_t state[12];
		
	public:
		StructPartial(){ for( auto& a : state ) a = 0; }
		void reserveLeft( unsigned width, unsigned start, unsigned end ){
			state[0] = width;
			state[4] = start;
			state[5] = end;
		}
		void reserveRight( unsigned width, unsigned start, unsigned end ){
			state[1] = width;
			state[6] = start;
			state[7] = end;
		}
		void reserveTop( unsigned width, unsigned start, unsigned end ){
			state[3] = width;
			state[8] = start;
			state[9] = end;
		}
		void reserveBottom( unsigned width, unsigned start, unsigned end ){
			state[4] = width;
			state[10] = start;
			state[11] = end;
		}
};

static xcb_atom_t getAtomType( std::string name ){
	auto conn = QX11Info::connection();
	
	auto atom_cookie = xcb_intern_atom( conn, 1, name.size(), name.c_str() );
	auto reply = xcb_intern_atom_reply( conn, atom_cookie, nullptr );
	
	if( reply ){
		auto atom = reply->atom;
		free( reply );
		return atom;
	}
	else{
		qDebug() << "Failed to get atom: " << name.c_str();
		return XCB_NONE;
	}
}

void registerDockbar( QWidget& widget ){
	//TODO: figure out which edge widget is on, or pass it along
	StructPartial space;
	space.reserveLeft( widget.width(), widget.x(), widget.x()+widget.height() );
	
	auto conn = QX11Info::connection();
	
	auto struct_cookie = xcb_change_property(
			conn, XCB_PROP_MODE_REPLACE, widget.winId()
		,	getAtomType( "_NET_WM_STRUT_PARTIAL" ), XCB_ATOM_CARDINAL
		,	32, 12, &space
		);
	
	auto err = xcb_request_check( conn, struct_cookie );
	if( err )
		qDebug() << err->error_code;
	
	auto window_type_atom = getAtomType( "_NET_WM_WINDOW_TYPE" );
	auto window_dock_atom = getAtomType( "_NET_WM_WINDOW_TYPE_DOCK" );
	auto dock_cookie = xcb_change_property(
			conn, XCB_PROP_MODE_REPLACE, widget.effectiveWinId()
		,	window_type_atom, XCB_ATOM_ATOM
		,	32, 1, (unsigned char*)&window_dock_atom
		);
	
	
	auto err2 = xcb_request_check( conn, dock_cookie );
	if( err2 )
		qDebug() << err2->error_code;
}

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
	registerDockbar( *this );
	
	//Register global short-cuts
	for( unsigned i=0; i<9; ++i )
		if( !registerShortcut( *this, i ) )
			qDebug() << "Error registering shortcut META:" << i;
}

void TaskBar::activate( int pos, bool shift ){
	//TODO:
	manager->activate( pos );
}

	