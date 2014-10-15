#ifndef		__MT_EXCEPTION_H
#define		__MT_EXCEPTION_H
//“Ï≥£¿‡
class	mtException 
{

#define		MT_EXCEPTION_DEBUG	condition

#define		MT_EXCEPTION_DEBUG_IOCP(_iRet, _iExceptionId)		\
				condition(((TRUE == (_iRet)) || (WSA_IO_PENDING == WSAGetLastError())), _iExceptionId)

#define		MT_EXCETPION_STRING(strException)	\
				case	strException:	\
				{	\
					MT_DEBUG(7, "%s\t%s\n", typeid(*this).name(), MT_GET_NAME(strException));	\
				}	\
				break;

public:
	mtException();
	virtual ~mtException();

	void	condition(bool bCondition, int iExceptionId);
	virtual	int exception()	= 0;

protected:
	int			m_iExceptionId;						//¥ÌŒÛ¥˙¬Î
};

#endif ///	__MT_EXCEPTION_H