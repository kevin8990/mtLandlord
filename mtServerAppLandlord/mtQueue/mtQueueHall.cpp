#include "mtQueueHall.h"

#include "mtSQLEnv.h"

mtQueueHall::mtQueueHall()
{
	m_iSpinCount	= 0;
}

mtQueueHall::~mtQueueHall()
{

}
 
int mtQueueHall::init(void* pData)
{
	DataInit*	pkDataInit    = (DataInit*)pData;
	m_iSpinCount		      = pkDataInit->iSpinCount;
	m_pcServiceExeDir         = pkDataInit->pcServiceExeDir;
	m_pkSQLEnv                = pkDataInit->pkSQLEnv;
	///
	initSpaceRoom();

	return	1;
}

int mtQueueHall::exit( void )
{
	delete [] m_kDataHall.pkUserDataNodeList;
	m_kDataHall.pkUserDataNodeList = NULL;

	delete [] m_kDataHall.pkDataRank;
	m_kDataHall.pkDataRank         = NULL;
	DeleteCriticalSection(&m_kDataHall.kCriticalSectionDataRank);

	delete [] m_kDataHall.pkSpace;
	m_kDataHall.pkSpace            = NULL;

	m_iSpinCount	= 0;
	return 1;
}

int mtQueueHall::initSpaceRoom()
{
	m_kDataHall.pkUserDataNodeList = new mtQueueHall::UserDataNode[MT_NODE_QUEUE_USER_MAX];
	memset(m_kDataHall.pkUserDataNodeList, 0, sizeof(mtQueueHall::UserDataNode) * MT_NODE_QUEUE_USER_MAX);

	/// datarank
	if (0 != InitializeCriticalSectionAndSpinCount(&m_kDataHall.kCriticalSectionDataRank, m_iSpinCount))
	{
		m_kDataHall.pkDataRank         = new DataRank[MT_NODE_QUEUE_USER_RANK_MAX];
		memset(m_kDataHall.pkDataRank, 0, sizeof(DataRank) * MT_NODE_QUEUE_USER_RANK_MAX);
		int i = 0;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 110006;
		m_kDataHall.pkDataRank[i].lUserLevel      = 14;
		m_kDataHall.pkDataRank[i].lUserScore      = 100100100;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 1;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 100008;
		m_kDataHall.pkDataRank[i].lUserLevel      = 14;
		m_kDataHall.pkDataRank[i].lUserScore      = 100001000;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 2;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 100101;
		m_kDataHall.pkDataRank[i].lUserLevel      = 13;
		m_kDataHall.pkDataRank[i].lUserScore      = 50089000;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 3;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 906036;
		m_kDataHall.pkDataRank[i].lUserLevel      = 13;
		m_kDataHall.pkDataRank[i].lUserScore      = 50003080;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 4;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 310005;
		m_kDataHall.pkDataRank[i].lUserLevel      = 13;
		m_kDataHall.pkDataRank[i].lUserScore      = 50000740;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 5;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 170009;
		m_kDataHall.pkDataRank[i].lUserLevel      = 10;
		m_kDataHall.pkDataRank[i].lUserScore      = 10250030;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 6;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 120603;
		m_kDataHall.pkDataRank[i].lUserLevel      = 10;
		m_kDataHall.pkDataRank[i].lUserScore      = 10201000;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 7;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 160401;
		m_kDataHall.pkDataRank[i].lUserLevel      = 9;
		m_kDataHall.pkDataRank[i].lUserScore      = 6007100;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 8;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 190708;
		m_kDataHall.pkDataRank[i].lUserLevel      = 9;
		m_kDataHall.pkDataRank[i].lUserScore      = 5000100;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);

		i = 9;
		/// m_kDataHall.pkDataRank[i].lStructBytes    = sizeof(DataRank);
		m_kDataHall.pkDataRank[i].lUserId         = 140329;
		m_kDataHall.pkDataRank[i].lUserLevel      = 3;
		m_kDataHall.pkDataRank[i].lUserScore      = 50010;
		/// m_kDataHall.pkDataRank[i].lUserRank       = i + 1;
		sprintf_s(m_kDataHall.pkDataRank[i].pcUserNickName, "%d", m_kDataHall.pkDataRank[i].lUserId);
	}
	mtQueueHall::ProjectInfo  m_kProjectInfo;
	m_pkSQLEnv->getProject(1000,1,&m_kProjectInfo);
	unsigned long        ulRoomServerIP = inet_addr(m_kProjectInfo.pcRoomServer);
	long                 lRoomServerPort = htons(m_kProjectInfo.lRoomPort);

	/// 第一个场 
	m_kDataHall.pkSpace                                   =  new mtQueueHall::DataSpace[4];
	memset(m_kDataHall.pkSpace, 0, sizeof(mtQueueHall::DataSpace) * 4);

	m_kDataHall.pkSpace[0].lStructBytes		              = sizeof(mtQueueHall::DataSpace);
	m_kDataHall.pkSpace[0].lSpaceId	                      = 0;

	m_kDataHall.pkSpace[0].pkRoom[0].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[0].pkRoom[1].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[0].pkRoom[2].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[0].pkRoom[3].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[0].pkRoom[4].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[0].pkRoom[5].lStructBytes	      = sizeof(mtQueueHall::DataRoom);

	m_kDataHall.pkSpace[0].pkRoom[0].lRoomId	          = 0;
	m_kDataHall.pkSpace[0].pkRoom[0].lGoldBase            = 30;
	m_kDataHall.pkSpace[0].pkRoom[0].lGoldAdmittance      = 90;
	m_kDataHall.pkSpace[0].pkRoom[0].lGoldDeduct          = 10;
	strcpy(m_kDataHall.pkSpace[0].pkRoom[0].pcRoomName, "");

	m_kDataHall.pkSpace[0].pkRoom[1].lRoomId	          = 1;
	m_kDataHall.pkSpace[0].pkRoom[1].lGoldBase            = 100;
	m_kDataHall.pkSpace[0].pkRoom[1].lGoldAdmittance      = 500;
	m_kDataHall.pkSpace[0].pkRoom[1].lGoldDeduct          = 20;
	strcpy(m_kDataHall.pkSpace[0].pkRoom[1].pcRoomName, "");

	m_kDataHall.pkSpace[0].pkRoom[2].lRoomId	          = 2;
	m_kDataHall.pkSpace[0].pkRoom[2].lGoldBase            = 200;
	m_kDataHall.pkSpace[0].pkRoom[2].lGoldAdmittance      = 1000;
	m_kDataHall.pkSpace[0].pkRoom[2].lGoldDeduct          = 30;
	strcpy(m_kDataHall.pkSpace[0].pkRoom[2].pcRoomName, "");

	m_kDataHall.pkSpace[0].pkRoom[3].lRoomId	          = 3;
	m_kDataHall.pkSpace[0].pkRoom[3].lGoldBase            = 500;
	m_kDataHall.pkSpace[0].pkRoom[3].lGoldAdmittance      = 2000;
	m_kDataHall.pkSpace[0].pkRoom[3].lGoldDeduct          = 50;
	strcpy(m_kDataHall.pkSpace[0].pkRoom[3].pcRoomName, "");

	m_kDataHall.pkSpace[0].pkRoom[4].lRoomId	          = 4;
	m_kDataHall.pkSpace[0].pkRoom[4].lGoldBase            = 800;
	m_kDataHall.pkSpace[0].pkRoom[4].lGoldAdmittance      = 3000;
	m_kDataHall.pkSpace[0].pkRoom[4].lGoldDeduct          = 60;
	strcpy(m_kDataHall.pkSpace[0].pkRoom[4].pcRoomName, "");

	m_kDataHall.pkSpace[0].pkRoom[5].lRoomId	          = 5;
	m_kDataHall.pkSpace[0].pkRoom[5].lGoldBase            = 1500;
	m_kDataHall.pkSpace[0].pkRoom[5].lGoldAdmittance      = 5000;
	m_kDataHall.pkSpace[0].pkRoom[5].lGoldDeduct          = 100;
	strcpy(m_kDataHall.pkSpace[0].pkRoom[5].pcRoomName, "");

	m_kDataHall.pkSpace[0].pkRoom[0].ulRoomlP	          = ulRoomServerIP;/// 0xDA8AA8B4 - 180.168.138.218, 0x1c01a8c0 - 192.168.1.28;
	m_kDataHall.pkSpace[0].pkRoom[0].lRoomPort	          = lRoomServerPort; /// 9050 --23075, 9066 --27171
	m_kDataHall.pkSpace[0].pkRoom[1].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[0].pkRoom[1].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[0].pkRoom[2].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[0].pkRoom[2].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[0].pkRoom[3].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[0].pkRoom[3].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[0].pkRoom[4].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[0].pkRoom[4].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[0].pkRoom[5].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[0].pkRoom[5].lRoomPort	          = lRoomServerPort;

	/// 第二个场
	m_kDataHall.pkSpace[1].lStructBytes		              = sizeof(mtQueueHall::DataSpace);
	m_kDataHall.pkSpace[1].lSpaceId	                      = 1;

	m_kDataHall.pkSpace[1].pkRoom[0].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[1].pkRoom[1].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[1].pkRoom[2].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[1].pkRoom[3].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[1].pkRoom[4].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[1].pkRoom[5].lStructBytes	      = sizeof(mtQueueHall::DataRoom);

	m_kDataHall.pkSpace[1].pkRoom[0].lRoomId	          = 0;
	m_kDataHall.pkSpace[1].pkRoom[0].lSpaceId	          = 1;
	m_kDataHall.pkSpace[1].pkRoom[0].lGoldBase            = 2000;
	m_kDataHall.pkSpace[1].pkRoom[0].lGoldAdmittance      = 10000;
	m_kDataHall.pkSpace[1].pkRoom[0].lGoldDeduct          = 200;
	strcpy(m_kDataHall.pkSpace[1].pkRoom[0].pcRoomName, "");

	m_kDataHall.pkSpace[1].pkRoom[1].lRoomId	          = 1;
	m_kDataHall.pkSpace[1].pkRoom[1].lSpaceId	          = 1;
	m_kDataHall.pkSpace[1].pkRoom[1].lGoldBase            = 3000;
	m_kDataHall.pkSpace[1].pkRoom[1].lGoldAdmittance      = 10000;
	m_kDataHall.pkSpace[1].pkRoom[1].lGoldDeduct          = 300;
	strcpy(m_kDataHall.pkSpace[1].pkRoom[1].pcRoomName, "");

	m_kDataHall.pkSpace[1].pkRoom[2].lRoomId	          = 2;
	m_kDataHall.pkSpace[1].pkRoom[2].lSpaceId	          = 1;
	m_kDataHall.pkSpace[1].pkRoom[2].lGoldBase            = 5000;
	m_kDataHall.pkSpace[1].pkRoom[2].lGoldAdmittance      = 15000;
	m_kDataHall.pkSpace[1].pkRoom[2].lGoldDeduct          = 500;
	strcpy(m_kDataHall.pkSpace[1].pkRoom[2].pcRoomName, "");

	m_kDataHall.pkSpace[1].pkRoom[3].lRoomId	          = 3;
	m_kDataHall.pkSpace[1].pkRoom[3].lSpaceId	          = 1;
	m_kDataHall.pkSpace[1].pkRoom[3].lGoldBase            = 10000;
	m_kDataHall.pkSpace[1].pkRoom[3].lGoldAdmittance      = 20000;
	m_kDataHall.pkSpace[1].pkRoom[3].lGoldDeduct          = 1000;
	strcpy(m_kDataHall.pkSpace[1].pkRoom[3].pcRoomName, "");

	m_kDataHall.pkSpace[1].pkRoom[4].lRoomId	          = 4;
	m_kDataHall.pkSpace[1].pkRoom[4].lSpaceId	          = 1;
	m_kDataHall.pkSpace[1].pkRoom[4].lGoldBase            = 20000;
	m_kDataHall.pkSpace[1].pkRoom[4].lGoldAdmittance      = 60000;
	m_kDataHall.pkSpace[1].pkRoom[4].lGoldDeduct          = 3000;
	strcpy(m_kDataHall.pkSpace[1].pkRoom[4].pcRoomName, "");

	m_kDataHall.pkSpace[1].pkRoom[5].lRoomId	          = 5;
	m_kDataHall.pkSpace[1].pkRoom[5].lSpaceId	          = 1;
	m_kDataHall.pkSpace[1].pkRoom[5].lGoldBase            = 50000;
	m_kDataHall.pkSpace[1].pkRoom[5].lGoldAdmittance      = 100000;
	m_kDataHall.pkSpace[1].pkRoom[5].lGoldDeduct          = 10000;
	strcpy(m_kDataHall.pkSpace[1].pkRoom[5].pcRoomName, "");

	m_kDataHall.pkSpace[1].pkRoom[0].ulRoomlP	          = ulRoomServerIP;/// 0xDA8AA8B4 - 180.168.138.218, 0x1c01a8c0 - 192.168.1.28;
	m_kDataHall.pkSpace[1].pkRoom[0].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[1].pkRoom[1].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[1].pkRoom[1].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[1].pkRoom[2].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[1].pkRoom[2].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[1].pkRoom[3].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[1].pkRoom[3].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[1].pkRoom[4].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[1].pkRoom[4].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[1].pkRoom[5].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[1].pkRoom[5].lRoomPort	          = lRoomServerPort;

	//第二个房间服务器  第一个场

	m_pkSQLEnv->getProject(1001,1,&m_kProjectInfo);
	ulRoomServerIP = inet_addr(m_kProjectInfo.pcRoomServer);
    lRoomServerPort =  htons(m_kProjectInfo.lRoomPort);

	m_kDataHall.pkSpace[2].lStructBytes		              = sizeof(mtQueueHall::DataSpace);
	m_kDataHall.pkSpace[2].lSpaceId	                      = 0;

	m_kDataHall.pkSpace[2].pkRoom[0].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[2].pkRoom[1].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[2].pkRoom[2].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[2].pkRoom[3].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[2].pkRoom[4].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[2].pkRoom[5].lStructBytes	      = sizeof(mtQueueHall::DataRoom);

	m_kDataHall.pkSpace[2].pkRoom[0].lRoomId	          = 0;
	m_kDataHall.pkSpace[2].pkRoom[0].lSpaceId	          = 1;
	m_kDataHall.pkSpace[2].pkRoom[0].lGoldBase            = 2000;
	m_kDataHall.pkSpace[2].pkRoom[0].lGoldAdmittance      = 10000;
	m_kDataHall.pkSpace[2].pkRoom[0].lGoldDeduct          = 200;
	strcpy(m_kDataHall.pkSpace[2].pkRoom[0].pcRoomName, "");

	m_kDataHall.pkSpace[2].pkRoom[1].lRoomId	          = 1;
	m_kDataHall.pkSpace[2].pkRoom[1].lSpaceId	          = 1;
	m_kDataHall.pkSpace[2].pkRoom[1].lGoldBase            = 3000;
	m_kDataHall.pkSpace[2].pkRoom[1].lGoldAdmittance      = 10000;
	m_kDataHall.pkSpace[2].pkRoom[1].lGoldDeduct          = 300;
	strcpy(m_kDataHall.pkSpace[2].pkRoom[1].pcRoomName, "");

	m_kDataHall.pkSpace[2].pkRoom[2].lRoomId	          = 2;
	m_kDataHall.pkSpace[2].pkRoom[2].lSpaceId	          = 1;
	m_kDataHall.pkSpace[2].pkRoom[2].lGoldBase            = 5000;
	m_kDataHall.pkSpace[2].pkRoom[2].lGoldAdmittance      = 15000;
	m_kDataHall.pkSpace[2].pkRoom[2].lGoldDeduct          = 500;
	strcpy(m_kDataHall.pkSpace[2].pkRoom[2].pcRoomName, "");

	m_kDataHall.pkSpace[2].pkRoom[3].lRoomId	          = 3;
	m_kDataHall.pkSpace[2].pkRoom[3].lSpaceId	          = 1;
	m_kDataHall.pkSpace[2].pkRoom[3].lGoldBase            = 10000;
	m_kDataHall.pkSpace[2].pkRoom[3].lGoldAdmittance      = 20000;
	m_kDataHall.pkSpace[2].pkRoom[3].lGoldDeduct          = 1000;
	strcpy(m_kDataHall.pkSpace[2].pkRoom[3].pcRoomName, "");

	m_kDataHall.pkSpace[2].pkRoom[4].lRoomId	          = 4;
	m_kDataHall.pkSpace[2].pkRoom[4].lSpaceId	          = 1;
	m_kDataHall.pkSpace[2].pkRoom[4].lGoldBase            = 20000;
	m_kDataHall.pkSpace[2].pkRoom[4].lGoldAdmittance      = 60000;
	m_kDataHall.pkSpace[2].pkRoom[4].lGoldDeduct          = 3000;
	strcpy(m_kDataHall.pkSpace[1].pkRoom[4].pcRoomName, "");

	m_kDataHall.pkSpace[2].pkRoom[5].lRoomId	          = 5;
	m_kDataHall.pkSpace[2].pkRoom[5].lSpaceId	          = 1;
	m_kDataHall.pkSpace[2].pkRoom[5].lGoldBase            = 50000;
	m_kDataHall.pkSpace[2].pkRoom[5].lGoldAdmittance      = 100000;
	m_kDataHall.pkSpace[2].pkRoom[5].lGoldDeduct          = 10000;
	strcpy(m_kDataHall.pkSpace[2].pkRoom[5].pcRoomName, "");

	m_kDataHall.pkSpace[2].pkRoom[0].ulRoomlP	          = ulRoomServerIP;/// 0xDA8AA8B4 - 180.168.138.218, 0x1c01a8c0 - 192.168.1.28;
	m_kDataHall.pkSpace[2].pkRoom[0].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[2].pkRoom[1].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[2].pkRoom[1].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[2].pkRoom[2].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[2].pkRoom[2].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[2].pkRoom[3].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[2].pkRoom[3].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[2].pkRoom[4].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[2].pkRoom[4].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[2].pkRoom[5].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[2].pkRoom[5].lRoomPort	          = lRoomServerPort;

	//第二个房间服务器 第二个场
	m_kDataHall.pkSpace[3].lStructBytes		              = sizeof(mtQueueHall::DataSpace);
	m_kDataHall.pkSpace[3].lSpaceId	                      = 1;

	m_kDataHall.pkSpace[3].pkRoom[0].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[3].pkRoom[1].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[3].pkRoom[2].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[3].pkRoom[3].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[3].pkRoom[4].lStructBytes	      = sizeof(mtQueueHall::DataRoom);
	m_kDataHall.pkSpace[3].pkRoom[5].lStructBytes	      = sizeof(mtQueueHall::DataRoom);

	m_kDataHall.pkSpace[3].pkRoom[0].lRoomId	          = 0;
	m_kDataHall.pkSpace[3].pkRoom[0].lSpaceId	          = 1;
	m_kDataHall.pkSpace[3].pkRoom[0].lGoldBase            = 2000;
	m_kDataHall.pkSpace[3].pkRoom[0].lGoldAdmittance      = 10000;
	m_kDataHall.pkSpace[3].pkRoom[0].lGoldDeduct          = 200;
	strcpy(m_kDataHall.pkSpace[3].pkRoom[0].pcRoomName, "");

	m_kDataHall.pkSpace[3].pkRoom[1].lRoomId	          = 1;
	m_kDataHall.pkSpace[3].pkRoom[1].lSpaceId	          = 1;
	m_kDataHall.pkSpace[3].pkRoom[1].lGoldBase            = 3000;
	m_kDataHall.pkSpace[3].pkRoom[1].lGoldAdmittance      = 10000;
	m_kDataHall.pkSpace[3].pkRoom[1].lGoldDeduct          = 300;
	strcpy(m_kDataHall.pkSpace[3].pkRoom[1].pcRoomName, "");

	m_kDataHall.pkSpace[3].pkRoom[2].lRoomId	          = 2;
	m_kDataHall.pkSpace[3].pkRoom[2].lSpaceId	          = 1;
	m_kDataHall.pkSpace[3].pkRoom[2].lGoldBase            = 5000;
	m_kDataHall.pkSpace[3].pkRoom[2].lGoldAdmittance      = 15000;
	m_kDataHall.pkSpace[3].pkRoom[2].lGoldDeduct          = 500;
	strcpy(m_kDataHall.pkSpace[3].pkRoom[2].pcRoomName, "");

	m_kDataHall.pkSpace[3].pkRoom[3].lRoomId	          = 3;
	m_kDataHall.pkSpace[3].pkRoom[3].lSpaceId	          = 1;
	m_kDataHall.pkSpace[3].pkRoom[3].lGoldBase            = 10000;
	m_kDataHall.pkSpace[3].pkRoom[3].lGoldAdmittance      = 20000;
	m_kDataHall.pkSpace[3].pkRoom[3].lGoldDeduct          = 1000;
	strcpy(m_kDataHall.pkSpace[3].pkRoom[3].pcRoomName, "");

	m_kDataHall.pkSpace[3].pkRoom[4].lRoomId	          = 4;
	m_kDataHall.pkSpace[3].pkRoom[4].lSpaceId	          = 1;
	m_kDataHall.pkSpace[3].pkRoom[4].lGoldBase            = 20000;
	m_kDataHall.pkSpace[3].pkRoom[4].lGoldAdmittance      = 60000;
	m_kDataHall.pkSpace[3].pkRoom[4].lGoldDeduct          = 3000;
	strcpy(m_kDataHall.pkSpace[3].pkRoom[4].pcRoomName, "");

	m_kDataHall.pkSpace[3].pkRoom[5].lRoomId	          = 5;
	m_kDataHall.pkSpace[3].pkRoom[5].lSpaceId	          = 1;
	m_kDataHall.pkSpace[3].pkRoom[5].lGoldBase            = 50000;
	m_kDataHall.pkSpace[3].pkRoom[5].lGoldAdmittance      = 100000;
	m_kDataHall.pkSpace[3].pkRoom[5].lGoldDeduct          = 10000;
	strcpy(m_kDataHall.pkSpace[3].pkRoom[5].pcRoomName, "");

	m_kDataHall.pkSpace[3].pkRoom[0].ulRoomlP	          = ulRoomServerIP;/// 0xDA8AA8B4 - 180.168.138.218, 0x1c01a8c0 - 192.168.1.28;
	m_kDataHall.pkSpace[3].pkRoom[0].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[3].pkRoom[1].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[3].pkRoom[1].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[3].pkRoom[2].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[3].pkRoom[2].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[3].pkRoom[3].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[3].pkRoom[3].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[3].pkRoom[4].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[3].pkRoom[4].lRoomPort	          = lRoomServerPort;
	m_kDataHall.pkSpace[3].pkRoom[5].ulRoomlP	          = ulRoomServerIP;
	m_kDataHall.pkSpace[3].pkRoom[5].lRoomPort	          = lRoomServerPort;

	return 1;
}
