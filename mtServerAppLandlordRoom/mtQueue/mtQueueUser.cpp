#include "mtQueueUser.h"


mtQueueUser::mtQueueUser()
{
	m_iNodeTotal	= MT_NODE_QUEUE_USER;

}

mtQueueUser::~mtQueueUser()
{

}

int mtQueueUser::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_iSpinCount			= pkDataInit->iSpinCount;
	m_hIOCP					= pkDataInit->hIOCP;
	
	if (!InitializeCriticalSectionAndSpinCount(&m_kCirticalSection, m_iSpinCount))
	{
		return	0;
	}
	
	memset (m_pkNodeList, 0, sizeof(m_pkNodeList));

	int		iRet			= m_pkIdQueue.init(m_iSpinCount);

	int			iNodeIdx;
	DataNode*	pkDataNode	= m_pkNodeList;
	for (iNodeIdx = 0; iNodeIdx < m_iNodeTotal; iNodeIdx++)
	{
		if (!m_pkIdQueue.push(&pkDataNode))
		{
			return	0;
		}
		pkDataNode++;
	}

	return	iRet;
}

int mtQueueUser::exit( void )
{

	DeleteCriticalSection(&m_kCirticalSection);
	m_iNodeTotal	= 0;
	return m_pkIdQueue.exit();


}

 /************************************************************************
  * @function:	initDataNode                                                     
  * @describe:	初始化好的元素的指针
  * @param-in:	
		-uiSocket		输入socket
  * @return:	
 		-成功			1
 		-失败			0
  * @author:		fuke
  ************************************************************************
  */
int mtQueueUser::initDataNode( DataNode* pkDataNode, SOCKET uiSocket )
{
	memset (pkDataNode, 0, sizeof(DataNode));
	pkDataNode->lStructBytes		= sizeof(DataNode);
	pkDataNode->uiSockets			= uiSocket;

	HANDLE	hIOCP = CreateIoCompletionPort((HANDLE)uiSocket, m_hIOCP, (ULONG_PTR)pkDataNode, NULL);

	return	(NULL == hIOCP) ? 0 : 1;
}

 /************************************************************************
  * @function:	exitDataNode                                                     
  * @describe:	删除一个结点相关的资源
  * @param-in:	
		-pkDataNode		输入结点
		-mtQueuePacket	数据包队列
  * @return:	
 		-成功			1
 		-失败			0
  * @author:		fuke
  ************************************************************************
  */

int mtQueueUser::exitDataNode( DataNode* pkQueueUserDataNode, mtQueuePacket* pkQueuePacket )
{

	MT_DEBUG_PTCL("\exitDataNode start !!!\n");

	int		iRetCloseSocket;

	if (INVALID_SOCKET != pkQueueUserDataNode->uiSockets)
	{
		MT_DEBUG_PTCL("\nexitDataNode [id:%d-socket:%d]", pkQueueUserDataNode->lUserId, pkQueueUserDataNode->uiSockets);
		iRetCloseSocket = closesocket(pkQueueUserDataNode->uiSockets);
	}

	mtQueuePacket::DataNode*	pkQueuePacketDataNode = pkQueueUserDataNode->pkQueuePacketDataNodeHead;
	mtQueuePacket::DataNode*	pkQueuePacketDataNodeDelete;

	DWORD i = 0;
	while (NULL != pkQueuePacketDataNode)
	{
		i++;
		MT_DEBUG_PTCL("\npkQueuePacketDataNode while  %d  %x %d!!!",i,pkQueuePacketDataNode,pkQueuePacketDataNode->eDataType);

		if(pkQueuePacketDataNode == pkQueuePacketDataNode->pkNext)
		{
			MT_DEBUG_PTCL("\nerror exit!!!\n");
			//break;
		}

		pkQueuePacketDataNodeDelete	= pkQueuePacketDataNode;
		pkQueuePacketDataNode		= pkQueuePacketDataNode->pkNext;
		pkQueuePacket->push(pkQueuePacketDataNodeDelete);
	}

	/// memset (pkQueueUserDataNode, 0, sizeof(DataNode));
	pkQueueUserDataNode->uiSockets	= INVALID_SOCKET;

	int		iRetPush				= push(pkQueueUserDataNode);

	return 1;
	//return	((SOCKET_ERROR != iRetCloseSocket) && (iRetPush >= 0)) ? 1 : 0;
}

 /************************************************************************
  * @function:	popInitDataNode                                                     
  * @describe:	从队列中弹出一个已经初始化好的元素的指针
  * @param-in:	
		-uiSocket		输入socket
  * @return:	
 		-成功			非NULL
 		-失败			NULL
  * @author:		fuke
  ************************************************************************
  */

