#include "mtInclude.h"
#define  LOG_FILE_FOLDER_NAME   "RunLog"           //日志文件夹
#define  THREAD_NUMBER    50                    //线程数量
#define  PORT             9099
#define  IP               "192.168.0.198"
#define  MAX_ROUND        10
#define  SLEEP_CARD_OUT_TIME       1000*2           //出牌等待时间
#define  SLEEP_CALL_START_TIME     1000*5           //重叫开始等带时间
static long sl_UserId = 1073;
SOCKADDR_IN  serverAddress;  //全局地址
CRITICAL_SECTION g_cs;

void  SaftyRelease(void *p); 

bool  initNetModule();

void  initThreadModule();

void  Log( FILE* pFiLE,char * pcformat, ... );            

bool  GetRunExeDir(char* pDir, DWORD dwLen);    //得到运行程序的路径

int   WSDCreateDirectory(const char* pcDir);    //新建目录 参数为全路径 其实就是新建一个文件夹

int   ReadData(SOCKET socket, char *pcBuffer, long bufferSize);  //读取网络数据

int   main()
{  
	//网络
	if(!initNetModule())
	{
		cout<<"初始化网络失败"<<endl;
		return 0;
	}
	//线程
	InitializeCriticalSection(&g_cs);

	initThreadModule();
	
	Sleep(INFINITE);  //主线程一直等待

	WSACleanup();
	return 0;
}

