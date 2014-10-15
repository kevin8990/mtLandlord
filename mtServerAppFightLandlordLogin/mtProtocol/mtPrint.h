#pragma once
#include "mtProtocol.h"
#include "mtProtocolAlterPassword.h"
#include "mtProtocolConnect.h"
#include "mtProtocolLogin.h"
#include "mtProtocolRegister.h"
#include "mtThreadMgr.h"

//extern	int		mtPrint(mtProtocol::DataSend* pkDataSend,int iTab = 0);
//extern	int		mtPrint(mtProtocol::DataRecv* pkDataRecv,int iTab = 0);
extern	int		mtPrint(mtProtocolAlterPassword::DataSend* pkDataSend,int iTab = 0);
extern	int		mtPrint(mtProtocolAlterPassword::DataRecv* pkDataRecv,int iTab = 0);
extern	int		mtPrint(mtProtocolConnect::DataSendAddr* pkDataSendAddr,int iTab = 0,int index = 0);
extern	int		mtPrint(mtProtocolConnect::DataSend* pkDataSend,int iTab = 0);
extern	int		mtPrint(mtProtocolConnect::DataRecv* pkDataRecv,int iTab = 0);
extern	int		mtPrint(mtProtocolLogin::DataSend* pkDataSend,int iTab = 0);
extern	int		mtPrint(mtProtocolLogin::DataRecv* pkDataRecv,int iTab = 0);
extern	int		mtPrint(mtProtocolRegister::DataSend* pkDataSend,int iTab = 0);
extern	int		mtPrint(mtProtocolRegister::DataRecv* pkDataRecv,int iTab = 0);

//extern	int		mtPrint(mtProtocol::DataInit* pkDataInit,int iTab = 0);
//extern	int		mtPrint(mtThreadMgr::DataInit* pkDataInit,int iTab = 0);
//extern	int		mtPrint(mtThreadMonitor::DataInit* pkDataInit,int iTab = 0);
//extern	int		mtPrint(mtThreadWork::DataUser* pkDataUser,int iTab = 0);
//extern	int		mtPrint(mtThreadWork::DataInit* pkDataInit,int iTab = 0);
