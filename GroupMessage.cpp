#include <CQEVE.h>
#include "CQEVE_ALL.h"
#include <CQLogger.h>
#include <CQAPI_EX.h>
//#include "APPINFO.h"
#include "CQTools.h"
#include "CQMsgCode.h"
#include <CQEVE_PrivateMsg.h>
#include <CQEVE_GroupMsg.h>
#include <CQcode.h>

#include <fstream>
#include <ostream>
#include <ctime>
#include <queue>
#include <list>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include "parrotKiller.h"
#include "administration.h"
#include "Levenshtein.h"
using namespace std;
using namespace CQ;
static Logger logger("GroupMsg");

extern map< long long, ParrotGroup> parrots;


EVE_GroupMsg_EX(GroupMsg)
{
	if (eve.isSystem()) //忽略系统消息
		return;

	int msgCount = 0;

	//忽略前导的空格
	while (eve.message[msgCount] == ' ')
		msgCount++;

	//判断是否为设置类指令	
	if (_stricmp(eve.message.substr(msgCount, strlen(KeyWord1)).c_str(), KeyWord1) == 0)
	{
		//GroupMemberInfo getGroupMemberInfo(长整数型 群号, 长整数型 QQID, 逻辑型 不使用缓存 = false);		
		if (getGroupMemberInfo(eve.fromGroup, getLoginQQ()).permissions >= 2)
		{
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2)
			{
				msgCount += strlen(KeyWord1);
				if (eve.message[msgCount] == ' ')
					msgCount++; //忽略后面的空格

				if (_stricmp(eve.message.substr(msgCount, strlen(KillerOn)).c_str(), KillerOn) == 0)
				{
					//开启插件
					msgCount += strlen(KillerOn);
					if (eve.message[msgCount] == ' ')
						msgCount++; //忽略后面的空格

					//打开禁言功能
					banMode readBanMode;
					int readBanTime = DefaultBanTime;
					string apply = eve.message.substr(msgCount);
					if (apply.find('X') != string::npos)
					{
						readBanMode = ex;
					}
					else
					{
						readBanTime = atoi(apply.c_str());
						/*int newtime = atoi(apply.c_str());
						if (newtime <= 0)
						{
							if (newtime < 0 || newtime> MaxBanTime)
								sendGroupMsg(eve.fromGroup, BanTimeError);
							newtime = DefaultBanTime;
						}
						readBanTime = newtime;*/						
						readBanMode = cnst;
					}

					//如果当前群不在map中，加进去
					if (!parrots.count(eve.fromGroup))
					{
						ParrotGroup tmpNew(eve.fromGroup);
						parrots.insert(make_pair(eve.fromGroup, tmpNew));
					}

					//sendGroupMsg(eve.fromGroup, to_string(readBanMode).c_str()); //for test
					//修改禁言类型和时长
					if (parrots[eve.fromGroup].turnOn(readBanMode, readBanTime) == 1)
					{
						string prompt = AlreadyOpen;
						if (parrots[eve.fromGroup].getBanMode() == cnst)
						{
							if (readBanTime <= 0)
								readBanTime = DefaultBanTime;
							else if (readBanTime > MaxBanTime)
								readBanTime = MaxBanTime;
							prompt += seconds_to_date(readBanTime);
						}							
						else
							prompt += ExponentialGrowth;
						sendGroupMsg(eve.fromGroup, prompt);
						//sendGroupMsg(eve.fromGroup, parrots[eve.fromGroup].getRepeatingMsg().c_str()); //for test
						//sendGroupMsg(eve.fromGroup, to_string(parrots[eve.fromGroup].getCounter()).c_str()); //for test
					}
					else
						sendGroupMsg(eve.fromGroup, TurnOnPrompt);
					return;
				}
				else if (_stricmp(eve.message.substr(msgCount, strlen(KillerOff)).c_str(), KillerOff) == 0)
				{
					if (parrots[eve.fromGroup].getEnabled() == true)
					{
						int tmpMsg; //for test
						tmpMsg = parrots[eve.fromGroup].turnOff(); //for test
						sendGroupMsg(eve.fromGroup, TurnOffPrompt);
						//sendGroupMsg(eve.fromGroup, to_string(tmpMsg).c_str()); //for test
					}
					else
					{
						sendGroupMsg(eve.fromGroup, AlreadyOff);
					}
					return;
				}
				else
				{
					sendGroupMsg(eve.fromGroup, InstError);
					return;
				}

			}
			else
			{
				//如果有群员妄图开关插件，禁言处理
				/*int tmpBanTime;
				if (parrots[eve.fromGroup].getBanMode() == cnst)
					tmpBanTime = parrots[eve.fromGroup].getBanTime();
				else
					tmpBanTime = DefaultBanTime * 2;
				sendGroupMsg(eve.fromGroup, PermissionError);
				setGroupBan(eve.fromGroup, eve.fromQQ, tmpBanTime);*/
				//string tmpprmpt = PermissionError; //for test
				//tmpprmpt += " 01";  //for test
				sendGroupMsg(eve.fromGroup, PermissionError);
				setGroupBan(eve.fromGroup, eve.fromQQ, DefaultBanTime * 2);
				return;
			}
		}
		else
		{
			sendGroupMsg(eve.fromGroup, NoAdministratorError);
			return;
		}
	}
	else if (_stricmp(eve.message.substr(msgCount, strlen(KeyWordHelp)).c_str(), KeyWordHelp) == 0)
	{
		string helpMsg = HelpInfo;
		helpMsg += Propanganda;
		helpMsg = AuthorInfo + helpMsg;
		sendGroupMsg(eve.fromGroup, helpMsg.c_str());
		return;
	}

	if ( parrots.count(eve.fromGroup) && parrots[eve.fromGroup].getEnabled())
	{
		int newBanTime = parrots[eve.fromGroup].newMessage(eve.message);
		if (newBanTime > 0)
			setGroupBan(eve.fromGroup, eve.fromQQ, newBanTime);
		return;
	}	
}
