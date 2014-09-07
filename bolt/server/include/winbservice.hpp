#pragma once

#include <winservice_base.hpp>
#include <dispatch.hpp>
#include <logger.hpp>


class WBService : public WBServiceBase
{
public:

	WBService(PWSTR pszServiceName,
		BOOL fCanStop = TRUE,
		BOOL fCanShutdown = TRUE,
		BOOL fCanPauseContinue = FALSE);
	virtual ~WBService(void);

protected:
	virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv) override;
	virtual void OnStop() override;

	void ServiceWorkerThread(void);

private:
	std::shared_ptr<Dispatch> g_server_handler;
	BOOL m_fStopping;
	HANDLE m_hStoppedEvent;
	BoltLog m_log_file;
};