DWORD WINAPI ThreadWorkFun( LPVOID lpParam )  //线程函数
{
	EnterCriticalSection(&g_cs);
	sl_UserId++;
	long lUserAiId = sl_UserId; //得到自己id
	LeaveCriticalSection(&g_cs);
	long lLastAiId = 0; //上户id
	long lNextAiId = 0; //下户id
	unsigned long lDeskId   = 0;  //桌子id
	int  iLastAiCardNum = 17;          //上户手中的牌
	int  iNextAiCardNum = 17;          //下户手中的牌
	long lRealOutCardPlay = 0;  //真正出牌的玩家 （过牌不算）
	char CardUserCard[24] = {0}; //用户的牌 服务端
	char CardBaseCard[7]={0};    //底牌     服务端
	mtCardInfo CardUserInfo[24];  //用户的牌 客户端
	mtCardInfo CardBaseInfo[7];   //底牌     客户端
	mtCardInfo CardSlectInfo[24];      //要出的牌  客户端
	char       CardSlect[24] = {0};    //要出的牌  服务端
	mtCardInfo CardInDeskInfo[24];     //桌子上刚刚出的牌 客户端
	int     iResult = -1;
	char    buffer[10*1024] ={0};         //用来接收数据的缓冲区
	int     iCompleteRound = 0;            //在同一桌子里 完成的局数
	int     iCompleteLunShu = 0;
	srand( (unsigned)time( NULL ) );       
	int iRoundTotal = 1 + rand()%10;        //在同一桌子里 要完成的局数
	bool bStartFlag = false;                //开始标志
	SOCKET clientSocket = INVALID_SOCKET;   //通讯套接字

	FILE*      pLogFile = NULL;            //日志文件指针
	char cExeDir[500] = {0};               //保存当前程序的路径
	char cLogDir[500] = {0};               //日志文件的路径

	GetRunExeDir(cExeDir,500);               //得到运行路径
	strcpy(cExeDir + strlen(cExeDir), "\\");
	strcpy(cLogDir, cExeDir);
	strcpy(cLogDir + strlen(cLogDir), LOG_FILE_FOLDER_NAME);

	WSDCreateDirectory(cLogDir);
	strcpy(cLogDir+strlen(cLogDir),"\\AiId");
	char buf[500] = {0};
	sprintf(buf,"%s%d.log",cLogDir,lUserAiId);
	if (pLogFile)
	{
		fclose(pLogFile);
		pLogFile = NULL;
	}
	pLogFile = fopen(buf, "a+");
	if (NULL == pLogFile)
	{
		return 0;
	}
	mtCardAI *pkCardAi = new mtCardAI;
	if (!pkCardAi)
	{
		SaftyRelease(pkCardAi);
		Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
		return 0;
	}
	mtNetWork *pkNetWork = new mtNetWork();
	if (!pkNetWork)
	{
		SaftyRelease(pkCardAi);
		SaftyRelease(pkNetWork);
		Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
		return 0;
	}
	Log(pLogFile,"用户一轮需要完成%d局\n",iRoundTotal);
	 //进入大循环
	while (1)   //重复连接                                                     
	{
step1:  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //创建套接字
		if (clientSocket == INVALID_SOCKET)
		{
			cout<<"套接字创建失败"<<endl;
			cout<<GetLastError()<<endl;
			SaftyRelease(pkCardAi);
			SaftyRelease(pkNetWork);
			Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
			return 0;
		}

		if(!pkNetWork->init(clientSocket,&serverAddress)) //
		{
			SaftyRelease(pkCardAi);
			SaftyRelease(pkNetWork);
			if (clientSocket != SOCKET_ERROR)
			{
				closesocket(clientSocket);
			}
			return 0;
		}

	    if(!pkNetWork->ConnectServer())  //建立好连接
		{
			SaftyRelease(pkNetWork);
			SaftyRelease(pkCardAi);
			if (clientSocket != SOCKET_ERROR)
			{
				closesocket(clientSocket);
			}
			Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
			return 0;
		}
		//数据重置
		iLastAiCardNum = 17;
		iNextAiCardNum = 17;
		iCompleteRound = 0;
		bStartFlag     = false;
		iCompleteLunShu++;                //轮数加1
		for (int i = 0; i<24; i++)  //要清理一下桌面当前的牌信息
		{
			CardInDeskInfo[i].cCardType = 0;
			CardInDeskInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
		}
		//发送连接协议数据 第一次发送数据
		mtServiceRoomConnect kServiceRoomConnect;
		kServiceRoomConnect.init(lUserAiId);
		if(!(pkNetWork->WriteData((char*)&kServiceRoomConnect.m_kConnectDataWrite,
			                       sizeof(kServiceRoomConnect.m_kConnectDataWrite))))  //发送连接数据 阻塞方式

		{
			SaftyRelease(pkNetWork);
			SaftyRelease(pkCardAi);
			if (clientSocket != SOCKET_ERROR)
			{
				closesocket(clientSocket);
			}
			Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
			return 0;
		}
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);
		Log(pLogFile,"发送连接协议 %d:%d:%d\n",curTime.wHour,curTime.wMinute,curTime.wSecond);
		while (1)                     //不断重复的接收数据
		{
			//Log(pLogFile,"\n准备接收协议......\n");
			memset(buffer,0,10*1024*sizeof(char)); //分配一个大的区间 存接收的数据
	
			int iReturn = ReadData(clientSocket,buffer,10*1024);  //select模型
			//int iReturn = recv(clientSocket,buffer,10*1024,0);      //普通方法
			if (iReturn > 0)  //接收数据没有错误 开始处理
			{
				long *lpCurPos = (long*)buffer;  //强转
				//必须处理一下得到的数据 可能会包含有几个协议  
				long lDataSize = lpCurPos[0];
				while (iReturn >= lDataSize)             // 一次性接了几个协议
				{
					if (iReturn == lDataSize) //只剩了一个
					{
						switch (lpCurPos[1]) //得到协议的类型
						{
						case E_SERVICE_ROOM_CONNECT:                         //连接
							{
								Log(pLogFile,"接收连接协议\n");
								mtServiceRoomConnect::DataRead *pkConnectDataRead = (mtServiceRoomConnect::DataRead*)(lpCurPos);
								iResult = pkConnectDataRead->lResult;
								if (iResult == 1)  //连接成功了
								{
									Log(pLogFile,"连接成功\n");
									//发送等待开始的协议
									/*mtServiceRoomWaitStart kServiceRoomWaitStart;
									kServiceRoomWaitStart.init();
									if(!(pkNetWork->WriteData((char*)(&kServiceRoomWaitStart.m_kWaitStartDataWrite),sizeof(kServiceRoomWaitStart.m_kWaitStartDataWrite))))
									{
									SaftyRelease(pkNetWork);
									SaftyRelease(pkCardAi);
									return 0;
									}*/
								}
								else 
								{
									Log(pLogFile,"连接出现异常返回%d\n",iResult);
									SaftyRelease(pkNetWork);
									SaftyRelease(pkCardAi);
									if (clientSocket != SOCKET_ERROR)
									{
										closesocket(clientSocket);
									}
									Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
									return 0;
									
									
								}														
							}
							break;
						case E_SERVICE_ROOM_DESK_TWO:          //组建双人桌子
							{
								Log(pLogFile,"\n接收双人桌子组建好协议\n");
								if(!bStartFlag)  //如果没有叫开始
								{
									mtServiceRoomWaitStart kServiceRoomWaitStart;
									kServiceRoomWaitStart.init();
									if(!(pkNetWork->WriteData((char*)(&kServiceRoomWaitStart.m_kWaitStartDataWrite),sizeof(kServiceRoomWaitStart.m_kWaitStartDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile,"发送双人桌子组好开始协议\n");
									bStartFlag = true;

								}
								
							}
							break;
						case E_SERVICE_ROOM_DESK_THREE:       //组建三人桌子 
							{
								Log(pLogFile,"接收三人桌子组建好协议\n");
								mtServiceRoomDeskThree::DataRead *pkDeskThreeDataRead = (mtServiceRoomDeskThree::DataRead*)(lpCurPos);
								lLastAiId = pkDeskThreeDataRead->kUserInfo[0].lUserId;  //得到上家id
								lNextAiId = pkDeskThreeDataRead->kUserInfo[1].lUserId;  //得到下家id
								lDeskId   = pkDeskThreeDataRead->kUserInfo[0].ulDeskId; //得到桌子id
								Log(pLogFile,"\n*******第%d轮第%d局开始,收到三人桌子建好协议,上家id:%d,下家id:%d,桌子id:%x*******\n",iCompleteLunShu,iCompleteRound + 1,lLastAiId,lNextAiId,lDeskId);							
								if (!bStartFlag)
								{
									mtServiceRoomWaitStart kServiceRoomWaitStart;
									kServiceRoomWaitStart.init();
									if(!(pkNetWork->WriteData((char*)(&kServiceRoomWaitStart.m_kWaitStartDataWrite),sizeof(kServiceRoomWaitStart.m_kWaitStartDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile,"发送三人桌子组建好的开始协议\n");
									bStartFlag = true;
								}
							}
							break;
						case E_SERVICE_ROOM_WAIT_START:       //开始协议    
							{
								mtServiceRoomWaitStart::DataRead *pkWaitStartDataRead = (mtServiceRoomWaitStart::DataRead*)(lpCurPos);
								Log(pLogFile,"接收开始协议%d\n",pkWaitStartDataRead->lUserId);
								 //要清理一下桌面当前的牌信息  因为重新开始 会到这里来 在这里重置一下数据
								iLastAiCardNum = 17;
								iNextAiCardNum = 17;
								lRealOutCardPlay = 0;
								for (int i = 0; i<24; i++) 
								{
									CardInDeskInfo[i].cCardType = 0;
									CardInDeskInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
								}
							}
							break;
						case E_SERVICE_ROOM_CARD_DEAL:         //牌过来了 接到牌 
							{
								Log(pLogFile,"接到洗牌发牌协议\n");
								mtServiceRoomCardDealRecv::DataRead *pkCardDealRecvDataRead = (mtServiceRoomCardDealRecv::DataRead*)(lpCurPos);
								// 先存好牌
								for (int i = 0; i<24; i++)
								{
									CardUserInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
									CardUserInfo[i].cCardType = 0;
									CardUserCard[i] = MTFL_NUM_CARD_TERMINAL;
									if (i<7)
									{
										CardBaseCard[i] = MTFL_NUM_CARD_TERMINAL;
									}
								}
								memcpy(CardUserCard,pkCardDealRecvDataRead->pcCards,sizeof(char)*17);
								memcpy(CardBaseCard,pkCardDealRecvDataRead->pcBaseCards,sizeof(char)*7);
								pkCardAi->cardConvert(CardUserCard,CardUserInfo);
								pkCardAi->sortCard(CardUserInfo);
								
								Log(pLogFile,"自己牌面：");
								pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);
								if( pkCardDealRecvDataRead->lDefaultLandlordUserId == lUserAiId )  // 自己被选中抢地主
								{
									//发送一个抢地主的协议
									Log(pLogFile,"自己被选中抢地主\n");
									mtServiceRoomGrabLandlord kServiceRoomGrabLandlord;
									kServiceRoomGrabLandlord.init();
									if (!(pkNetWork->WriteData((char*)&(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite),sizeof(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile,"发送抢地主协议 3分\n");
								}
								else                                                         //默认抢地主的不是自己
								{
									//Log(pLogFile,"等待别人抢地主叫分\n");
								}

							}
							break;
						case E_SERVICE_ROOM_GRAB_LANDLORD:                                   //抢地主协议
							{
								Log(pLogFile,"接到抢地主协议\n");
								mtServiceRoomGrabLandlord::DataRead *pkGrabLandLordDataRead = (mtServiceRoomGrabLandlord::DataRead*)(lpCurPos);
								if (pkGrabLandLordDataRead->lUserIdLandlord > 0)   //已经有地主了
								{
								
									if ( pkGrabLandLordDataRead->lUserIdLandlord == lUserAiId )     // 自己抢到了地主 底牌要加进自己的牌面里面 还要翻译一下                                                    
									{
										//发送出牌协议
										Log(pLogFile,"自己是地主\n");
										for (int i = 0; i<24; i++)
										{
											CardUserInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
											CardUserInfo[i].cCardType = 0;
											CardSlectInfo[i].cCardType = 0;
											CardSlectInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
											CardSlect[i] = MTFL_NUM_CARD_TERMINAL;
											if (i<7)
											{
												CardBaseInfo[i].cCardType = 0;
												CardBaseInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
											}
										}
										pkCardAi->cardConvert(CardUserCard,CardUserInfo);     //服务器的牌 转换为客户端的牌
										pkCardAi->cardConvert(CardBaseCard,CardBaseInfo);
										pkCardAi->addInCard(CardUserInfo,CardBaseInfo);      //添加 底牌  所有的只要保证了 后面试以 MTFL_NUM_CARD_TERMINAL 结束 就安全的
										pkCardAi->sortCard(CardUserInfo);                    //排列一下
										//执行第一次出牌操作 发出牌协议过去
										pkCardAi->getCardOut(CardUserInfo,NULL,MT_FL_CARD_TYPE_BEG,CardSlectInfo);//得到出牌的内容
										mteCardType eCardType = pkCardAi->getTypeFromCard(CardSlectInfo);          //得到出牌的类型
										pkCardAi->cardConvert(CardSlectInfo,CardSlect); //出的牌转换一下 用来发送到服务端
										mtServiceRoomCardOut kServiceRoomCardOut;
										kServiceRoomCardOut.init(eCardType,CardSlect);
										Sleep(SLEEP_CARD_OUT_TIME);
										if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
										{
											SaftyRelease(pkNetWork);
											SaftyRelease(pkCardAi);
											if (clientSocket != SOCKET_ERROR)
											{
												closesocket(clientSocket);
											}
											Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
											return 0;
										}
										
										Log(pLogFile,"自己发送出牌协议,牌内容：");
										pkCardAi->dispalyCardInfo(CardSlectInfo,pLogFile);
										//整理一下自己的牌面
										pkCardAi->takeAwayCard(CardUserInfo,CardSlectInfo);
										pkCardAi->sortCard(CardUserInfo);
										Log(pLogFile,"自己手中剩余的牌：");
										pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);
									}
									else if(pkGrabLandLordDataRead->lUserIdLandlord == lLastAiId)
									{
										
										Log(pLogFile,"地主是上家:%d\n",pkGrabLandLordDataRead->lUserIdLandlord);
										iLastAiCardNum = 20;
									}
									else if(pkGrabLandLordDataRead->lUserIdLandlord == lNextAiId)
									{
										Log(pLogFile,"地主是下家:%d\n",pkGrabLandLordDataRead->lUserIdLandlord);
										iNextAiCardNum = 20;
									}
									else
									{
										cout<<"****"<<endl;
									}
								}
								if (pkGrabLandLordDataRead->lUserIdLandlord == -1)  //还没有产生地主
								{
									Log(pLogFile,"还没有产生地主\n");
									if (pkGrabLandLordDataRead->lUserId == lLastAiId) //是上一家叫的地主
									{
										//自己抢地主
										
										Log(pLogFile,"上家抢地主分数:%d\n",pkGrabLandLordDataRead->lScore);
										mtServiceRoomGrabLandlord kServiceRoomGrabLandlord;
										kServiceRoomGrabLandlord.init();
										if (!(pkNetWork->WriteData((char*)&(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite),sizeof(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite))))
										{
											SaftyRelease(pkNetWork);
											SaftyRelease(pkCardAi);
											if (clientSocket != SOCKET_ERROR)
											{
												closesocket(clientSocket);
											}
											Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
											return 0;
										}
										Log(pLogFile,"发送抢地主协议 分数：%d\n",kServiceRoomGrabLandlord.m_kGrablandlordDataWrite.lScore);
									}
									else
									{
										Log(pLogFile,"下家抢地主\n");
									}
						
								}
							}
							break;
						case E_SERVICE_ROOM_CARD_OUT:                                     //出牌协议 
							{
								mtServiceRoomCardOut::DataRead *pkCardOutDataRead = (mtServiceRoomCardOut::DataRead*)(lpCurPos);
								mteCardType eCardType = (mteCardType)pkCardOutDataRead->lCardType; //得到出牌的类型
								long lUserId = pkCardOutDataRead->lUserId; //得到出牌人的id（过牌的也算）
								int iCardOutNum = 0;                 //当前出牌的张数
								for (int i = 0; i<24; i++)
								{
									CardSlectInfo[i].cCardType = 0;
									CardSlectInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
									CardSlect[i] = MTFL_NUM_CARD_TERMINAL;
								}
								if (eCardType != MT_FL_CARD_TYPE_PASS) //如果不是过牌  就要更新一下桌子当前的牌信息 还有真实的出牌人（出了实牌的人）
								{
									char *pCardOut = pkCardOutDataRead->pcCard;  //得到桌子上刚刚出的牌
									for (int i = 0; i<24; i++)
									{
										CardInDeskInfo[i].cCardType = 0;
										CardInDeskInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;	
									}
									pkCardAi->cardConvert(pCardOut,CardInDeskInfo);//转换成客户端的牌类型 放CardInDeskInfo里
									pkCardAi->sortCard(CardInDeskInfo);
									Log(pLogFile,"\n接收出牌协议,出牌人%d,出牌内容：",lUserId);
									pkCardAi->dispalyCardInfo(CardInDeskInfo,pLogFile);
									lRealOutCardPlay = pkCardOutDataRead->lUserId;//更新一下 真正的出牌玩家 过的不算 
									iCardOutNum = pkCardAi->getCardNum(CardInDeskInfo); //得到出牌的张数
								}
								else                                                                //过牌
								{
									//让CardInDeskInfo 保存上一次桌面的牌
									Log(pLogFile,"\n接收出牌协议,出牌人%d,出牌内容：过\n",lUserId);

								}
								Log(pLogFile,"当前桌上牌面是:");
								pkCardAi->dispalyCardInfo(CardInDeskInfo,pLogFile);

								if (lUserId == lNextAiId)                                //下家出的牌  
								{
									iNextAiCardNum -= iCardOutNum;
									if (iNextAiCardNum <= 0)                         //下家已经出完了牌  发送结束一局协议 也可能会发送请求人家牌面协议 再看
									{
										Log(pLogFile,"下家剩余牌为0\n");
										mtServiceRoomRoundOver kServiceRoomRoundOver;
										kServiceRoomRoundOver.init();
										if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
										{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
										closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
										}
										Log(pLogFile,"自己发送结束一局协议\n");
									}
									else
									{
										//Log(pLogFile,"下家的出牌，不要\n");
									}
								}
								else if (lUserId == lLastAiId)                            //上家出的牌 那要压牌
								{
									if (lUserAiId == lRealOutCardPlay)           //上次真正出牌的是自己
									{
										//判断一下自己手中的牌
										if (!pkCardAi->getCardNum(CardUserInfo))  //手中没有牌了
										{
											//发送 结束一局的协议过去
										    Log(pLogFile,"自己剩余牌为0\n");
											mtServiceRoomRoundOver kServiceRoomRoundOver;
											kServiceRoomRoundOver.init();
											if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
											{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
											}
											Log(pLogFile,"发送结束一局协议\n");

										}
										else //还有牌 继续出牌
										{
											
											//执行第一次出牌操作 发出牌协议过去
											pkCardAi->getCardOut(CardUserInfo,NULL,MT_FL_CARD_TYPE_BEG,CardSlectInfo);//得到出牌的内容
											mteCardType eCardType = pkCardAi->getTypeFromCard(CardSlectInfo);          //得到出牌的类型
											pkCardAi->cardConvert(CardSlectInfo,CardSlect); //出的牌转换一下 用来发送到服务端
											mtServiceRoomCardOut kServiceRoomCardOut;
											kServiceRoomCardOut.init(eCardType,CardSlect);
											Sleep(SLEEP_CARD_OUT_TIME); //休息一会
											if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
											{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
											}
											Log(pLogFile,"自己继续出牌:");
											pkCardAi->dispalyCardInfo(CardSlectInfo,pLogFile);
											//整理一下自己的牌面
											pkCardAi->takeAwayCard(CardUserInfo,CardSlectInfo);
											pkCardAi->sortCard(CardUserInfo);
											Log(pLogFile,"自己手中剩余的牌:");
											pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);
										}
									}
									else //真正出牌的不是自己 有可能是 上家 也有可能是下家 不管是那一家 我都要压牌
									{
										iLastAiCardNum -= iCardOutNum; //得到上家手中的牌
										if (iLastAiCardNum <= 0)                         //上家已经出完了牌  发送结束一局协议 也可能会发送请求人家牌面协议 再看
										{
										
											mtServiceRoomRoundOver kServiceRoomRoundOver;
											kServiceRoomRoundOver.init();
											if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
											{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
											}
											Log(pLogFile, "上家剩余牌为0，自己发送结束一局协议过去\n");
										}
										else
										{
											//Log(pLogFile, "上家出的牌\n");
											eCardType = pkCardAi->getCardOut(CardUserInfo,CardInDeskInfo,eCardType,CardSlectInfo); //压牌
											mtServiceRoomCardOut kServiceRoomCardOut;
											if (eCardType == MT_FL_CARD_TYPE_PASS) //就是要不起 发送数据 只关注出牌类型  后面的牌忽略 现在随便传一个参数
											{
												char *pCardOut = pkCardOutDataRead->pcCard;  //得到桌子上刚刚出的牌
												Log(pLogFile, "自己发送出牌协议，内容：过\n");
												kServiceRoomCardOut.init(MT_FL_CARD_TYPE_PASS,pCardOut); //*********
												if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
												{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
												}
											}
											else                                   //可以压死牌 发送协议过去
											{
											
												pkCardAi->cardConvert(CardSlectInfo,CardSlect);//转换成服务端的牌
												kServiceRoomCardOut.init(eCardType,CardSlect); 
												Sleep(SLEEP_CARD_OUT_TIME); //休息一会
												if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
												{
													SaftyRelease(pkNetWork);
													SaftyRelease(pkCardAi);
													if (clientSocket != SOCKET_ERROR)
													{
														closesocket(clientSocket);
													}
													Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
													return 0;
												}
												//整理一下自己的牌面
												Log(pLogFile,"自己发送出牌协议,内容:");
												pkCardAi->dispalyCardInfo(CardSlectInfo,pLogFile);
												pkCardAi->takeAwayCard(CardUserInfo,CardSlectInfo);
												pkCardAi->sortCard(CardUserInfo);
												Log(pLogFile, "自己剩余牌是:");
												pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);															
											}
										}							
									}						
								}
								else if (lUserId == lUserAiId)                            //自己出的牌
								{
									
									//判断一下 自己手中的牌  
									if (!pkCardAi->getCardNum(CardUserInfo))  //手中没有牌了
									{
										//发送 结束一局的协议过去
									    
										mtServiceRoomRoundOver kServiceRoomRoundOver;
										kServiceRoomRoundOver.init();
										if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
										{
											SaftyRelease(pkNetWork);
											SaftyRelease(pkCardAi);
											if (clientSocket != SOCKET_ERROR)
											{
												closesocket(clientSocket);
											}
											Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
											return 0;
										}
										Log(pLogFile, "自己手中剩余牌为0，发送结束一局协议过去\n");
									}					
								}
							}
							break;
						case E_SERVICE_ROOM_ROUND_OVER:                                //接到打完了一局牌协议  发送退出房间协议
							{
								Log(pLogFile, "接收结束一局协议\n");
								Log(pLogFile,"\n*******第%d轮第%d局结束,收到结束一局协议,上家id:%d,下家id:%d,桌子id:%x*******\n",iCompleteLunShu,iCompleteRound + 1,lLastAiId,lNextAiId,lDeskId);							
								bStartFlag = false;                                      //开始标志 置 false
								iCompleteRound++;                                       //完成局数 加1
								if (iCompleteRound == iRoundTotal)                      //达到局数的要求  才退出房间
								{
									//直接发送退出大厅协议 
									mtServiceRoomExit kServiceRoomExit;
									kServiceRoomExit.init();
									if (!(pkNetWork->WriteData((char*)(&kServiceRoomExit.m_kExitDataWrite),sizeof(kServiceRoomExit.m_kExitDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile, "发送退出协议\n");
								}
								else
								{
									iLastAiCardNum = 17;
									iNextAiCardNum = 17;
									lRealOutCardPlay = 0;
									for (int i = 0; i<24; i++) 
									{
										CardInDeskInfo[i].cCardType = 0;
										CardInDeskInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
									}
									//发送开始协议
									mtServiceRoomWaitStart kServiceRoomWaitStart;
									kServiceRoomWaitStart.init();
									Sleep(SLEEP_CALL_START_TIME);                   //测试用
									if(!(pkNetWork->WriteData((char*)(&kServiceRoomWaitStart.m_kWaitStartDataWrite),sizeof(kServiceRoomWaitStart.m_kWaitStartDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									bStartFlag = true;
									Log(pLogFile, "发送结束一局的重新开始协议\n");
								}					
							}
							break;
						case E_SERVICE_ROOM_EXIT:                                           //退出房间协议
							{
								SYSTEMTIME curTime;
								GetLocalTime(&curTime);

								mtServiceRoomExit::DataRead *pkExitDataRead = (mtServiceRoomExit::DataRead*)lpCurPos;
								if (lUserAiId == pkExitDataRead->lUserId)//是自己的退出协议
								{
									Log(pLogFile, "接到自己退出房间协议，准备重新连接  %d:%d:%d\n",curTime.wHour,curTime.wMinute,curTime.wSecond);
									closesocket(clientSocket);
									Sleep(1000*3);//暂停三秒 再重连        						
									goto step1;
						
								}
								else  //别人退出房间了  
								{
								
									Log(pLogFile, "接到玩家：%d退出房间协议,我不做处理  %d:%d:%d\n",pkExitDataRead->lUserId,curTime.wHour,curTime.wMinute,curTime.wSecond);
								}
							}
							break;
						default:
							Log(pLogFile, "别的协议:%d\n",lpCurPos[1]);
							break;
						}

						break;
					}
					else
					{
						//调用接收协议处理函数
						switch (lpCurPos[1]) //得到协议的类型
						{
						case E_SERVICE_ROOM_CONNECT:                         //连接
							{
								Log(pLogFile, "接到连接协议\n");
								mtServiceRoomConnect::DataRead *pkConnectDataRead = (mtServiceRoomConnect::DataRead*)(lpCurPos);
								iResult = pkConnectDataRead->lResult;
								if (iResult == 1)  //连接成功了
								{
									Log(pLogFile,"连接成功\n");
								}
								else       
								{
									Log(pLogFile, "连接异常返回:%d\n",iResult);
									SaftyRelease(pkNetWork);
									SaftyRelease(pkCardAi);
									if (clientSocket != SOCKET_ERROR)
									{
										closesocket(clientSocket);
									}
									Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
									return 0;
								}					
							}
							break;
						case E_SERVICE_ROOM_DESK_TWO:          //组建双人桌子
							{
								Log(pLogFile, "接到组建好双人桌子协议\n");
								if (!bStartFlag)
								{
									mtServiceRoomWaitStart kServiceRoomWaitStart;
									kServiceRoomWaitStart.init();
									if(!(pkNetWork->WriteData((char*)(&kServiceRoomWaitStart.m_kWaitStartDataWrite),sizeof(kServiceRoomWaitStart.m_kWaitStartDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile, "发送组建好双人桌子开始协议\n");
									bStartFlag = true;
								}								
							}
							break;
						case E_SERVICE_ROOM_DESK_THREE:       //组建三人桌子 
							{
								Log(pLogFile,"接到组建好三人桌子协议\n");
								mtServiceRoomDeskThree::DataRead *pkDeskThreeDataRead = (mtServiceRoomDeskThree::DataRead*)(lpCurPos);
								lLastAiId = pkDeskThreeDataRead->kUserInfo[0].lUserId;  //得到上家id
								lNextAiId = pkDeskThreeDataRead->kUserInfo[1].lUserId;  //得到下家id
								lDeskId   = pkDeskThreeDataRead->kUserInfo[0].ulDeskId; //得到桌子id
								Log(pLogFile,"\n*******第%d轮第%d局开始,收到三人桌子建好协议,上家id:%d,下家id:%d,桌子id:%x*******\n",iCompleteLunShu,iCompleteRound + 1,lLastAiId,lNextAiId,lDeskId);							
								if (!bStartFlag)
								{
									mtServiceRoomWaitStart kServiceRoomWaitStart;
									kServiceRoomWaitStart.init();
									if(!(pkNetWork->WriteData((char*)(&kServiceRoomWaitStart.m_kWaitStartDataWrite),sizeof(kServiceRoomWaitStart.m_kWaitStartDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile, "发送组建好三人桌子开始协议\n");
									bStartFlag = true;
								}
							}
							break;
						case E_SERVICE_ROOM_WAIT_START:       //开始协议    
							{
								mtServiceRoomWaitStart::DataRead *pkWaitStartDataRead = (mtServiceRoomWaitStart::DataRead*)(lpCurPos);
								Log(pLogFile, "接到开始协议%d\n",pkWaitStartDataRead->lUserId);
								 //要清理一下桌面当前的牌信息  因为重新开始 会到这里来 在这里重置一下数据
								iLastAiCardNum = 17;
								iNextAiCardNum = 17;
								lRealOutCardPlay = 0;
								for (int i = 0; i<24; i++) 
								{
									CardInDeskInfo[i].cCardType = 0;
									CardInDeskInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
								}
							}
							break;
						case E_SERVICE_ROOM_CARD_DEAL:         //牌过来了 接到牌 
							{
								Log(pLogFile, "接到洗牌发牌协议\n");
								mtServiceRoomCardDealRecv::DataRead *pkCardDealRecvDataRead = (mtServiceRoomCardDealRecv::DataRead*)(lpCurPos);
								// 先存好牌
								for (int i = 0; i<24; i++)
								{
									CardUserInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
									CardUserInfo[i].cCardType = 0;
									CardUserCard[i] = MTFL_NUM_CARD_TERMINAL;
									if (i<7)
									{
										CardBaseCard[i] = MTFL_NUM_CARD_TERMINAL;
									}
								}
								memcpy(CardUserCard,pkCardDealRecvDataRead->pcCards,sizeof(char)*17);
								memcpy(CardBaseCard,pkCardDealRecvDataRead->pcBaseCards,sizeof(char)*7);
								pkCardAi->cardConvert(CardUserCard,CardUserInfo);
								pkCardAi->sortCard(CardUserInfo);
								Log(pLogFile, "自己的牌是：\n");
								pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);
								if( pkCardDealRecvDataRead->lDefaultLandlordUserId == lUserAiId )  // 自己被选中抢地主
								{
									//发送一个抢地主的协议
									
									Log(pLogFile, "自己被选中抢地主\n");
									mtServiceRoomGrabLandlord kServiceRoomGrabLandlord;
									kServiceRoomGrabLandlord.init();
									if (!(pkNetWork->WriteData((char*)&(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite),sizeof(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile, "发送抢地主协议 3分\n");
								}
								else                                                         //默认抢地主的不是自己
								{
									Log(pLogFile, "等待抢地主\n");
								}

							}
							break;
						case E_SERVICE_ROOM_GRAB_LANDLORD:                                   //抢地主协议
							{
								Log(pLogFile,"接到抢地主协议\n");
								mtServiceRoomGrabLandlord::DataRead *pkGrabLandLordDataRead = (mtServiceRoomGrabLandlord::DataRead*)(lpCurPos);
								if (pkGrabLandLordDataRead->lUserIdLandlord > 0)   //已经有地主了
								{
									Log(pLogFile,"地主产生\n");
									
									if ( pkGrabLandLordDataRead->lUserIdLandlord == lUserAiId )     // 自己抢到了地主 底牌要加进自己的牌面里面 还要翻译一下                                                    
									{
										//发送出牌协议
										Log(pLogFile,"自己是地主\n");
										for (int i = 0; i<24; i++)
										{
											CardUserInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
											CardUserInfo[i].cCardType = 0;
											CardSlectInfo[i].cCardType = 0;
											CardSlectInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
											CardSlect[i] = MTFL_NUM_CARD_TERMINAL;
											if (i<7)
											{
												CardBaseInfo[i].cCardType = 0;
												CardBaseInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
											}
										}
										pkCardAi->cardConvert(CardUserCard,CardUserInfo);     //服务器的牌 转换为客户端的牌
										pkCardAi->cardConvert(CardBaseCard,CardBaseInfo);
										pkCardAi->addInCard(CardUserInfo,CardBaseInfo);      //添加 底牌  所有的只要保证了 后面试以 MTFL_NUM_CARD_TERMINAL 结束 就安全的
										pkCardAi->sortCard(CardUserInfo);                    //排列一下
										//执行第一次出牌操作 发出牌协议过去
										pkCardAi->getCardOut(CardUserInfo,NULL,MT_FL_CARD_TYPE_BEG,CardSlectInfo);//得到出牌的内容
										mteCardType eCardType = pkCardAi->getTypeFromCard(CardSlectInfo);          //得到出牌的类型
										pkCardAi->cardConvert(CardSlectInfo,CardSlect); //出的牌转换一下 用来发送到服务端
										mtServiceRoomCardOut kServiceRoomCardOut;
										kServiceRoomCardOut.init(eCardType,CardSlect);
										Sleep(SLEEP_CARD_OUT_TIME);
										if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
										{
											SaftyRelease(pkNetWork);
											SaftyRelease(pkCardAi);
											if (clientSocket != SOCKET_ERROR)
											{
												closesocket(clientSocket);
											}
											Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
											return 0;
										}
										Log(pLogFile, "发送出牌协议，内容：");
										pkCardAi->dispalyCardInfo(CardSlectInfo,pLogFile);
										//整理一下自己的牌面
										pkCardAi->takeAwayCard(CardUserInfo,CardSlectInfo);
										pkCardAi->sortCard(CardUserInfo);
										Log(pLogFile, "自己剩余的牌：");
										pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);
									}
									else if(pkGrabLandLordDataRead->lUserIdLandlord == lLastAiId)
									{
										Log(pLogFile, "地主是上家\n");
										iLastAiCardNum = 20;
									}
									else if(pkGrabLandLordDataRead->lUserIdLandlord == lNextAiId)
									{
										Log(pLogFile, "地主是下家\n");
										iNextAiCardNum = 20;
									}
									else
									{
										cout<<"****"<<endl;
									}
								}
								if (pkGrabLandLordDataRead->lUserIdLandlord == -1)  //还没有产生地主
								{
									//Log(pLogFile, "地主还没有产生\n");
									if (pkGrabLandLordDataRead->lUserId == lLastAiId) //是上一家叫的地主
									{
										//自己抢地主
										//Log(pLogFile,"上家抢地主,分数:%d\n",pkGrabLandLordDataRead->lScore);
										mtServiceRoomGrabLandlord kServiceRoomGrabLandlord;
										kServiceRoomGrabLandlord.init();
										if (!(pkNetWork->WriteData((char*)&(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite),sizeof(kServiceRoomGrabLandlord.m_kGrablandlordDataWrite))))
										{
											SaftyRelease(pkNetWork);
											SaftyRelease(pkCardAi);
											if (clientSocket != SOCKET_ERROR)
											{
												closesocket(clientSocket);
											}
											Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
											return 0;
										}
										Log(pLogFile, "发送抢地主协议 3分\n");
									}
									else
									{
										//cout<<"下家抢地主，不管"<<endl;
									}
						
								}
							}
							break;
						case E_SERVICE_ROOM_CARD_OUT:                                     //出牌协议 
							{
								
								mtServiceRoomCardOut::DataRead *pkCardOutDataRead = (mtServiceRoomCardOut::DataRead*)(lpCurPos);
								mteCardType eCardType = (mteCardType)pkCardOutDataRead->lCardType; //得到出牌的类型
								long lUserId = pkCardOutDataRead->lUserId; //得到出牌人的id（过牌的也算）
								int iCardOutNum = 0;                 //当前出牌的张数
								for (int i = 0; i<24; i++)
								{
									CardSlectInfo[i].cCardType = 0;
									CardSlectInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
									CardSlect[i] = MTFL_NUM_CARD_TERMINAL;
								}
								if (eCardType != MT_FL_CARD_TYPE_PASS) //如果不是过牌  就要更新一下桌子当前的牌信息 还有真实的出牌人（出了实牌的人）
								{
									char *pCardOut = pkCardOutDataRead->pcCard;  //得到桌子上刚刚出的牌
									for (int i = 0; i<24; i++)
									{
										CardInDeskInfo[i].cCardType = 0;
										CardInDeskInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;	
									}
									pkCardAi->cardConvert(pCardOut,CardInDeskInfo);//转换成客户端的牌类型 放CardInDeskInfo里
									pkCardAi->sortCard(CardInDeskInfo);
									Log(pLogFile,"接收出牌协议，出牌人：%d,内容：",lUserId);
									pkCardAi->dispalyCardInfo(CardInDeskInfo,pLogFile);
									lRealOutCardPlay = pkCardOutDataRead->lUserId;//更新一下 真正的出牌玩家 过的不算 
									iCardOutNum = pkCardAi->getCardNum(CardInDeskInfo); //得到出牌的张数
								}
								else                                                                //过牌
								{
									//让CardInDeskInfo 保存上一次桌面的牌
									Log(pLogFile,"接收出牌协议，出牌人：%d,内容:过\n",lUserId);


								}
								Log(pLogFile,"桌子当前牌面：");
								pkCardAi->dispalyCardInfo(CardInDeskInfo,pLogFile);

								if (lUserId == lNextAiId)                                //下家出的牌  有可能要发送完成一局协议过去
								{
									iNextAiCardNum -= iCardOutNum;
									if (iNextAiCardNum <= 0)                         //下家已经出完了牌  发送结束一局协议 也可能会发送请求人家牌面协议 再看
									{
										
										Log(pLogFile,"下家手中的牌已经打完\n");
										mtServiceRoomRoundOver kServiceRoomRoundOver;
										kServiceRoomRoundOver.init();
										if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
										{
											SaftyRelease(pkNetWork);
											SaftyRelease(pkCardAi);
											if (clientSocket != SOCKET_ERROR)
											{
												closesocket(clientSocket);
											}
											Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
											return 0;
										}
										Log(pLogFile, "自己发送结束一局协议\n");
									}
									else
									{
										
										//Log(pLogFile, "是下家出的牌，不要\n");
									}
								}
								else if (lUserId == lLastAiId)                            //上家出的牌 那要压牌
								{
									if (lUserAiId == lRealOutCardPlay)           //上次真正出牌的是自己
									{
										//判断一下自己手中的牌
										if (!pkCardAi->getCardNum(CardUserInfo))  //手中没有牌了
										{
											//发送 结束一局的协议过去
											
											Log(pLogFile, ",自己手中的牌打完\n");
											mtServiceRoomRoundOver kServiceRoomRoundOver;
											kServiceRoomRoundOver.init();
											if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
											{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
											}
											Log(pLogFile, "发送结束一局协议\n");

										}
										else //还有牌 继续出牌
										{
											
											//执行第一次出牌操作 发出牌协议过去
											pkCardAi->getCardOut(CardUserInfo,NULL,MT_FL_CARD_TYPE_BEG,CardSlectInfo);//得到出牌的内容
											mteCardType eCardType = pkCardAi->getTypeFromCard(CardSlectInfo);          //得到出牌的类型
											pkCardAi->cardConvert(CardSlectInfo,CardSlect); //出的牌转换一下 用来发送到服务端
											mtServiceRoomCardOut kServiceRoomCardOut;
											kServiceRoomCardOut.init(eCardType,CardSlect);
											Sleep(SLEEP_CARD_OUT_TIME); //休息一会
											if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
											{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
											}
											Log(pLogFile, "自己发送出牌协议,内容：");
											pkCardAi->dispalyCardInfo(CardSlectInfo,pLogFile);
											//整理一下自己的牌面
											pkCardAi->takeAwayCard(CardUserInfo,CardSlectInfo);
											pkCardAi->sortCard(CardUserInfo);
											Log(pLogFile, "自己手中的牌,内容：");
											pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);
										}
									}
									else //真正出牌的不是自己 有可能是 上家 也有可能是下家 不管是那一家 我都要压牌
									{
										iLastAiCardNum -= iCardOutNum; //得到上家手中的牌
										if (iLastAiCardNum <= 0)                         //上家已经出完了牌  发送结束一局协议 也可能会发送请求人家牌面协议 再看
										{
											Log(pLogFile, "上家手中的牌已经打完\n");
											mtServiceRoomRoundOver kServiceRoomRoundOver;
											kServiceRoomRoundOver.init();
											if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
											{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
											}
											Log(pLogFile, "自己发送结束一局协议过去\n");
										}
										else
										{
											
											eCardType = pkCardAi->getCardOut(CardUserInfo,CardInDeskInfo,eCardType,CardSlectInfo); //压牌
											mtServiceRoomCardOut kServiceRoomCardOut;
											if (eCardType == MT_FL_CARD_TYPE_PASS) //就是要不起 发送数据 只关注出牌类型  后面的牌忽略 现在随便传一个参数
											{
												char *pCardOut = pkCardOutDataRead->pcCard;  //得到桌子上刚刚出的牌
												
												Log(pLogFile, "自己发送出牌协议,内容：过\n");
												kServiceRoomCardOut.init(MT_FL_CARD_TYPE_PASS,pCardOut); //*********
												if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
												{
												SaftyRelease(pkNetWork);
												SaftyRelease(pkCardAi);
												if (clientSocket != SOCKET_ERROR)
												{
													closesocket(clientSocket);
												}
												Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
												return 0;
												}
											}
											else                                   //可以压死牌 发送协议过去
											{
												
												pkCardAi->cardConvert(CardSlectInfo,CardSlect);//转换成服务端的牌
												kServiceRoomCardOut.init(eCardType,CardSlect); 
												Sleep(SLEEP_CARD_OUT_TIME); //休息一会
												if (!(pkNetWork->WriteData((char*)(&kServiceRoomCardOut.m_kCardOutDataWrite),sizeof(kServiceRoomCardOut.m_kCardOutDataWrite))))
												{
													SaftyRelease(pkNetWork);
													SaftyRelease(pkCardAi);
													if (clientSocket != SOCKET_ERROR)
													{
														closesocket(clientSocket);
													}
													Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
													return 0;
												}
												Log(pLogFile, "自己发送出牌协议，内容：");
											
												
												pkCardAi->dispalyCardInfo(CardSlectInfo,pLogFile);
												pkCardAi->takeAwayCard(CardUserInfo,CardSlectInfo);
												pkCardAi->sortCard(CardUserInfo);
												Log(pLogFile, "自己剩余牌面是，内容：");
												pkCardAi->dispalyCardInfo(CardUserInfo,pLogFile);															
											}
										}							
									}						
								}
								else if (lUserId == lUserAiId)                            //自己出的牌
								{
									
									//判断一下 自己手中的牌  
									if (!pkCardAi->getCardNum(CardUserInfo))  //手中没有牌了
									{
										//发送 结束一局的协议过去
										mtServiceRoomRoundOver kServiceRoomRoundOver;
										kServiceRoomRoundOver.init();
										if (!(pkNetWork->WriteData((char*)(&kServiceRoomRoundOver.m_kRoundOverDataWrite),sizeof(kServiceRoomRoundOver.m_kRoundOverDataWrite))))
										{
											SaftyRelease(pkNetWork);
											SaftyRelease(pkCardAi);
											if (clientSocket != SOCKET_ERROR)
											{
												closesocket(clientSocket);
											}
											Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
											return 0;
										}
										Log(pLogFile, "自己手中牌打完，发送结束一局协议\n");
									}					
								}
							}
							break;
						case E_SERVICE_ROOM_ROUND_OVER:                                //接到打完了一局牌协议  发送退出房间协议
							{
								Log(pLogFile, "接收结束一局协议\n");
								Log(pLogFile,"\n*******第%d轮第%d局结束,收到结束一局协议,上家id:%d,下家id:%d,桌子id:%x*******\n",iCompleteLunShu,iCompleteRound + 1,lLastAiId,lNextAiId,lDeskId);							
								bStartFlag = false;                                      //开始标志 置 false
								iCompleteRound++;                                       //完成局数 加1
								if (iCompleteRound == iRoundTotal)                      //达到局数的要求  才退出房间
								{
									//直接发送退出房间协议 
									mtServiceRoomExit kServiceRoomExit;
									kServiceRoomExit.init();
									if (!(pkNetWork->WriteData((char*)(&kServiceRoomExit.m_kExitDataWrite),sizeof(kServiceRoomExit.m_kExitDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									Log(pLogFile, "发送退出房间协议\n");
								}
								else
								{
									iLastAiCardNum = 17;
									iNextAiCardNum = 17;
									lRealOutCardPlay = 0;
									for (int i = 0; i<24; i++) 
									{
										CardInDeskInfo[i].cCardType = 0;
										CardInDeskInfo[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
									}
									//发送开始协议
									mtServiceRoomWaitStart kServiceRoomWaitStart;
									kServiceRoomWaitStart.init();
									Sleep(SLEEP_CALL_START_TIME);
									if(!(pkNetWork->WriteData((char*)(&kServiceRoomWaitStart.m_kWaitStartDataWrite),sizeof(kServiceRoomWaitStart.m_kWaitStartDataWrite))))
									{
										SaftyRelease(pkNetWork);
										SaftyRelease(pkCardAi);
										if (clientSocket != SOCKET_ERROR)
										{
											closesocket(clientSocket);
										}
										Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
										return 0;
									}
									bStartFlag = true;
									Log(pLogFile,"发送结束一局的重新开始的协议\n");
								}					
							}
							break;
						case E_SERVICE_ROOM_EXIT:                                           //退出房间协议
							{
								mtServiceRoomExit::DataRead *pkExitDataRead = (mtServiceRoomExit::DataRead*)lpCurPos;
								if (lUserAiId == pkExitDataRead->lUserId)//是自己的退出协议
								{
									Log(pLogFile,"接到自己退出房间协议，准备重连房间\n");
									closesocket(clientSocket);
									Sleep(1000 * 3);//暂停三秒 再重连        						
									goto step1;
						
								}
								else  //
								{
									Log(pLogFile,"接到玩家:%d退出房间协议\n",pkExitDataRead->lUserId);
								}
							}
							break;
						default:
							Log(pLogFile, "别的协议%d\n",lpCurPos[1]);
							break;
						}
						lpCurPos = (long*)(buffer + lDataSize);
						lDataSize += lpCurPos[0];
					}
				}
			}
			else if (iReturn == E_ERROR_SELECT_NONE)
			{
				//不管
				
			}
			else
			{
				Log(pLogFile, "接收数据错误\n");
				SaftyRelease(pkNetWork);
				SaftyRelease(pkCardAi);
				if (clientSocket != SOCKET_ERROR)
				{
					closesocket(clientSocket);
				}
				Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
				return 0;
			}
	    }
	}
	SaftyRelease(pkNetWork);
	SaftyRelease(pkCardAi);
	if (clientSocket != SOCKET_ERROR)
	{
		closesocket(clientSocket);
	}
	Log(pLogFile,"线程退出 行数:%d\n",__LINE__);
	return 0;

}

void SaftyRelease(void *p)  //释放指针 
{
	if (p == NULL)
	{
		return ;
	}
	else
	{
		delete p;
		if (p != NULL)
		{
			p = NULL;
		}
	}
}

bool initNetModule()
{
	//初始化网络模块
	WSADATA  wsadata;
	if( WSAStartup(MAKEWORD(2,2),&wsadata) != 0 )
	{
		cout<<"Init Windows Socket Fail"<<endl;
		return false;
	}
	//填充服务器地址 
	memset(&serverAddress,0,sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(IP);   
	serverAddress.sin_port = htons(PORT);
	return true;
}

void  initThreadModule()
{
	HANDLE hThreadWork[THREAD_NUMBER];
	for(int i = 0; i<THREAD_NUMBER; i++)
	{
		hThreadWork[i] = CreateThread(0,0,ThreadWorkFun,NULL,0,0);
		Sleep(500);
		CloseHandle(hThreadWork[i]);
	}
}

void Log( FILE* pFiLE,char * pcformat, ... )  //日志函数
{
	va_list kArgs;
	int     iLen;
	char    pcBuffer[1024];

	memset(pcBuffer, 0, sizeof(pcBuffer));
	va_start(kArgs, pcformat);
	iLen = _vscprintf(pcformat, kArgs) ;
	vsprintf(pcBuffer, pcformat, kArgs);

	if (NULL == pFiLE)
	{
		return ;

    }
	fwrite(pcBuffer, iLen, 1, pFiLE);
	fflush(pFiLE);
	va_end(kArgs);
	printf(pcBuffer);
}

bool GetRunExeDir(char* pDir, DWORD dwLen)  //得到当前运行程序的路径保存到参数指针里面
{
	GetModuleFileName(NULL, pDir, dwLen);//返回当前应用程序全路径，此为api函数
	bool flag = NULL;
	int iLen = strlen(pDir);
	if (0 >= iLen)
	{
		return FALSE;
	}

	//取出文件路径  
	for(int i = iLen; i >= 0; i--)
	{
		if(pDir[i] == '\\')
		{   
			pDir[i] = '\0';
			break;
		} 
	}

	return TRUE;
}

int WSDCreateDirectory(const char* pcDir)    //新建目录 参数为全路径 其实就是新建一个文件夹
{
	char pcDirTmp[256];
	strcpy(pcDirTmp, pcDir);

	char* pcOne = strchr(pcDirTmp, '\\');    
	while(pcOne)
	{
		*pcOne = '\0';
		_mkdir(pcDirTmp);
		*pcOne = '\\';
		pcOne = strchr(pcOne + 1, '\\');
	}

	_mkdir(pcDirTmp);

	return 1;
}
								
int ReadData(SOCKET socket, char *pcBuffer, long bufferSize)
{
	timeval		kTimeVal;  
	kTimeVal.tv_sec = 0;
	kTimeVal.tv_usec = 1000*10; //等待十毫秒
	fd_set		m_kFdSet;
	int 		iRetRead	= 0;
	FD_ZERO(&m_kFdSet);
	FD_SET(socket, &m_kFdSet);
	int 		iRetSelect		= select (socket+1, &m_kFdSet, NULL, NULL, &kTimeVal);
	if (1 == iRetSelect)   //
	{
		iRetRead	= recv(socket, pcBuffer, bufferSize,0);
		if (iRetRead > (sizeof(long)<<2))  //接到的数据大于 16个字节
		{
			return 	iRetRead;  //返回读到的字节数
		}
		
	}
	if (0 == iRetSelect)   //没有可读的套接字
	{
		return 	E_ERROR_SELECT_NONE;
	}
	else if(1 == iRetSelect && 0 == iRetRead)  //读到数据为0
	{
		return E_ERROR_NETWORK_CLOSE;
	}
	else if (1 == iRetSelect && -1 == iRetRead)
	{
		return 	E_ERROR_READ;
	} 
	else if (1 == iRetSelect) 
	{
		return 	E_ERROR_READ_BYTES;
	} 
	else 
	{
		return 	E_ERROR_SELECT;
	}


}
