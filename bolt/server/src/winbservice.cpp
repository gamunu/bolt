#pragma once

#include <winbservice.hpp>
#include <threadpool.hpp>
#include <configuration.hpp>

WBService::WBService(PWSTR pszServiceName,
	BOOL fCanStop,
	BOOL fCanShutdown,
	BOOL fCanPauseContinue)
	: WBServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
	m_fStopping = FALSE;

	// Create a manual-reset event that is not signaled at first to indicate
	// the stopped signal of the service.
	m_hStoppedEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (m_hStoppedEvent == nullptr)
	{
		throw GetLastError();
	}
}

WBService::~WBService(void)
{
	if (m_hStoppedEvent)
	{
		CloseHandle(m_hStoppedEvent);
		m_hStoppedEvent = nullptr;
	}
}

/// <summary>
/// The function is executed when a Start command is sent to the
/// service by the SCM or when the operating system starts (for a service
/// that starts automatically). It specifies actions to take when the
/// service starts. In this code sample, OnStart logs a service-start
/// message to the Application log, and queues the main service function for
/// execution in a thread pool worker thread.
///
/// NOTE: A service application is designed to be long running. Therefore,
/// it usually polls or monitors something in the system. The monitoring is
/// set up in the OnStart method. However, OnStart does not actually do the
/// monitoring. The OnStart method must return to the operating system after
/// the service's operation has begun. It must not loop forever or block. To
/// set up a simple monitoring mechanism, one general solution is to create
/// a timer in OnStart. The timer would then raise events in your code
/// periodically, at which time your service could do its monitoring. The
/// other solution is to spawn a new thread to perform the main service
/// functions, which is demonstrated in this code sample.
/// </summary>
/// <param name="dwArgc">number of command line arguments.</param>
/// <param name="lpszArgv">array of command line arguments.</param>
void WBService::OnStart(DWORD dwArgc, LPWSTR *lpszArgv)
{
	m_log_file << BoltLog::LOG_INFO << "WBService in OnStart";
	// Log a service start message to the Application log.
	WriteEventLogEntry(L"WBService in OnStart",
		EVENTLOG_INFORMATION_TYPE);

	// Queue the main service function for execution in a worker thread.
	CThreadPool::QueueUserWorkItem(&WBService::ServiceWorkerThread, this);
}

/// <summary>
/// The method performs the main function of the service. It runs
/// on a thread pool worker thread.
/// </summary>
void WBService::ServiceWorkerThread(void)
{
	// Log a service start message to the Application log.
	m_log_file << BoltLog::LOG_INFO << "WBService in ServiceWorkerThread";
	m_log_file << BoltLog::LOG_INFO << "Setting UP port and address";
	
	string_t address = Config::getInstance().getServerHostWithPort();

	g_server_handler = shared_ptr<Dispatch>(new Dispatch(address));
	g_server_handler->open().wait();

	m_log_file << BoltLog::LOG_INFO << "Listing to port requests at " + conversions::to_utf8string(address);

	// Periodically check if the service is stopping.
	// and Keep running http_listner
	while (!m_fStopping) {
		Sleep(2000);
	}

	// Log a service start message to the Application log.
	m_log_file << BoltLog::LOG_INFO << "WBService exiting ServiceWorkerThread";

	// Signal the stopped event.
	SetEvent(m_hStoppedEvent);
}

/// <summary>
/// The function is executed when a Stop command is sent to the
/// service by SCM. It specifies actions to take when a service stops
/// running. In this code sample, OnStop logs a service-stop message to the
/// Application log, and waits for the finish of the main service function.
///
/// COMMENTS:
/// Be sure to periodically call ReportServiceStatus() with
/// SERVICE_STOP_PENDING if the procedure is going to take long time.
/// </summary>
void WBService::OnStop()
{
	g_server_handler->close().wait();
	// Log a service stop message to the Application log.
	m_log_file << BoltLog::LOG_INFO << "WBService in OnStop";

	// Indicate that the service is stopping and wait for the finish of the
	// main service function (ServiceWorkerThread).
	m_fStopping = TRUE;
	if (WaitForSingleObject(m_hStoppedEvent, INFINITE) != WAIT_OBJECT_0)
	{
		throw GetLastError();
	}
}