void* mtQueueUser::popInitDataNode(SOCKET uiSocket)
{
	DataNode*	pkDataNode	= pop();

	if (NULL != pkDataNode)
	{
		int iRet = initDataNode(pkDataNode, uiSocket);

		if (1 == iRet)
		{
			return	pkDataNode;
		}

		push(pkDataNode);
	}	

	return	NULL;

}

int mtQueueUser::addDataNodePacket( DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket )
{
	if ((NULL == pkDataNodeUser) || (NULL == pkDataNodePacket))
	{
		return	0;
	}
	////////////////////////////////
	/*else{
		EnterCriticalSection(&m_kCirticalSection);

		mtQueuePacket::DataNode*	pkQueuePacketDataNode = pkDataNodeUser->pkQueuePacketDataNodeHead;

		DWORD i = 0;
		while (pkQueuePacketDataNode)
		{
			i++;
			MT_DEBUG_PTCL("\naddDataNodePacket while  %d  %d !!!",i,pkQueuePacketDataNode);
			if(pkDataNodePacket == pkQueuePacketDataNode){
				MT_DEBUG_PTCL("\nerror node!!!\n");
				//break;
			}
			if(pkQueuePacketDataNode == pkQueuePacketDataNode->pkNext){
				MT_DEBUG_PTCL("\nadd same!!!\n");
				//break;
			}
			if(pkQueuePacketDataNode->pkNext == pkQueuePacketDataNode->pkNext->pkNext){
				MT_DEBUG_PTCL("\nadd same!!!\n");
				break;
			}
			pkQueuePacketDataNode		= pkQueuePacketDataNode->pkNext;
		}
		LeaveCriticalSection(&m_kCirticalSection);
	}*/
	////////////////////////////////

	EnterCriticalSection(&m_kCirticalSection);

	pkDataNodePacket->pkPre					= NULL;
	pkDataNodePacket->pkNext				= pkDataNodeUser->pkQueuePacketDataNodeHead;

	if (pkDataNodeUser->pkQueuePacketDataNodeHead)
	{
		pkDataNodeUser->pkQueuePacketDataNodeHead->pkPre	= pkDataNodePacket;
	}

	pkDataNodeUser->pkQueuePacketDataNodeHead	= pkDataNodePacket;
	LeaveCriticalSection(&m_kCirticalSection);
	return	1;
}

void mtQueueUser::printUserPacket(DataNode* pkDataNodeUser)
{
	mtQueuePacket::DataNode*	pkQueuePacketDataNode = pkDataNodeUser->pkQueuePacketDataNodeHead;
	int i = 0;
	while (NULL != pkQueuePacketDataNode)
	{
		i++;
		MT_DEBUG_PTCL("\nUserPacket info[%d][%d][%x][%d]",pkDataNodeUser->lUserId,i,pkQueuePacketDataNode,pkQueuePacketDataNode->eDataType);

		if(pkQueuePacketDataNode == pkQueuePacketDataNode->pkNext)
		{
			MT_DEBUG_PTCL("\nprint UserPacket error exit!!!\n");
		}

		pkQueuePacketDataNode		= pkQueuePacketDataNode->pkNext;

	}
}

