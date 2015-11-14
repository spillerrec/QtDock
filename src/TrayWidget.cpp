
#include "TrayWidget.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QCalendarWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractNativeEventFilter>
#include <QApplication>

#include <QDebug>

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_atom.h>

struct x_mon{
	xcb_atom_t atom;
	xcb_intern_atom_cookie_t atom_cookie;
	xcb_get_property_cookie_t prop_cookie;
	x_mon( xcb_intern_atom_cookie_t atom ) : atom_cookie(atom) { }
};

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

class TrayFilter : public QAbstractNativeEventFilter {
	public:
		virtual bool nativeEventFilter( const QByteArray& event_type
			,	void* message, long* ) override {
			if( event_type == "xcb_generic_event_t" ){
				auto event = static_cast<xcb_generic_event_t*>(message);
				
			}
			else
				qDebug() << "Unknown event received" << event_type;
			return false;
		}
} tray_filter;

TrayWidget::TrayWidget( TaskBar& task_bar ) : TaskBarQWidget<QWidget>(task_bar) {
	qApp->installNativeEventFilter( &tray_filter );
	
//	xcb_connection_t *conn = QX11Info::connection();
//	xcb_window_t window = QX11Info::appRootWindow();
//	int monitor_amount = QApplication::desktop()->screenCount();
	
	/*
	//Get cookies for atoms
	vector<Atom> x_mons;
	x_mons.reserve( monitor_amount );
	for( int i=0; i<monitor_amount; i++ )
		x_mons.emplace_back( conn, "_NET_SYSTEM_TRAY_S" + to_string( i ) );
	
	for( auto& atom : x_mons )
		atom.get();
	
	
		//Get property cookies
		for( int i=0; i<monitor_amount; i++ )
			x_mons[i].prop_cookie = xcb_get_property( conn, 0, window, x_mons[i].atom, XCB_ATOM_CARDINAL, 0, UINT_MAX );
		
		//Load profiles
		vector<colorManager::MonitorIcc> iccs;
		iccs.reserve( monitor_amount );
		for( int i=0; i<monitor_amount; i++ ){
			xcb_get_property_reply_t *reply = xcb_get_property_reply( conn, x_mons[i].prop_cookie, NULL );
			
			colorManager::MonitorIcc icc( NULL );
			if( reply ){
				icc.profile = cmsOpenProfileFromMem( xcb_get_property_value( reply ), reply->length );
				free( reply );
			}
			iccs.push_back( icc );
		}
	*/
}