#include "extraWidgets.hpp"

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

using namespace std;

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
		
		QString getTitle() const{ return title; }
		
		bool isVisible( int desktop=KWindowSystem::currentDesktop() ) const{
			KWindowInfo info( id, NET::WMDesktop );
			return visible && info.isOnDesktop( desktop );
		}
		
		QPixmap icon() const{ return KWindowSystem::icon( id ); }
		
		void activate(){ KWindowSystem::forceActiveWindow( id ); }
};

class TaskGroup: public QWidget{
	private:
		QByteArray name;
		QPixmap icon;
		vector<Window> windows;
		
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
		
		int amount() const{ return windows.size(); }
		
		int areVisible( int desktop=KWindowSystem::currentDesktop() ) const{
			return count_if( windows.begin(), windows.end(),
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

QPixmap TaskGroup::getIcon(){
	if( icon.isNull() ){
		if( windows.size() > 0 )
			icon = windows.front().icon();
		//TODO: find application icon
	}
	return icon;
}


class TaskManager : public QWidget{
	private:
		QBoxLayout* boxlayout;
	
	private:
		using TaskGroups = map<QByteArray, TaskGroup*>;
		TaskGroups tasks;
		
		void addWindow( WId id );
		void refreshWidgets();
		
	public:
		TaskManager( QWidget* parent=nullptr ) : QWidget( parent ) {
			boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
			boxlayout->setContentsMargins( 0,0,0,0 );
			setLayout( boxlayout );
			
			for( auto wid : KWindowSystem::windows() )
				addWindow( wid );
			
			setGeometry( QRect( 0,0, 200, 200 ) );
		}
		
		const TaskGroups& getTasks() const{ return tasks; }
};

void TaskManager::addWindow( WId id ){
	KWindowInfo info( id, 0, NET::WM2WindowClass );
	//TODO: determine the difference between class and name
	
	auto task_name = info.windowClassName();
	auto task = tasks.find( task_name );
	if( task != tasks.end() )
		task->second->addWindow( id );
	else{
		auto new_task = new TaskGroup( info.windowClassClass(), id, this );
		tasks.insert( { task_name, new_task } );
		layout()->addWidget( new_task );
		if( new_task->areVisible() <= 0 )
			new_task->hide();
	}
};


class TaskBar : public QWidget{
	private:
		TaskManager manager;
		ClockWidget clock;
		
	public:
		TaskBar( QWidget* parent = nullptr ) : QWidget(parent), manager(this), clock(this) {
			auto boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
			boxlayout->setContentsMargins( 0,0,0,0 );
			setLayout( boxlayout );
 			boxlayout->addWidget( &manager );
 			boxlayout->addStretch();
 			boxlayout->addWidget( &clock );
		}
};


int main( int argc, char* argv[] ){
	QApplication app( argc, argv );
	
	TaskBar tasks;
	tasks.show();
	
	return app.exec();
}
