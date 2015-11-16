#ifndef TRAY_WIDGET_HPP
#define TRAY_WIDGET_HPP

#include "TaskBar.hpp"

#include <xcb/xcb_atom.h>

#include <QWidget>
#include <QAbstractNativeEventFilter>

struct TrayFilter : public QAbstractNativeEventFilter {
	class TrayWidget& widget;
	xcb_atom_t atom;
	
	TrayFilter( TrayWidget& widget, xcb_atom_t atom )
		: widget(widget), atom(atom) { }
	
	virtual bool nativeEventFilter( const QByteArray&, void*, long* ) override;
};


class TrayWidget : public TaskBarQWidget<QWidget> {
	Q_OBJECT
	
	private:
            TrayFilter filter;
		
	public:
		TrayWidget( TaskBar& task_bar );
		
		void beginDock( WId id );
};

#endif
