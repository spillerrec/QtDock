
#ifndef WINDOW_LIST_HPP
#define WINDOW_LIST_HPP

#include <QWidget>
#include <QList>

class TaskGroup;
class WindowItem;

class QLabel;

class WindowList : public QWidget{
	private:
		TaskGroup* group{ nullptr };
		QList<WindowItem*> windows;
		
		QLabel* title;
	
	public:
		WindowList( QWidget* parent );
		
		void changeGroup( TaskGroup* new_group );
};

void positionPopup( QWidget& parent, QWidget& popup, QPoint parent_offset );

#endif
