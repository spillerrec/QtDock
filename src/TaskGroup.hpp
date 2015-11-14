#ifndef TASK_GROUP_HPP
#define TASK_GROUP_HPP

#include "TaskBar.hpp"

#include <QPixmap>
#include <QWidget>

#include <kwindowsystem.h>
#include <kwindowinfo.h>

#include <vector>


class Application{
	public:
		QString app_path;
		QByteArray class_name;
		QString working_dir;
		QPixmap icon;
		
		Application(){ }
		Application( WId id );
		Application( QString name ) : app_path(name), class_name(name.toLocal8Bit()) { }
};

Q_DECLARE_METATYPE(Application)
Q_DECLARE_METATYPE(QList<Application>)


class Window{
	private:
		WId id;
		bool visible{ true };
		
	public:
		Window( WId id );
		
		WId getId() const{ return id; }
		bool isId( WId id ) const{ return this->id == id; }
		
		QString getTitle() const;
		
		bool isVisible( int desktop=KWindowSystem::currentDesktop() ) const{
			KWindowInfo info( id, NET::WMDesktop );
			return visible && info.isOnDesktop( desktop );
		}
		
		QPixmap icon() const{ return KWindowSystem::icon( id ); }
		
		bool isActive() const{ return KWindowSystem::activeWindow() == id; }
		void activate(){ KWindowSystem::forceActiveWindow( id ); }
		void minimize(){ KWindowSystem::minimizeWindow( id ); }
		void close(){ qDebug( "Window::close(), not yet implemented :(" ); } //TODO: Implement this method
};

class TaskGroup: public TaskBarQWidget<>{
	Q_OBJECT
	
	private:
		class TaskManager& manager;
		Application app;
		std::vector<Window> windows;
		
		bool pinned{ false };
		bool active{ false };
		
		QPixmap getIcon();
		void startApplication();
		
		TaskGroup( TaskManager& manager );
		
	public:
		TaskGroup( WId window, TaskManager& manager );
		TaskGroup( Application application, TaskManager& manager );
		void addWindow( WId id ){ windows.emplace_back( id ); refresh(); }
		
		bool removeWindow( WId id ){
			windows.erase( std::remove_if( windows.begin(), windows.end()
				,	[=]( const Window& w ){ return w.isId( id ); } )
				,	windows.end() );
			refresh();
			return windows.size() == 0 && !pinned;
		}
		
		void activate( Qt::KeyboardModifiers mods = Qt::NoModifier );
		
		std::vector<Window>& getWindows(){ return windows; }
		
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
		virtual void mousePressEvent( QMouseEvent* event ) override;
		virtual void moveEvent( QMoveEvent* ) override { }
		virtual void mouseReleaseEvent( QMouseEvent* event ) override;
		
	signals:
		void pinnedChanged();
};

#endif
