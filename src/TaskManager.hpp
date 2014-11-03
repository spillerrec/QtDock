#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP


#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QMouseEvent>

#include <kwindowsystem.h>
#include <kwindowinfo.h>

#include <algorithm>
#include <map>
#include <vector>



class Window{
	private:
		WId id;
		QString title;
		bool visible{ true };
		
	public:
		Window( WId id ) : id(id) {
			KWindowInfo info( id, NET::WMName );
			title = info.name();
		}
		
		bool isId( WId id ) const{ return this->id == id; }
		
		QString getTitle() const{ return title; }
		
		bool isVisible( int desktop=KWindowSystem::currentDesktop() ) const{
			KWindowInfo info( id, NET::WMDesktop );
			return visible && info.isOnDesktop( desktop );
		}
		
		QPixmap icon() const{ return KWindowSystem::icon( id ); }
		
		void activate(){ KWindowSystem::forceActiveWindow( id ); }
};

class TaskGroup: public QWidget{
	Q_OBJECT
	
	private:
		QByteArray name;
		QPixmap icon;
		std::vector<Window> windows;
		
		bool pinned{ false };
		bool hover{ false }, active{ false };
		QPixmap getIcon();
		
	public:
		TaskGroup( QByteArray name, WId window, QWidget* parent )
			:	QWidget(parent), name(name) {
			addWindow( window );
			setMinimumSize( 32,32 );
			setMaximumSize( 32,32 );
		}
		void addWindow( WId id ){ windows.emplace_back( id ); }
		
		bool removeWindow( WId id ){
			windows.erase( std::remove_if( windows.begin(), windows.end()
				,	[=]( const Window& w ){ return w.isId( id ); } )
				,	windows.end() );
			return windows.size() == 0 && !pinned;
		}
		
		int amount() const{ return windows.size(); }
		
		int areVisible( int desktop=KWindowSystem::currentDesktop() ) const{
			return std::count_if( windows.begin(), windows.end(),
					[=]( const Window& w ){ return w.isVisible( desktop ); } );
		}
		
	protected:
		virtual void paintEvent( QPaintEvent* ) override{
			QPainter painter(this);
			painter.setRenderHint( QPainter::SmoothPixmapTransform );
			painter.drawPixmap( 0,0, 32,32, getIcon() );
			
			if( hover )
				painter.drawRect( 1,1, 30,30 );
			
			auto amount = areVisible();
			if( amount > 1 )
				painter.drawText( 20, 28, QString::number( amount ) );
		}
		
		virtual void mouseReleaseEvent( QMouseEvent* event ) override {
			switch( areVisible() ){
				case 0: break;//TODO: open application
				case 1:
						for( auto& window : windows )
							if( window.isVisible() ){
								window.activate();
							}
					break;
				default: break; //TODO: show selection menu
			}
			event->accept();
		}
		
		virtual void enterEvent( QEvent* ) override{
			hover = true;
			update();
		}
		virtual void leaveEvent( QEvent* ) override{
			hover = false;
			update();
		}
};


class TaskManager : public QWidget{
	Q_OBJECT
	
	private:
		QBoxLayout* boxlayout;
	
	private:
		using TaskGroups = std::map<QString, TaskGroup*>;
		TaskGroups tasks;
		
	private slots:
		void addWindow( WId id );
		void removeWindow( WId id );
		
	public:
		TaskManager( QWidget* parent=nullptr ) : QWidget( parent ) {
			boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
			boxlayout->setContentsMargins( 0,0,0,0 );
			setLayout( boxlayout );
			
			for( auto wid : KWindowSystem::windows() )
				addWindow( wid );
			
			setGeometry( QRect( 0,0, 200, 200 ) );
			
			auto system = KWindowSystem::self();
			connect( system, SIGNAL(windowAdded(WId)), this, SLOT(addWindow(WId)) );
			connect( system, SIGNAL(windowRemoved(WId)), this, SLOT(removeWindow(WId)) );
		}
		
		const TaskGroups& getTasks() const{ return tasks; }
};



#endif
