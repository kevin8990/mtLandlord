#include "mtServiceHallUpdateSocialInfo.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceHallUpdateSocialInfo::~mtServiceHallUpdateSocialInfo()
{

}

mtServiceHallUpdateSocialInfo::mtServiceHallUpdateSocialInfo()
{

}

int mtServiceHallUpdateSocialInfo::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;

	m_pkSQLEnv				= pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceHallUpdateSocialInfo::run( void* pData )
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

int mtServiceHallUpdateSocialInfo::exit()
{
	return 1;
}

int	mtServiceHallUpdateSocialInfo::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceHallUpdateSocialInfo::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceHallUpdateSocialInfo::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void  mtServiceHallUpdateSocialInfo::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::UserInfo    kUserInfo;

	memset(&kUserInfo,0,sizeof(kUserInfo));

	m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kUserInfo);

	kUserInfo.lUserIconId               = kDataRead.lUserIconId;
	kUserInfo.lUserSceneId              = kDataRead.lUserSceneId;    
	kUserInfo.lUserSex                  = kDataRead.lUserSex;
	kUserInfo.lUserAge                  = kDataRead.lUserAge;
	memcpy(kUserInfo.pcUserName, kDataRead.pcUserName, sizeof(kDataRead.pcUserName));
	memcpy(kUserInfo.pcUserNickName, kDataRead.pcUserNickName, sizeof(kDataRead.pcUserNickName));
	memcpy(kUserInfo.pcUserPwd, kDataRead.pcUserPwd, sizeof(kDataRead.pcUserPwd));
	memcpy(kUserInfo.pcUserLove, kDataRead.pcUserLove, sizeof(kDataRead.pcUserLove));
	memcpy(kUserInfo.pcUserRemark, kDataRead.pcUserRemark, sizeof(kDataRead.pcUserRemark));
	memcpy(kUserInfo.pcUserPhone, kDataRead.pcUserPhone, sizeof(kDataRead.pcUserPhone));
	memcpy(kUserInfo.pcUserQQ, kDataRead.pcUserQQ, sizeof(kDataRead.pcUserQQ));


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
	mtQueueHall::UserDataNode* pkHallUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kUserInfo.lUserId);
	if (pkHallUserDataNode)
	{
		pkHallUserDataNode->kUserInfo.lUserIconId        = kUserInfo.lUserIconId;
		pkHallUserDataNode->kUserInfo.lUserSex           = kUserInfo.lUserSex;
		pkHallUserDataNode->kUserInfo.lUserAge           = kUserInfo.lUserAge;
		memcpy(pkHallUserDataNode->kUserInfo.pcUserNickName, kUserInfo.pcUserNickName, sizeof(kUserInfo.pcUserNickName));
		memcpy(pkHallUserDataNode->kUserInfo.pcUserPwd, kUserInfo.pcUserPwd, sizeof(kUserInfo.pcUserPwd));

		memcpy(pkHallUserDataNode->kUserInfo.pcUserLove, kUserInfo.pcUserLove, sizeof(kUserInfo.pcUserLove));
		memcpy(pkHallUserDataNode->kUserInfo.pcUserRemark, kUserInfo.pcUserRemark, sizeof(kUserInfo.pcUserRemark));
		memcpy(pkHallUserDataNode->kUserInfo.pcUserPhone, kUserInfo.pcUserPhone, sizeof(kUserInfo.pcUserPhone));
		memcpy(pkHallUserDataNode->kUserInfo.pcUserQQ, kUserInfo.pcUserQQ, sizeof(kUserInfo.pcUserQQ));
	}
}

