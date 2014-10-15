//#include "mtCardAi.h"
//#include<iostream>
//using namespace std;
////#define DEBUG   //这个用来固定测试 专门给机器人固定的牌（自己选择）
//
//int   main()
//{  
//#if 0
//	mtCardInfo pkCardTakeout[21]; //存桌面上刚刚出的牌
//	mtCardInfo pkCardDst[21];  //存选出来的牌 
//	for (int i = 0; i<21; i++)
//	{
//		pkCardTakeout[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//	}
//	for (int i = 0; i<21; i++)
//	{
//		pkCardDst[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//	}
//	mtCardAI* pkCardAI = new mtCardAI;
//	if (!pkCardAI)
//	{
//		delete pkCardAI;
//		return 0;
//	}
//	mtCardAI* pkCardMan = new mtCardAI;  //  我用的  用的是里面的 
//	if (!pkCardMan)
//	{
//		delete pkCardMan;
//		delete pkCardAI;
//		return 0;
//	}
//#ifdef DEBUG
//	mtCardInfo kCardInfoAI[21] = {
//		{0,2},{1,3},{1,3},{3,4},{0,6},
//		{0,6},{1,7},{0,7},{2,8},{3,8},
//		{2,9},{1,9},{2,10},{3,10},{0,11},
//		{2,12},{3,13},
//		{0,MTFL_NUM_CARD_TERMINAL},
//		{0,MTFL_NUM_CARD_TERMINAL},
//		{0,MTFL_NUM_CARD_TERMINAL},
//		{0,MTFL_NUM_CARD_TERMINAL}
//	};                                            //定义且初始化
//#else
//	mtCardInfo kCardInfoAI[21];  //存机器人用的牌
//
//#endif // DEBUG
//
//	while (1)
//	{
//		cout<<"洗牌：1,放弃：0"<<endl;
//		bool bFlag = false;
//		cin>>bFlag;
//		if (bFlag == false)
//		{
//			return 0;
//		}
//		else //洗牌
//		{
//#ifndef DEBUG   //
//			for (int i = 0; i<21; i++)
//			{
//				kCardInfoAI[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//			}
//			pkCardAI->shuffle();  //洗好牌
//			memcpy(kCardInfoAI,pkCardAI->m_pkASetOfCards,17*sizeof(mtCardInfo)); //复制洗好的牌的前面17张牌 给机器人存牌数组
//#endif // DEBUG			
//			pkCardAI->sortCard(kCardInfoAI); //排列一下
//			cout<<"机器人的牌面如下：";
//			pkCardAI->dispalyCardInfo(kCardInfoAI);
//			while (pkCardAI->getCardNum(kCardInfoAI)) //机器人没有牌了
//			{
//				for (int i = 0; i<21; i++)
//				{
//					pkCardTakeout[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//				}
//				for (int i = 0; i<21; i++)
//				{
//					pkCardDst[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//				}
//				cout<<"请选择谁先出牌：1 你，0 机器人"<<endl;
//				int iWhoCardOut = -1;
//				cin>>iWhoCardOut;
//				switch (iWhoCardOut)
//				{
//				case 1:                    //你出牌
//					{
//sign1:				    cout<<"请输入您所有要出牌的下标,最后以100结束："<<endl;
//						for (int i = 0; i<21; i++)
//						{
//							pkCardTakeout[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//						}
//						int a= 0;
//						int i = 0;
//						while(cin>>a)
//						{
//							if (a == 100)
//							{
//								break;
//							}
//							memcpy(&pkCardTakeout[i],pkCardMan->m_pkASetOfCards+a,sizeof(mtCardInfo));
//							i++;
//						}
//						cout<<"我选择的牌是：";
//						pkCardAI->sortCard(pkCardTakeout);
//						pkCardAI->dispalyCardInfo(pkCardTakeout);						
//						if (pkCardAI->judgeCardOut(pkCardTakeout,NULL,MT_FL_CARD_TYPE_BEG))  //选择的牌能够成为类型
//						{
//							mteCardType eCardTypeTakeOut = pkCardAI->getTypeFromCard(pkCardTakeout); //得到出牌的类型
//							cout<<"牌类型为：";
//							cout<<eCardTypeTakeOut<<endl;
//							mteCardType eCardType = pkCardAI->getCardOut(kCardInfoAI,pkCardTakeout,eCardTypeTakeOut,pkCardDst);//得到机器人的牌
//
//							if (eCardType == MT_FL_CARD_TYPE_PASS)
//							{
//								cout<<"机器人打不起,请继续出牌"<<endl;
//								cout<<"机器人剩余的牌是:";
//								pkCardAI->dispalyCardInfo(kCardInfoAI); //再打印出来
//								goto sign1;
//							}
//							else
//							{
//								cout<<"机器人出的牌是:";
//								pkCardAI->dispalyCardInfo(pkCardDst);
//								cout<<"牌类型为：";
//								cout<<eCardType<<endl;
//								pkCardAI->takeAwayCard(kCardInfoAI,pkCardDst);//删除除掉的牌
//								if(!pkCardAI->getCardNum(kCardInfoAI))
//								{
//									continue;
//								}
//								pkCardAI->sortCard(kCardInfoAI); //重新排列好
//								cout<<"机器人剩余的牌是:";
//								pkCardAI->dispalyCardInfo(kCardInfoAI); //再打印出来
//
//							}
//						}
//						else
//						{
//							cout<<"您选择的牌不符合要求,请重新选择下标"<<endl;
//							goto sign1;
//						}						
//					}
//					break;
//				case 0:                   //机器人出牌 不是压牌 
//					{
//sign2:					mteCardType eCardTypeTakeOut = MT_FL_CARD_TYPE_BEG;
//						for (int i = 0; i<21; i++)
//						{
//							pkCardDst[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//						}
//						mteCardType eCardType = pkCardAI->getCardOut(kCardInfoAI,NULL,eCardTypeTakeOut,pkCardDst);//得到机器人的牌
//						cout<<"机器人出的牌是：";
//						pkCardAI->dispalyCardInfo(pkCardDst);
//						cout<<"牌类型是：";
//						cout<<eCardType<<endl;
//						pkCardAI->takeAwayCard(kCardInfoAI,pkCardDst);
//						if(!pkCardAI->getCardNum(kCardInfoAI))
//						{
//							continue;
//						}
//						pkCardAI->sortCard(kCardInfoAI); //重新排列好
//						cout<<"机器人剩余的牌是：";
//						pkCardAI->dispalyCardInfo(kCardInfoAI); //再打印出来
//
//						cout<<"选择是否压牌：1:压牌 ，0:过牌"<<endl;
//						int iIsOutCard = -1;
//						cin>>iIsOutCard;
//						if (iIsOutCard) //压牌
//						{
//sign3:					    cout<<"请输入您所有要出牌的下标,最后以100结束："<<endl;
//							for (int i = 0; i<21; i++)
//							{
//								pkCardTakeout[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
//							}
//							int a= 0;
//							int i = 0;
//							while(cin>>a)
//							{
//								if (a == 100)
//								{
//									break;
//
//								}
//								memcpy(&pkCardTakeout[i],pkCardMan->m_pkASetOfCards+a,sizeof(mtCardInfo));
//								i++;
//							}
//							cout<<"我选择的牌是：";
//							pkCardAI->sortCard(pkCardTakeout);
//							pkCardAI->dispalyCardInfo(pkCardTakeout);
//							if(pkCardAI->judgeCardOut(pkCardTakeout,pkCardDst,eCardType))  //可以压死刚刚打的牌
//							{
//								cout<<"选牌符合要求,我出的牌是："<<endl;
//								pkCardAI->dispalyCardInfo(pkCardTakeout);
//								cout<<"牌类型是：";
//								cout<<pkCardAI->getTypeFromCard(pkCardTakeout)<<endl;
//
//							}
//							else
//							{
//								cout<<"您选择的牌不符合要求,请重新选择下标"<<endl;
//								goto sign3;
//							}
//						}
//						else //过牌
//						{
//							cout<<"你选择过牌,机器人继续出牌"<<endl;
//							goto sign2;
//						}						
//					}
//					break;
//				default:
//					break;
//				}				
//			}
//			cout<<"机器人出完牌了,请重新开一局"<<endl;
//		}
//	}	
//	delete pkCardAI;
//	if (pkCardAI != NULL)
//	{
//		pkCardAI = NULL;
//	}
//	delete pkCardMan;
//	if(pkCardMan != NULL)
//	{
//		pkCardMan = NULL;
//	}
//#endif
//	return 0;
//}
