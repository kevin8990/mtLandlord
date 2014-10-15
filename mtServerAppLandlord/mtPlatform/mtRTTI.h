#ifndef		__MT_RTTI_H
#define		__MT_RTTI_H

template <class T>
class	mtRTTI 
{
public:
	virtual const char* name();
};

template <class T>
const char* mtRTTI<T>::name()
{
	return	typeid(T).name();
}

#endif ///	__MT_RTTI_H