void mtServiceHallUpdateSocialInfo::UpdateUserInfo(mtQueuePacket::DataNode* pkQueuePacketDataNode)
{
	mtQueuePacket::DataNode*	    pkQueuePacketDataRead         = (mtQueuePacket::DataNode*)pkQueuePacketDataNode->pvData;
	mtServiceHallUpdateSocialInfo * pkServiceHallUpdateSocialInfo = (mtServiceHallUpdateSocialInfo*)(pkQueuePacketDataNode->pvService);
	if (NULL == pkServiceHallUpdateSocialInfo || NULL == pkQueuePacketDataRead)
	{
		return;
	}
   if (mtQueuePacket::E_DATA_TYPE_READ == pkQueuePacketDataNode->eDataType) 
	{
		/// 游戏代码 读完成
		mtQueueHall::UserInfo* pkUserInfo    = (mtQueueHall::UserInfo*)pkQueuePacketDataNode->pcData;
		DataRead*              pkDataRead    = (mtServiceHallUpdateSocialInfo::DataRead*)pkQueuePacketDataRead->pcData;
		if (pkUserInfo->lUserId == pkDataRead->lUserId)
		{
			/// 修改用户节点信息
			mtQueueHall::UserDataNode* pkHallUserDataNode = (mtQueueHall::UserDataNode*)pkServiceHallUpdateSocialInfo->getUserNodeAddr(pkUserInfo->lUserId);
			if (pkHallUserDataNode)
			{
				pkUserInfo->lUserGiveGoldFrequence  = pkHallUserDataNode->kUserInfo.lUserGiveGoldFrequence;
			}

			pkUserInfo->lUserIconId               = pkDataRead->lUserIconId;
			pkUserInfo->lUserSceneId              = pkDataRead->lUserSceneId;    
			pkUserInfo->lUserSex                  = pkDataRead->lUserSex;
			pkUserInfo->lUserAge                  = pkDataRead->lUserAge;
			memcpy(pkUserInfo->pcUserName, pkDataRead->pcUserName, sizeof(pkDataRead->pcUserName));
			memcpy(pkUserInfo->pcUserNickName, pkDataRead->pcUserNickName, sizeof(pkDataRead->pcUserNickName));
			memcpy(pkUserInfo->pcUserPwd, pkDataRead->pcUserPwd, sizeof(pkDataRead->pcUserPwd));
			memcpy(pkUserInfo->pcUserLove, pkDataRead->pcUserLove, sizeof(pkDataRead->pcUserLove));
			memcpy(pkUserInfo->pcUserRemark, pkDataRead->pcUserRemark, sizeof(pkDataRead->pcUserRemark));
			memcpy(pkUserInfo->pcUserPhone, pkDataRead->pcUserPhone, sizeof(pkDataRead->pcUserPhone));
			memcpy(pkUserInfo->pcUserQQ, pkDataRead->pcUserQQ, sizeof(pkDataRead->pcUserQQ));

			pkServiceHallUpdateSocialInfo->m_pkQueuePacket->push(pkQueuePacketDataRead);

			/// 写入文件
			pkQueuePacketDataNode->eDataType    = mtQueuePacket::E_DATA_TYPE_WRITE;
			m_pkSQLEnv->updateUserInfo(pkUserInfo);

			UpdateUserInfo(pkQueuePacketDataNode);


			return;
		}
	}
	else if (mtQueuePacket::E_DATA_TYPE_WRITE == pkQueuePacketDataNode->eDataType)	
	{
		/// 游戏代码 写完成
		mtQueueHall::UserInfo*  pkUserInfo  = (mtQueueHall::UserInfo*)pkQueuePacketDataNode->pcData;
		DataWrite  kDataWrite;
		kDataWrite.lStructBytes             = sizeof(kDataWrite);
		kDataWrite.lServiceType             = pkQueuePacketDataNode->lServiceType;
		kDataWrite.lResult                  = 1;

		/// 更新排行榜
		{
			mtQueueHall::DataRank kDataRank;
			kDataRank.lUserId         = pkUserInfo->lUserId;
			kDataRank.lUserLevel      = pkUserInfo->lUserLevel;
			kDataRank.lUserScore      = pkUserInfo->lUserScore;
			strcpy(kDataRank.pcUserNickName, pkUserInfo->pcUserNickName);
			pkServiceHallUpdateSocialInfo->updateDataRank(&kDataRank);
		}

		/// 修改用户节点信息
		mtQueueHall::UserDataNode* pkHallUserDataNode = (mtQueueHall::UserDataNode*)pkServiceHallUpdateSocialInfo->getUserNodeAddr(pkUserInfo->lUserId);
		if (pkHallUserDataNode)
		{
			pkHallUserDataNode->kUserInfo.lUserIconId        = pkUserInfo->lUserIconId;
			pkHallUserDataNode->kUserInfo.lUserSex           = pkUserInfo->lUserSex;
			pkHallUserDataNode->kUserInfo.lUserAge           = pkUserInfo->lUserAge;
			memcpy(pkHallUserDataNode->kUserInfo.pcUserNickName, pkUserInfo->pcUserNickName, sizeof(pkUserInfo->pcUserNickName));
			memcpy(pkHallUserDataNode->kUserInfo.pcUserPwd, pkUserInfo->pcUserPwd, sizeof(pkUserInfo->pcUserPwd));

			memcpy(pkHallUserDataNode->kUserInfo.pcUserLove, pkUserInfo->pcUserLove, sizeof(pkUserInfo->pcUserLove));
			memcpy(pkHallUserDataNode->kUserInfo.pcUserRemark, pkUserInfo->pcUserRemark, sizeof(pkUserInfo->pcUserRemark));
			memcpy(pkHallUserDataNode->kUserInfo.pcUserPhone, pkUserInfo->pcUserPhone, sizeof(pkUserInfo->pcUserPhone));
			memcpy(pkHallUserDataNode->kUserInfo.pcUserQQ, pkUserInfo->pcUserQQ, sizeof(pkUserInfo->pcUserQQ));
		}

		mtPrint(&kDataWrite);
		if (1 <= pkServiceHallUpdateSocialInfo->runWrite(pkQueuePacketDataNode->iSocket, (char*)&kDataWrite, kDataWrite.lStructBytes, 0, 3))
		{
			shutdown(pkQueuePacketDataNode->iSocket, 1);
			closesocket(pkQueuePacketDataNode->iSocket);
			pkQueuePacketDataNode->iSocket = INVALID_SOCKET;
		}
	}

	CloseHandle(pkQueuePacketDataNode->hFile);
	pkQueuePacketDataNode->hFile   = 0;
	pkServiceHallUpdateSocialInfo->m_pkQueuePacket->push(pkQueuePacketDataNode);
}

int mtServiceHallUpdateSocialInfo::updateDataRank(mtQueueHall::DataRank* pkDataRank)
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
