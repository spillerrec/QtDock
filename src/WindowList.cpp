
#include "WindowList.hpp"

#include "TaskManager.hpp"

#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QKeyEvent>

class WindowItem : public QLabel {
	private:
		Window* w;
		WindowList* parent;
	
	public:
		WindowItem( Window* w, WindowList* parent )
			:	QLabel(parent), w(w), parent(parent) {
			setText( w->getTitle() );
			setFocusPolicy( Qt::StrongFocus );
			setStyleSheet( "*:focus{ background:palette(highlight); color:palette(highlighted-text) }" );
		}
		
		void activate(){
			w->activate();
			parent->hide();
		}
		
	protected:
		virtual void mouseReleaseEvent( QMouseEvent* ) override
			{ activate(); }
		
		virtual void enterEvent( QEvent* ) override
			{ setFocus(); }
			
		virtual void keyReleaseEvent( QKeyEvent* event ) override{
			if( event->key() == Qt::Key_Return )
				activate();
		}
};


WindowList::WindowList( QWidget* parent ) : QWidget(parent){
	setWindowFlags( Qt::Popup );
	hide();
	
	setLayout( new QVBoxLayout( this ) );
	layout()->setContentsMargins( 4, 4, 4, 4 );
	layout()->addWidget( title = new QLabel( this ) );
	title->setAlignment( Qt::AlignHCenter );
	
	auto frame = new QFrame( this );
	frame->setFrameShape( QFrame::HLine );
	layout()->addWidget( frame );
}

void WindowList::changeGroup( TaskGroup* new_group ){
	group = new_group;
	
	for( auto& win : windows ){
		layout()->removeWidget( win );
		win->deleteLater();
	}
	windows.clear();
	
	if( group ){
		title->setText( "<b>" + group->getApp().class_name + "</b>" );
		
		for( auto& window : group->getWindows() ){
			if( window.isVisible() ){
				auto item = new WindowItem( &window, this );
				layout()->addWidget( item );
				windows << item;
			}
		}
	}
	
	update();
}

