#include "mtServiceHallUpdatePlayInfo.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceHallUpdatePlayInfo::~mtServiceHallUpdatePlayInfo()
{
}

mtServiceHallUpdatePlayInfo::mtServiceHallUpdatePlayInfo()
{

}

int mtServiceHallUpdatePlayInfo::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		= pkDataInit->pkSQLEnv;

	return	1;
}


int mtServiceHallUpdatePlayInfo::run( void* pData )
{
	SOCKET	   iSocket	= *(SOCKET*)pData;
	DataRead   kDataRead;
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0 , sizeof(kDataWrite));
	memset(&kDataRead, 0 , sizeof(kDataRead));

	int		   iRet	= 0;
	if (runRead(iSocket, &kDataRead))
	{
		//截断超过12位的六个汉字
		if(strlen(kDataRead.pcUserNickName) >= sizeof(kDataRead.pcUserNickName)){
			(kDataRead.pcUserNickName)[sizeof(kDataRead.pcUserNickName)-1] = '\0';
		}
		mtPrint(&kDataRead);

		kDataWrite.lStructBytes            = sizeof(kDataWrite);
		kDataWrite.lServiceType            = kDataRead.lServiceType;

		if (MT_SERVER_WORK_USER_ID_MIN <= kDataRead.lUserId && MT_SERVER_WORK_USER_ID_MAX > kDataRead.lUserId)
		{
			// 根据用户id，获得用户节点在内存的地址
			mtQueueHall::UserDataNode* pkUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.lUserId);
			if (!pkUserDataNode)
			{
				kDataWrite.lResult = -1;
			}
			else
			{
				if (mtQueueHall::E_HALL_USER_STATUS_OFFLINE == pkUserDataNode->lIsOnLine)
				{
					kDataWrite.lResult = -1;
					MT_DEBUG_ERROR(PTCL,"\nERROR:user(%d)is offline!Please login!",kDataRead.lUserId);
				}
				else
				{
					initDataWrite(kDataRead,kDataWrite);
				}
			}		
		}

		mtPrint(&kDataWrite);
		iRet = runWrite(iSocket, (char*)&kDataWrite, kDataWrite.lStructBytes, 0, 3);
		if (iRet >= 1)
		{
			shutdown(iSocket, 1);
			closesocket(iSocket);
			*(SOCKET*)pData = INVALID_SOCKET;
		}
	}

	return 1;
}

int mtServiceHallUpdatePlayInfo::exit()
{
	return 1;
}

int	mtServiceHallUpdatePlayInfo::runRead(SOCKET socket, DataRead* pkDataRead)
{	
	int	iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, sizeof(DataRead), MSG_PEEK);
	if (iReadBytesTotalHas < sizeof(pkDataRead->lStructBytes))
	{		
		return	0;
	}

	if (iReadBytesTotalHas >= pkDataRead->lStructBytes) 
	{
		iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, pkDataRead->lStructBytes, 0);
	}

	return (iReadBytesTotalHas < pkDataRead->lStructBytes) ? 0 : 1;
}

int	mtServiceHallUpdatePlayInfo::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
{
	int		iRet		= 0;
	int		iWriteTimes;

	for (iWriteTimes = 0;iWriteTimes < iTimes;iWriteTimes ++)
	{
		iRet	+= send(socket, pcBuffer + iRet, iBytes - iRet, flags);
		if (iRet >= iBytes)
		{
			return 1;
		}
	}

	return 0;
}

