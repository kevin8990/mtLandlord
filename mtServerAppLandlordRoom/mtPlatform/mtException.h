#ifndef		__MT_EXCEPTION_H
#define		__MT_EXCEPTION_H



class	mtException 
{
// #define		MT_EXCEPTION_DEBUG(_bCondition, _iErrorId)				\
// 	if ((_bCondition))	{										\
// 	m_iExceptionId = (_iErrorId);									\
// 	MT_DEBUG(9, "%d\t%s\n", __LINE__, __FILE__);			\
// 	throw this;									\
// 	} else {													\
// 	MT_DEBUG(8, "%d\t%s\n", __LINE__, __FILE__);			\
// 	}	

#define			MT_EXCEPTION_DEBUG	condition
#define			MT_EXCEPTION_SQL_DEBUG(_sSqlRet, _iException)		\
	condition((SQL_SUCCESS != (_sSqlRet)) && (SQL_SUCCESS_WITH_INFO != (_sSqlRet)), _iException)

#define			MT_EXCETPION_STRING(strException)	\
	case	strException:	\
	{	\
		MT_DEBUG(7, "%s\t%s\n", typeid(*this).name(), MT_GET_NAME(strException));	\
	}	\
	break;

public:
	mtException();
	virtual ~mtException();

	void		condition(bool bCondition, int iExceptionId);
	virtual int exception()	= 0;

protected:
	int			m_iExceptionId;
};

#endif ///	__MT_EXCEPTION_H