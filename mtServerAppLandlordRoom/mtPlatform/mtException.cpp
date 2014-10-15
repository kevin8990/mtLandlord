#include "mtException.h"

mtException::mtException()
{
	m_iExceptionId	= 0;
}

mtException::~mtException()
{

}

void	mtException::condition( bool bCondition, int iExceptionId )
{
	if (bCondition)
	{
		m_iExceptionId	= iExceptionId;
		throw	this;
	}
}
