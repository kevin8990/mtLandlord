#pragma once 

class	mtProcotolMonitorMgr
{
public:
	struct	DataInit
	{
		long		lStructBytes;
	};
public:
	mtProcotolMonitorMgr();
	~mtProcotolMonitorMgr();

	int		init(DataInit* pkDataInit);
};