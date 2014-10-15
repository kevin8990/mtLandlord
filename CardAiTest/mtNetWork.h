#pragma once 
#include <windows.h>
class mtNetWork
{
public:
	mtNetWork();
	~mtNetWork();
public:
	bool   init(SOCKET socketClient,SOCKADDR_IN* pkServerAddress);  //实现创建套接字，接收服务器地址
	bool   ConnectServer();   //连接服务器
	bool   WriteData(char *buffer,int iBufferBytes);        //发送数据
	bool   ReadData(char *buffer,int iBufferBytes);         //接收数据
public:
	SOCKET m_SocketClient;       //客户端的套接字  在该类对象初始化的时候 创建
	SOCKADDR_IN*     m_pkServerAddress; //服务器地址结构体指针

};