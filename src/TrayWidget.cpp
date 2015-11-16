
#include "TrayWidget.hpp"
#include "TaskGroup.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QCalendarWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QApplication>
#include <QX11Info>
#include <QWindow>
#include <QVBoxLayout>
#include <QLabel>

#include <KSelectionOwner>
#include <netwm.h>

#include <QDebug>

#include <string>
#include <vector>

#include <xcb/xcb.h>
#include <xcb/xproto.h>

/*
struct x_mon{
	xcb_atom_t atom;
	xcb_intern_atom_cookie_t atom_cookie;
	xcb_get_property_cookie_t prop_cookie;
	x_mon( xcb_intern_atom_cookie_t atom ) : atom_cookie(atom) { }
};
*/

struct Atom{
	xcb_atom_t atom;
	xcb_intern_atom_cookie_t cookie;
	
	Atom( xcb_connection_t* conn, std::string name, bool only_if_exists=true )
		: cookie( xcb_intern_atom( conn, only_if_exists?1:0, name.size(), name.c_str() ) ) { }
	
	void get( xcb_connection_t* conn ){
		auto reply = xcb_intern_atom_reply( conn, cookie, nullptr );
		atom = reply ? reply->atom : XCB_NONE;
		free( reply );
	}
};

bool TrayFilter::nativeEventFilter( const QByteArray& event_type, void* message, long* ){
	if( event_type == "xcb_generic_event_t" ){
		auto event = static_cast<xcb_generic_event_t*>(message);
		if( event->response_type == XCB_CLIENT_MESSAGE || event->response_type == (XCB_CLIENT_MESSAGE|128) ){
			auto client = static_cast<xcb_client_message_event_t*>(message);
			qDebug() << "Client type is:" << client->type;
			
			if( client->type == atom ){
				auto opcode = client->data.data32[1];
				auto wid    = client->data.data32[2];
				if( opcode == 0 )
					widget.beginDock( wid );
				return true;
			}
		}
		
		
	}
	else
		qDebug() << "Unknown event received" << event_type;
	return false;
}

xcb_atom_t debugAtom( xcb_connection_t* conn, std::string name ){
	Atom request_dock( conn, name );
	request_dock.get(conn);
	qDebug() << "atom opcode is:" << request_dock.atom;
	return request_dock.atom;
}

TrayWidget::TrayWidget( TaskBar& task_bar ) : TaskBarQWidget<QWidget>(task_bar), filter(*this) {
	setLayout( new QVBoxLayout( this ) );
	setContentsMargins( 0,0,0,0 );
	layout()->setContentsMargins( 0,0,0,0 );
	qApp->installNativeEventFilter( &filter );
	
	
	
	
	xcb_connection_t *conn = QX11Info::connection();
	int monitor_amount = QApplication::desktop()->screenCount();
	
	//Get cookies for atoms
	std::vector<Atom> x_mons;
	x_mons.reserve( monitor_amount );
	for( int i=0; i<monitor_amount; i++ )
		x_mons.emplace_back( conn, "_NET_SYSTEM_TRAY_S" + std::to_string( i ) );
	
	for( auto& atom : x_mons )
		atom.get( conn );
	
	filter.atom = debugAtom( conn, "_NET_SYSTEM_TRAY_OPCODE" );
	//debugAtom( conn, "_NET_SYSTEM_TRAY_OPCODE" );
	
	auto tray_selection = new KSelectionOwner( x_mons[0].atom, -1, this );
	
	connect( tray_selection, &KSelectionOwner::claimedOwnership, [](){ qDebug() << "Got selection"; } );
	connect( tray_selection, &KSelectionOwner::failedToClaimOwnership, [](){ qDebug() << "Did not get selection"; } );
	
	tray_selection->claim( false );
}


void TrayWidget::beginDock( WId id ){
	qDebug() << "Attemting to embed:" << id;
	auto window = QWindow::fromWinId( id );
	window->resize( 16, 16 );
	
	auto widget = QWidget::createWindowContainer( window, this );
	widget->setMaximumSize( 16,16 );
	widget->setMinimumSize( 16,16 );
	layout()->addWidget( widget );
	
	qDebug() << widget->minimumSize();
	qDebug() << widget->maximumSize();
}