void* mtServiceHallUpdatePlayInfo::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void  mtServiceHallUpdatePlayInfo::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::UserInfo    kUserInfo;

	memset(&kUserInfo,0,sizeof(kUserInfo));

	m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kUserInfo);

	kUserInfo.lUserGold                   = kDataRead.lUserGold;
	kUserInfo.lUserScore                  = kDataRead.lUserScore;
	kUserInfo.lUserLevel                  = kDataRead.lUserLevel;
	kUserInfo.lUserAllChess               = kDataRead.lUserAllChess;
	kUserInfo.lUserWinChess               = kDataRead.lUserWinChess;
	kUserInfo.lUserWinRate                = kDataRead.lUserWinRate;
	kUserInfo.lUserOffLineCount           = kDataRead.lUserOffLineCount;
	memcpy(kUserInfo.plPropsCount, kDataRead.plPropsCount, sizeof(kDataRead.plPropsCount));
	memcpy(kUserInfo.plUserChessPalyed, kDataRead.plUserChessPlayedToday, sizeof(kDataRead.plUserChessPlayedToday));
	memcpy(kUserInfo.pcUserNickName, kDataRead.pcUserNickName, sizeof(kDataRead.pcUserNickName));

	m_pkSQLEnv->updateUserInfo(&kUserInfo);

	kDataWrite.lResult                  = 1;

	/// 更新排行榜
	if (kUserInfo.lUserScore > m_pkQueueHall->m_kDataHall.pkDataRank[9].lUserScore)
	{
		mtQueueHall::DataRank kDataRank;
		kDataRank.lUserId         = kUserInfo.lUserId;
		kDataRank.lUserLevel      = kUserInfo.lUserLevel;
		kDataRank.lUserScore      = kUserInfo.lUserScore;
		strcpy(kDataRank.pcUserNickName, kUserInfo.pcUserNickName);
		updateDataRank(&kDataRank);
	}

	/// 修改用户节点信息
	mtQueueHall::UserDataNode* pkHallUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.lUserId);
	if (pkHallUserDataNode)
	{
		pkHallUserDataNode->kUserInfo.lUserGold            = kUserInfo.lUserGold;
		pkHallUserDataNode->kUserInfo.lUserScore           = kUserInfo.lUserScore;
		pkHallUserDataNode->kUserInfo.lUserLevel           = kUserInfo.lUserLevel;
		pkHallUserDataNode->kUserInfo.lUserAllChess        = kUserInfo.lUserAllChess;
		pkHallUserDataNode->kUserInfo.lUserWinChess        = kUserInfo.lUserWinChess;
		pkHallUserDataNode->kUserInfo.lUserWinRate         = kUserInfo.lUserWinRate;
		pkHallUserDataNode->kUserInfo.lUserOffLineCount    = kUserInfo.lUserOffLineCount;
		memcpy(pkHallUserDataNode->kUserInfo.plPropsCount, kUserInfo.plPropsCount, sizeof(kUserInfo.plPropsCount));
		memcpy(pkHallUserDataNode->kUserInfo.plUserChessPalyed, kUserInfo.plUserChessPalyed, sizeof(kUserInfo.plUserChessPalyed));
		memcpy(pkHallUserDataNode->kUserInfo.pcUserNickName, kUserInfo.pcUserNickName, sizeof(kUserInfo.pcUserNickName));
	}
}

int mtServiceHallUpdatePlayInfo::updateDataRank(mtQueueHall::DataRank* pkDataRank)
{
	int i = 0;
	int iUserInRank = 0;
	EnterCriticalSection(&m_pkQueueHall->m_kDataHall.kCriticalSectionDataRank);
	for (i = 0; i < MT_NODE_QUEUE_USER_RANK_MAX; i++)
	{
		if (pkDataRank->lUserId == m_pkQueueHall->m_kDataHall.pkDataRank[i].lUserId)
		{
			iUserInRank = 1;
			if (9 > i)
			{
				m_pkQueueHall->m_kDataHall.pkDataRank[i] = m_pkQueueHall->m_kDataHall.pkDataRank[i + 1];
			}
		}
		else
		{
			if (1 == iUserInRank && 9 > i)
			{
				m_pkQueueHall->m_kDataHall.pkDataRank[i] = m_pkQueueHall->m_kDataHall.pkDataRank[i + 1];
			}
		}
	}

	if (1 == iUserInRank)
	{
		m_pkQueueHall->m_kDataHall.pkDataRank[9].lUserScore = 0;
	}

	for (i = (MT_NODE_QUEUE_USER_RANK_MAX - 2); i >= 0; i--)
	{
		if (pkDataRank->lUserScore > m_pkQueueHall->m_kDataHall.pkDataRank[i].lUserScore)
		{
			m_pkQueueHall->m_kDataHall.pkDataRank[i + 1] = m_pkQueueHall->m_kDataHall.pkDataRank[i];
		}
		else
		{
			m_pkQueueHall->m_kDataHall.pkDataRank[i + 1] = *pkDataRank;
			break;
		}
	}

	if (0 > i)
	{
		m_pkQueueHall->m_kDataHall.pkDataRank[0] = *pkDataRank;
	}
	LeaveCriticalSection(&m_pkQueueHall->m_kDataHall.kCriticalSectionDataRank);

	return m_pkQueueHall->m_kDataHall.pkDataRank[9].lUserScore;
}


