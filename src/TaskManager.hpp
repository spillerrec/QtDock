#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include "TaskBar.hpp"

#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QWidget>

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
		Window( WId id );
		
		WId getId() const{ return id; }
		bool isId( WId id ) const{ return this->id == id; }
		
		QString getTitle() const{ return title; }
		
		bool isVisible( int desktop=KWindowSystem::currentDesktop() ) const{
			KWindowInfo info( id, NET::WMDesktop );
			return visible && info.isOnDesktop( desktop );
		}
		
		QPixmap icon() const{ return KWindowSystem::icon( id ); }
		
		void activate(){ KWindowSystem::forceActiveWindow( id ); }
};

class TaskGroup: public TaskBarQWidget<>{
	Q_OBJECT
	
	private:
		QByteArray name;
		QPixmap icon;
		std::vector<Window> windows;
		
		bool pinned{ false };
		bool hover{ false }, active{ false };
		
		QPixmap getIcon();
		void startApplication();
		
	public:
		TaskGroup( QByteArray name, WId window, TaskBar& task_bar )
			:	TaskBarQWidget<>( task_bar ), name(name) {
			addWindow( window );
			setMinimumSize( 32,32 );
			setMaximumSize( 32,32 );
		}
		void addWindow( WId id ){ windows.emplace_back( id ); refresh(); }
		
		bool removeWindow( WId id ){
			windows.erase( std::remove_if( windows.begin(), windows.end()
				,	[=]( const Window& w ){ return w.isId( id ); } )
				,	windows.end() );
			refresh();
			return windows.size() == 0 && !pinned;
		}
		
		void refresh();
		int amount() const{ return windows.size(); }
		
		int areVisible( int desktop=KWindowSystem::currentDesktop() ) const{
			return std::count_if( windows.begin(), windows.end(),
					[=]( const Window& w ){ return w.isVisible( desktop ); } );
		}
		
	protected:
		virtual void paintEvent( QPaintEvent* event ) override;
		virtual void mouseReleaseEvent( QMouseEvent* event ) override;
		
		virtual void enterEvent( QEvent* ) override{
			hover = true;
			update();
		}
		virtual void leaveEvent( QEvent* ) override{
			hover = false;
			update();
		}
};


class TaskManager : public TaskBarQWidget<>{
	Q_OBJECT
	
	private:
		QBoxLayout* boxlayout;
	
	private:
		using TaskGroups = std::map<QString, TaskGroup*>;
		TaskGroups tasks;
		
	private slots:
		void addWindow( WId id );
		void removeWindow( WId id );
		void refresh(){
			for( auto& task : tasks )
				task.second->refresh();
		}
		
	public:
		TaskManager( TaskBar& task_bar ) : TaskBarQWidget<>( task_bar ) {
			boxlayout = new QBoxLayout( QBoxLayout::TopToBottom, this );
			boxlayout->setContentsMargins( 0,0,0,0 );
			setLayout( boxlayout );
			
			for( auto wid : KWindowSystem::windows() )
				addWindow( wid );
			
			auto system = KWindowSystem::self();
			connect( system, SIGNAL(windowAdded(WId)), this, SLOT(addWindow(WId)) );
			connect( system, SIGNAL(windowRemoved(WId)), this, SLOT(removeWindow(WId)) );
			connect( system, SIGNAL(currentDesktopChanged(int)), this, SLOT(refresh()) );
		}
		
		const TaskGroups& getTasks() const{ return tasks; }
};



#endif
