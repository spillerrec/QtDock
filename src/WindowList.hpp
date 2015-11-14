
#ifndef WINDOW_LIST_HPP
#define WINDOW_LIST_HPP

#include <QWidget>

class WindowList : public QWidget{
	public:
		WindowList( class TaskGroup& group, QWidget* parent=nullptr );
		void initFocus(){ focusNextChild(); }
};

void positionPopup( QWidget& parent, QWidget& popup, QPoint parent_offset );

#endif
