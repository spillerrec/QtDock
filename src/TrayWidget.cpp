#include "TrayWidget.hpp"

#include "Xcb.hpp"

#include <KSelectionOwner>

#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QWindow>

#include <string>


//Handles the request-dock messages
//TODO: Handle balloon messages as well?
bool TrayFilter::nativeEventFilter( const QByteArray& event_type, void* message, long* ){
	if( event_type == "xcb_generic_event_t" ){
		auto event = static_cast<xcb_generic_event_t*>(message);
		if( event->response_type == XCB_CLIENT_MESSAGE || event->response_type == (XCB_CLIENT_MESSAGE|128) ){
			//TODO: I'm not exactly sure how the masking works, we might be missing something
			auto client = static_cast<xcb_client_message_event_t*>(message);
			
			if( client->type == atom ){
				auto opcode = client->data.data32[1];
				auto wid    = client->data.data32[2];
				if( opcode == 0 ){
					widget.beginDock( wid );
					return true;
				}
				else
					qDebug() << "Not handling system tray opcode:" << opcode;
			}
		}
	}
	
	return false;
}

TrayWidget::TrayWidget( TaskBar& task_bar )
	:	TaskBarQWidget<QWidget>(task_bar)
	,	filter( *this, getAtom("_NET_SYSTEM_TRAY_OPCODE") ) {
		
	setLayout( new QVBoxLayout( this ) );
	layout()->setContentsMargins( 0,0,0,0 );
	
	
	//Get system tray selection
	auto tray_selection = new KSelectionOwner( getAtom("_NET_SYSTEM_TRAY_S0"), -1, this );
	//TODO: what does system trays on other monitors mean?
	
	connect( tray_selection, &KSelectionOwner::claimedOwnership
		,	[&](){ qApp->installNativeEventFilter( &filter ); }
		); //NOTE: Can we sure this will happen before the first requests?
	
	connect( tray_selection, &KSelectionOwner::failedToClaimOwnership
		,	[](){ qDebug() << "Could not get X selection for the system tray"; }
		);
	
	tray_selection->claim( false );
}


void TrayWidget::beginDock( WId id ){
	qDebug() << "Attemting to embed tray:" << id;
	auto window = QWindow::fromWinId( id );
	window->resize( 16, 16 );
	
	auto widget = QWidget::createWindowContainer( window, this );
	widget->setMaximumSize( 16,16 );
	widget->setMinimumSize( 16,16 );
	layout()->addWidget( widget );
}