int mtQueueUser::removeDataNodePacket( DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket )
{

	if ((NULL == pkDataNodeUser) || (NULL == pkDataNodePacket))
	{
		return	0;
	}

	EnterCriticalSection(&m_kCirticalSection);

	if (NULL != pkDataNodePacket->pkPre)					//如果是不是头节点
	{
		pkDataNodePacket->pkPre->pkNext				= pkDataNodePacket->pkNext;
	}
	else											//如果是头节点
	{
		if(NULL != pkDataNodePacket->pkNext)
			pkDataNodePacket->pkNext->pkPre = NULL;
		pkDataNodeUser->pkQueuePacketDataNodeHead	= pkDataNodePacket->pkNext;
		//if(pkDataNodeUser->pkQueuePacketDataNodeHead)
			//pkDataNodeUser->pkQueuePacketDataNodeHead->pkPre = NULL;
	}

	if (NULL != pkDataNodePacket->pkNext)					//如果是中间节点
	{
		pkDataNodePacket->pkNext->pkPre				= pkDataNodePacket->pkPre;
	}

	LeaveCriticalSection(&m_kCirticalSection);

	return	1;
}

int mtQueueUser::DataRead( DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket )
{
	pkDataNodePacket->eDataType	= mtQueuePacket::E_DATA_TYPE_READ;
	if (addDataNodePacket(pkDataNodeUser, pkDataNodePacket))
	{
		MT_DEBUG_PTCL("\nUser post a read packet!UserId:%d,packet:%x",pkDataNodeUser->lUserId,pkDataNodePacket);
		int	iRet	= WSARecv(pkDataNodeUser->uiSockets, &(pkDataNodePacket->kWSABuf), 1, 
			 		&(pkDataNodePacket->ulTransmitBytes), &(pkDataNodePacket->ulFlag), 
					&(pkDataNodePacket->kDataOverLapped), NULL);
		printUserPacket(pkDataNodeUser);
		if ((0 == iRet) || (WSA_IO_PENDING == WSAGetLastError()))
		{
			return	1;
		}
		else
		{
			MT_DEBUG_PTCL("\nUser post read packet ERROR!UserId:%d,packet:%x,ERROR_CODE:%d",pkDataNodeUser->lUserId,pkDataNodePacket,WSAGetLastError());
		}

		int iErrorCode = WSAGetLastError();

		if(WSAECONNABORTED == iErrorCode ||WSAECONNRESET == iErrorCode)
		{
			PostQueuedCompletionStatus(m_hIOCP, 0,  (ULONG_PTR)pkDataNodeUser, &(pkDataNodePacket->kDataOverLapped));
		}
	}

	return	0;
}
 
int mtQueueUser::DataWrite( DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket )
{
	pkDataNodePacket->eDataType = mtQueuePacket::E_DATA_TYPE_WRITE;
	if (addDataNodePacket(pkDataNodeUser, pkDataNodePacket))
	{
		MT_DEBUG_PTCL("\nUser post a write packet!UserId:%d,packet:%x",pkDataNodeUser->lUserId,pkDataNodePacket);
		int	iRet	= WSASend(pkDataNodeUser->uiSockets, &(pkDataNodePacket->kWSABuf), 1, 
			&(pkDataNodePacket->ulTransmitBytes), pkDataNodePacket->ulFlag, 
			&(pkDataNodePacket->kDataOverLapped), NULL);

		printUserPacket(pkDataNodeUser);
		if ((0 == iRet) || (WSA_IO_PENDING == WSAGetLastError()))
		{
			return	1;
		}
		else
		{
			MT_DEBUG_PTCL("\nUser post write packet ERROR!UserId:%d,packet:%x",pkDataNodeUser->lUserId,pkDataNodePacket);
		}

		int iErrorCode = WSAGetLastError();

		if(WSAECONNABORTED == iErrorCode ||WSAECONNRESET == iErrorCode)
		{
		   PostQueuedCompletionStatus(m_hIOCP, 0,  (ULONG_PTR)pkDataNodeUser, &(pkDataNodePacket->kDataOverLapped));
		}
	}


	return	0;
}

long mtQueueUser::getNumberOfReadPacket(DataNode* pkDataNodeUser)
{
	mtQueuePacket::DataNode*	pkQueuePacketDataNode = pkDataNodeUser->pkQueuePacketDataNodeHead;

	DWORD dwNumberOfReadPacket = 0;

	while (NULL != pkQueuePacketDataNode)
	{
		if(mtQueuePacket::E_DATA_TYPE_READ == pkQueuePacketDataNode->eDataType)
		   ++dwNumberOfReadPacket;

		pkQueuePacketDataNode		= pkQueuePacketDataNode->pkNext;
	}
	return dwNumberOfReadPacket;
}