#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QWidget>

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
		
		bool isVisible( int desktop ) const{
			KWindowInfo info( id, NET::WMDesktop );
			return visible && info.isOnDesktop( desktop );
		}
		
		QPixmap icon() const{ return KWindowSystem::icon( id ); }
};

class TaskGroup: public QWidget{
	private:
		QByteArray name;
		//QPixmap icon;
		vector<Window> windows;
		
	public:
		TaskGroup( QByteArray name, WId window, QWidget* parent )
			:	QWidget(parent), name(name)//, icon( KWindowSystem::icon( window ) )
			{
				addWindow( window );
				//setPixmap( icon );
				setMaximumSize( 32,32 );
			}
		void addWindow( WId id ){ windows.emplace_back( id ); }
		
		int amount() const{ return windows.size(); }
		
		int areVisible( int desktop ) const{
			return count_if( windows.begin(), windows.end(),
					[=]( const Window& w ){ return w.isVisible( desktop ); } );
		}
		
	protected:
		void paintEvent( QPaintEvent* e ){
			QPainter painter(this);
			painter.setRenderHint( QPainter::SmoothPixmapTransform );
			painter.drawPixmap( 0,0, 32,32, windows[0].icon() );
		}
};

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
			setLayout( boxlayout );
			
			for( auto wid : KWindowSystem::windows() )
				addWindow( wid );
			
			setGeometry( QRect( 0,0, 200, 200 ) );
		}
		
		
		void list(){
			for( auto& task : tasks ){
				if( task.second->areVisible(2) > 0 )
					qDebug() << task.first ;
			}
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
		if( new_task->areVisible( 2 ) <= 0 )
			new_task->hide();
	}
};


int main( int argc, char* argv[] ){
	QApplication app( argc, argv );
	auto amount = KWindowSystem::numberOfDesktops();
	qDebug() << amount;
	for( int i=1; i<=amount; i++ )
		qDebug() << KWindowSystem::desktopName(i);
	
	TaskManager tasks;
	tasks.show();
	
	tasks.list();
	
	
	return app.exec();
}
