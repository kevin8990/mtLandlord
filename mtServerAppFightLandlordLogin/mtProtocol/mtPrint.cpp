#include "mtProtocol.h"
#include "mtProtocolAlterPassword.h"
#include "mtProtocolConnect.h"
#include "mtProtocolLogin.h"
#include "mtProtocolRegister.h"
#include "mtThreadMgr.h"
#include "mtGlobal.h"


#define	MT_ARRAY_LENGTH(_array)	((sizeof(_array))/sizeof(_array[0]))
///#define		MT_DEBUG_PTCL(...)		printf(__VA_ARGS__)

///#define		MT_DEBUG(iLevel, ...)	MT_DEBUG_##iLevel(__VA_ARGS__); 

void  mtPrintTab(int iTab)
{
	MT_DEBUG_PTCL("\n");
	for (int iIdx=0;iIdx<iTab;++iIdx)
	{
		MT_DEBUG_PTCL("\t");
	}
}
void mtPrintString(char* pcFormat,char* pcSrc,int iByte,int iTab)
{
	int iLineCount=16;
	mtPrintTab(iTab);
	//MT_DEBUG(PTCL,pcFormat);
	MT_DEBUG(PTCL,"%s\t%s",pcFormat,pcSrc);
	MT_DEBUG(PTCL,"\n");
	int iIdx=0;
	for(;iIdx<iByte;++iIdx)
	{
		MT_DEBUG(PTCL,"%.2x ",pcSrc[iIdx]);
		if(((iIdx+1)%iLineCount)==0)
		{
			for(int j=0;j<iLineCount;++j)
			{
				if (isprint((unsigned char)(pcSrc[iIdx-iLineCount+j+1]))) 
					MT_DEBUG_PTCL("%c",pcSrc[iIdx-iLineCount+j]);
				else
					printf(".");

			}
			MT_DEBUG_PTCL("\n");
		}
	}
	int imod=iIdx&iLineCount;
	for(int j=0;j<imod;++j)
	{
		if (isprint((unsigned char)(pcSrc[j]))) 
			MT_DEBUG_PTCL("%c",pcSrc[j]);
		else
			printf(".");

	}
	//MT_DEBUG_PTCL("\n");
}
void mtLoopLongPrint(char* pcFormat,long* plSrc,int iByte,int iTab)
{
	for(int iIdx=0;iIdx<iByte;++iIdx)
	{
		mtPrintTab(iTab);
		MT_DEBUG(PTCL,pcFormat,iIdx,plSrc[iIdx]);
	}
}

/*int		mtPrint(mtProtocol::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",pkDataInit->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.ulIdBeg:			%d",pkDataInit->ulIdBeg			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkFileMgr*:		%d",pkDataInit->pkFileMgr			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkHallMaxIdCs*:	%d",pkDataInit->pkHallMaxIdCs			);
	return	0;																	      
}*/	

int		mtPrint(mtProtocol::DataSend* pkDataSend,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lStructBytes:		%d",pkDataSend->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lProtocolId:		%d",pkDataSend->lProtocolId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lKey:				%d",pkDataSend->lKey				);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lResultId:			%d",pkDataSend->lResultId				);
	mtLoopLongPrint("DataSend.plReservation[%d]:	%d",pkDataSend->plReservation,MT_ARRAY_LENGTH(pkDataSend->plReservation),iTab);
	return	0;																	      
}

int		mtPrint(mtProtocol::DataRecv* pkDataRecv,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRecv.lStructBytes:		%d",pkDataRecv->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRecv.lProtocolId:		%d",pkDataRecv->lProtocolId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRecv.lKey:				%d",pkDataRecv->lKey				);
	mtLoopLongPrint("DataRecv.plReservation[%d]:	%d",pkDataRecv->plReservation,MT_ARRAY_LENGTH(pkDataRecv->plReservation),iTab);
	return	0;																	      
}

int		mtPrint(mtProtocolAlterPassword::DataSend* pkDataSend,int iTab)
{
	mtPrint(&pkDataSend->kDataHead,iTab+1);
	return	0;																	      
}

int		mtPrint(mtProtocolAlterPassword::DataRecv* pkDataRecv,int iTab)
{
	mtPrint(&pkDataRecv->kDataHead,iTab+1);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRecv.lAccountType:		%d",pkDataRecv->lAccountType		);
	mtPrintString("DataRecv.pcAccount:",pkDataRecv->pcAccount,MT_ARRAY_LENGTH(pkDataRecv->pcAccount),iTab);
	mtPrintString("DataRecv.pcPwd:",pkDataRecv->pcPwd,MT_ARRAY_LENGTH(pkDataRecv->pcPwd),iTab);
	mtPrintString("DataRecv.pcNewPwd:",pkDataRecv->pcNewPwd,MT_ARRAY_LENGTH(pkDataRecv->pcNewPwd),iTab);
	return	0;																	      
}
std::string convertIPToString(unsigned long ip)
{
	char buffer[17];

	int res = sprintf(buffer, "%u.%u.%u.%u", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24));
	if (res < 0) {
		return std::string();
	}

	return buffer;
}


