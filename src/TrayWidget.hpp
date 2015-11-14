#ifndef TRAY_WIDGET_HPP
#define TRAY_WIDGET_HPP

#include "TaskBar.hpp"

#include <QWidget>
#include <QAbstractNativeEventFilter>

struct TrayFilter : public QAbstractNativeEventFilter {
	class TrayWidget& widget;
	TrayFilter( TrayWidget& widget ) : widget(widget) { }
	
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
