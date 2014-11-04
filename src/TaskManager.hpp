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


class Application{
	public:
		QString app_path;
		QByteArray class_name;
		QString working_dir;
		
		Application(){ }
		Application( WId id );
		Application( QString name ) : app_path(name), class_name(name.toLocal8Bit()) { }
};

Q_DECLARE_METATYPE(Application)
Q_DECLARE_METATYPE(QList<Application>)


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
		Application app;
		QPixmap icon;
		std::vector<Window> windows;
		
		bool pinned{ false };
		bool hover{ false }, active{ false };
		
		QPixmap getIcon();
		void startApplication();
		
		TaskGroup( TaskBar& task_bar );
		
	public:
		TaskGroup( WId window, TaskBar& task_bar );
		TaskGroup( Application application, TaskBar& task_bar );
		void addWindow( WId id ){ windows.emplace_back( id ); refresh(); }
		
		bool removeWindow( WId id ){
			windows.erase( std::remove_if( windows.begin(), windows.end()
				,	[=]( const Window& w ){ return w.isId( id ); } )
				,	windows.end() );
			refresh();
			return windows.size() == 0 && !pinned;
		}
		
		bool isPinned() const{ return pinned; }
		Application getApp() const{ return app; }
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
		
	signals:
		void pinnedChanged();
};


class TaskManager : public TaskBarQWidget<>{
	Q_OBJECT
	
	private:
		QBoxLayout* boxlayout;
	
	private:
		using TaskGroups = std::map<QString, TaskGroup*>;
		TaskGroups tasks;
		
		void add( QString name, TaskGroup* group ){
			tasks.insert( { name, group } );
			layout()->addWidget( group );
			connect( group, SIGNAL(pinnedChanged()), this, SLOT(savePinned()) );
		}
		
	private slots:
		void addWindow( WId id );
		void removeWindow( WId id );
		void refresh(){
			for( auto& task : tasks )
				task.second->refresh();
		}
		void savePinned();
		
	public:
		TaskManager( TaskBar& task_bar );
		
		const TaskGroups& getTasks() const{ return tasks; }
};



#endif