int		mtPrint(mtProtocolConnect::DataSendAddr* pkDataSendAddr,int iTab,int index)
{
	static std::string  ipType[] = {"Download","Charge","Hall"};
	mtPrintTab(iTab);MT_DEBUG(PTCL,"%s.ulIP:			%s",ipType[index].c_str(),convertIPToString(pkDataSendAddr->ulIP).c_str());
	mtPrintTab(iTab);MT_DEBUG(PTCL,"%s.lPort:			%d",ipType[index].c_str(),ntohs(pkDataSendAddr->lPort));
	return	0;																	      
}

int		mtPrint(mtProtocolConnect::DataSend* pkDataSend,int iTab)
{
	mtPrint(&pkDataSend->kDataHead,iTab+1);
	mtLoopLongPrint("DataSend.lVersion[%d]:			%d",pkDataSend->lVersion,MT_ARRAY_LENGTH(pkDataSend->lVersion),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lUpdateVersion:	%d",pkDataSend->lUpdateVersion		);
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataSend->pkAddr);iIdx++)
	{
		mtPrint(pkDataSend->pkAddr+iIdx,iTab+1,iIdx);
	}
	return	0;					   										      
}

int		mtPrint(mtProtocolConnect::DataRecv* pkDataRecv,int iTab)
{
	mtPrint(&pkDataRecv->kDataHead,iTab+1);
	mtLoopLongPrint("DataRecv.lVersion[%d]:			%d",pkDataRecv->lVersion,MT_ARRAY_LENGTH(pkDataRecv->lVersion),iTab);
	return	0;																	      
}

int		mtPrint(mtProtocolLogin::DataSend* pkDataSend,int iTab)
{
	mtPrint(&pkDataSend->kDataHead,iTab+1);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.ulId:				%d",pkDataSend->ulId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lRegister:			%d",pkDataSend->lRegister		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lUserRegisterDate:	%d",pkDataSend->lUserRegisterDate		);
	return	0;					   										      
}

long convertAccount(char *pcAccount)
{
	long * id = (long *)pcAccount;
	return *id + MT_SERVER_WORK_USER_ID_BASE;
}
int		mtPrint(mtProtocolLogin::DataRecv* pkDataRecv,int iTab)
{
	mtPrint(&pkDataRecv->kDataHead,iTab+1);
	mtPrintTab(iTab + 1);MT_DEBUG(PTCL,"DataRecv.lAccountType:    %d",pkDataRecv->lAccountType);
	mtPrintTab(iTab + 1);MT_DEBUG(PTCL,"DataRecv.pcAccount:       %d",convertAccount(pkDataRecv->pcAccount));
	mtPrintTab(iTab + 1);MT_DEBUG(PTCL,"DataRecv.pcPwd:           %s",pkDataRecv->pcPwd);
	
	//mtPrintString("DataRecv.pcAccount:",pkDataRecv->pcAccount,MT_ARRAY_LENGTH(pkDataRecv->pcAccount),iTab);
	//mtPrintString("DataRecv.pcPwd:",pkDataRecv->pcPwd,MT_ARRAY_LENGTH(pkDataRecv->pcAccount),iTab);
	return	0;																	      
}

int		mtPrint(mtProtocolRegister::DataSend* pkDataSend,int iTab)
{
	mtPrint(&pkDataSend->kDataHead,iTab+1);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.uUserId:			%d",pkDataSend->uUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.pcAccount:         %s",pkDataSend->pcAccount);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.pcPwd:             %s",pkDataSend->pcPwd);
	//mtPrintString("DataSend.pcAccount:",pkDataSend->pcAccount,MT_ARRAY_LENGTH(pkDataSend->pcAccount),iTab);
	//mtPrintString("DataSend.pcPwd:",pkDataSend->pcPwd,MT_ARRAY_LENGTH(pkDataSend->pcAccount),iTab);
	//mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lRegister:			%d",pkDataSend->lRegister		);
	//mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSend.lUserRegisterDate:	%d",pkDataSend->lUserRegisterDate		);

	return	0;					   										      
}

int		mtPrint(mtProtocolRegister::DataRecv* pkDataRecv,int iTab)
{
	mtPrint(&pkDataRecv->kDataHead,iTab+1);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRecv.uUserId:			%d",pkDataRecv->uUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRecv.lAccountType:		%d",pkDataRecv->lAccountType		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"pkDataRecv.pcAccount:       %s",pkDataRecv->pcAccount);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"pkDataRecv.pcPwd:           %s",pkDataRecv->pcPwd);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"pkDataRecv.pcPhone:         %s",pkDataRecv->pcPhone);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"pkDataRecv.pcQQ:            %s",pkDataRecv->pcQQ);

	//mtPrintString("DataRecv.pcAccount:",pkDataRecv->pcAccount,MT_ARRAY_LENGTH(pkDataRecv->pcAccount),iTab);
	//mtPrintString("DataRecv.pcQQ:",pkDataRecv->pcQQ,MT_ARRAY_LENGTH(pkDataRecv->pcQQ),iTab);
	//mtPrintString("DataRecv.pcPhone:",pkDataRecv->pcPhone,MT_ARRAY_LENGTH(pkDataRecv->pcPhone),iTab);
	//mtPrintString("DataRecv.pcPwd:",pkDataRecv->pcPwd,MT_ARRAY_LENGTH(pkDataRecv->pcPwd),iTab);
	
	return	0;																	      
}