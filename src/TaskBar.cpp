
#include "TaskBar.hpp"

#include "extraWidgets.hpp"
#include "TaskManager.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QX11Info>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_atom.h>

#include <string>

void registerDockbar( QWidget& widget ){
	auto conn = QX11Info::connection();
	
	std::string name = "_NET_WM_STRUT_PARTIAL";
	auto atom_cookie = xcb_intern_atom( conn, 1, name.size(), name.c_str() );
	
	auto reply = xcb_intern_atom_reply( conn, atom_cookie, nullptr );
	if( reply ){
		auto atom = reply->atom;
		free( reply );
		
		
		const uint32_t state[] = {
				32	//left
			,	0	//right
			,	0	//top
			,	0	//bottom
			,	0	//left_start_y
			,	static_cast<uint32_t>( widget.height() )	//left_end_y
			,	0	//right_start_y
			,	0	//right_end_y
			,	0	//top_start_x
			,	0	//top_end_x
			,	0	//bottom_start_x
			,	0	//bottom_end_x
			};
		
		auto struct_cookie = xcb_change_property( conn, XCB_PROP_MODE_REPLACE, widget.winId()
			,	atom
			,	XCB_ATOM_CARDINAL
			, 32
			, 12
			, state
			);
		
		auto err = xcb_request_check( conn, struct_cookie );
		if( err )
			qDebug() << err->error_code;
	}
	else
		qDebug() << "Failed to get _NET_WM_STRUT_PARTIAL atom";
}

TaskBar::TaskBar( QWidget* parent ) : QWidget(parent), settings( "spillerrec", "QtDock" ) {
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
	registerDockbar( *this );
}

	