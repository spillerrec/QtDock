
#ifndef WINDOW_LIST_HPP
#define WINDOW_LIST_HPP

#include <QWidget>
#include <QList>

class TaskGroup;
class WindowItem;

class QLabel;

class WindowList : public QWidget{
	private:
		QList<WindowItem*> windows;
	
	public:
		WindowList( TaskGroup& group, QWidget* parent=nullptr );
};

void positionPopup( QWidget& parent, QWidget& popup, QPoint parent_offset );

#endif
