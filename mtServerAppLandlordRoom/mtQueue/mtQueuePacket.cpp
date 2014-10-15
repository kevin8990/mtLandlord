#include "mtQueuePacket.h"


mtQueuePacket::mtQueuePacket()
{
	m_iNodeTotal	= MT_NODE_QUEUE_PACKET;
	m_iSpinCount	= 0;
}

mtQueuePacket::~mtQueuePacket()
{

}
 
int mtQueuePacket::init(void* pData)
{
	DataInit*	pkDataInit = (DataInit*)pData;

	m_iSpinCount			= pkDataInit->iSpinCount;

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

int mtQueuePacket::exit( void )
{
	m_iNodeTotal	= 0;
	m_iSpinCount	= 0;
	return m_pkIdQueue.exit();
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
void* mtQueuePacket::popInitDataNode(char* location)
{
	DataNode*	pkDataNode	= pop();

	//MT_DEBUG_PTCL("\npop %d %x %s",(m_pkIdQueue.m_iNodeIdxBack-1),pkDataNode,location);

	if (NULL != pkDataNode)
	{
		memset (pkDataNode, 0, sizeof(DataNode) - MT_BYTES_DATA_PACKET);

		pkDataNode->kWSABuf.len		= sizeof(pkDataNode->pcData);
		pkDataNode->kWSABuf.buf		= pkDataNode->pcData;

		return	pkDataNode;
	}

	return	NULL;
}

