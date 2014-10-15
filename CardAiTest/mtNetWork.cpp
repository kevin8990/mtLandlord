#include <WinSock2.h>
#include "mtNetWork.h"
#include <iostream>
using namespace std;


mtNetWork::mtNetWork()
{
	m_SocketClient = INVALID_SOCKET;
	m_pkServerAddress = NULL;
}

mtNetWork::~mtNetWork()
{
	/*if (m_SocketClient != INVALID_SOCKET)
	{
		closesocket(m_SocketClient);
	}*/
}

bool mtNetWork::init(SOCKET socketClient,SOCKADDR_IN* pkServerAddress)
{
	
	m_SocketClient    = socketClient;
	m_pkServerAddress = pkServerAddress;
	return true;
}

bool mtNetWork::WriteData(char *buffer,int iBufferBytes)        //发送数据 参数为要传的数据和大小
{
	int iSendTotal = 0;
	int iReturn = send(m_SocketClient,buffer,iBufferBytes,0);
	if (iReturn == SOCKET_ERROR)
	{
		cout<<"发送数据失败，失败代码;"<<GetLastError()<<endl;
		return false;
	}
	iSendTotal = iReturn;
	for (int i = 0; iSendTotal != iBufferBytes && i <3; i++)
	{
		iReturn = send(m_SocketClient,buffer,iBufferBytes,0);
		iSendTotal += iReturn;
	}
	if (iSendTotal != iBufferBytes)
	{
		return false;
	}
	return true;
}

bool mtNetWork::ReadData(char *buffer,int iBufferBytes)        //接收数据
{
		int iReturn = recv(m_SocketClient,buffer,iBufferBytes,0);  //阻塞的函数 
		if (iReturn == SOCKET_ERROR)
		{
			cout<<"接收数据失败，失败代码;"<<GetLastError()<<endl;
			return false;
		}
		return true;
}

bool mtNetWork::ConnectServer()
{
	int iReturn = connect(m_SocketClient,(SOCKADDR*)m_pkServerAddress,sizeof(SOCKADDR_IN));
	if (iReturn == SOCKET_ERROR) //连接失败
	{
		cout<<"连接失败，失败代码;"<<GetLastError()<<endl;
		return false;
	}
	return true;

}