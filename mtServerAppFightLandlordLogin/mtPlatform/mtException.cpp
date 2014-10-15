#include "mtException.h"

mtException::mtException()
{
	m_iExceptionId	= 0;			//初始化错误代码
}

mtException::~mtException()
{

}

//bCondition为false时，记录异常并抛出
void	mtException::condition( bool bCondition, int iExceptionId )
{
	if (!bCondition)
	{
		m_iExceptionId	= iExceptionId;
		throw	this;
	}
}
