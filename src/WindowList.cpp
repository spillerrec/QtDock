
#include "WindowList.hpp"

#include "TaskManager.hpp"

#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QBoxLayout>
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>

class WindowItem : public QWidget {
	private:
		Window& w;
		WindowList& parent;
	
	public:
		WindowItem( Window& w, WindowList& parent )
			:	QWidget(&parent), w(w), parent(parent) {
			setLayout( new QHBoxLayout( this ) );
			layout()->setContentsMargins( 0, 0, 0, 0 );
			layout()->addWidget( new QLabel( w.getTitle(), this ) );
			
			auto close_btn = new QPushButton( QIcon::fromTheme("window-close"), "", this );
			close_btn->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
			close_btn->setFocusPolicy( Qt::NoFocus );
			close_btn->setFlat( true );
			layout()->addWidget( close_btn );
			connect( close_btn, &QPushButton::clicked, [&](){ close(); } );
			
			setFocusPolicy( Qt::StrongFocus );
			setStyleSheet( "*:focus{ background:palette(highlight); color:palette(highlighted-text) }" );
		}
		
		void activate(){
			w.activate();
			parent.close();
		}
		
		void close(){
			w.close();
			parent.close();
		}
		
	protected:
		virtual void mouseReleaseEvent( QMouseEvent* event ) override{
			switch( event->button() ){
				case Qt::LeftButton: activate(); break;
				case Qt::MiddleButton: close(); break;
				default: event->ignore();
			}
		}
		
		virtual void enterEvent( QEvent* ) override
			{ setFocus(); }
			
		virtual void keyReleaseEvent( QKeyEvent* event ) override{
			switch( event->key() ){
				case Qt::Key_Return: activate(); break;
				case Qt::Key_Delete: close(); break;
				
				case Qt::Key_Right:
				case Qt::Key_Down:
					focusNextChild(); break;
				
				case Qt::Key_Left:
				case Qt::Key_Up:
					focusPreviousChild(); break;
				
				default: event->ignore();
			}
		}
};


WindowList::WindowList( TaskGroup& group, QWidget* parent ) : QWidget(parent){
	setWindowFlags( Qt::Popup );
	setAttribute( Qt::WA_DeleteOnClose );
	
	setLayout( new QVBoxLayout( this ) );
	layout()->setContentsMargins( 4, 4, 4, 4 );
	
	//Add Header
	auto title = new QLabel( this );
	title->setText( "<b>" + group.getApp().class_name + "</b>" );
	title->setAlignment( Qt::AlignHCenter );
	layout()->addWidget( title );
	
	auto frame = new QFrame( this );
	frame->setFrameShape( QFrame::HLine );
	layout()->addWidget( frame );

	//Add WindowItems
	for( auto& window : group.getWindows() )
		if( window.isVisible() )
			layout()->addWidget( new WindowItem( window, *this ) );
}

void positionPopup( QWidget& parent, QWidget& popup, QPoint parent_offset ){
	auto parent_pos = parent.mapToGlobal( parent_offset );
	
	//Center window on the parent
	QPoint pos(
			 parent_pos.x() - popup.width() /2
		,	 parent_pos.y() - popup.height()/2 );
	
	//Make sure it keeps within the available space
	//NOTE: we already reserved the space, so it will position itself beside the parent
	auto space = qApp->desktop()->availableGeometry( &parent );
	pos.setX( std::min( pos.x(), space.x() + space.width()  - popup.width()  ) );
	pos.setY( std::min( pos.y(), space.y() + space.height() - popup.height() ) );
	pos.setX( std::max( pos.x(), space.x() ) );
	pos.setY( std::max( pos.y(), space.y() ) );
	
	popup.move( pos );
	
}

