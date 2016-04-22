
#ifndef XCB_HPP
#define XCB_HPP

#include <xcb/xcb.h>

#include <QX11Info>

struct Atom{
	xcb_atom_t atom;
	xcb_intern_atom_cookie_t cookie;
	
	Atom( xcb_connection_t* conn, std::string name, bool only_if_exists=true )
		: cookie( xcb_intern_atom( conn, only_if_exists?1:0, name.size(), name.c_str() ) ) { }
	
	void get( xcb_connection_t* conn ){
		auto reply = xcb_intern_atom_reply( conn, cookie, nullptr );
		atom = reply ? reply->atom : XCB_NONE;
		free( reply );
	}
};

inline xcb_atom_t getAtom( std::string name ){
	auto conn = QX11Info::connection();
	Atom request_dock( conn, name );
	request_dock.get(conn);
	return request_dock.atom;
}

#endif
