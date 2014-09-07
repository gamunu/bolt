#pragma once

#include <cpprest/http_listener.h>

using namespace std;

class Dispatch {
public:
	Dispatch();
	~Dispatch();

	Dispatch(utility::string_t url);

	pplx::task<void> open() { return m_listener.open(); }
	pplx::task<void> close() { return m_listener.close(); }
private:

	class DispatchImpl;
	std::shared_ptr<DispatchImpl> m_impl;
	web::http::experimental::listener::http_listener m_listener;
};