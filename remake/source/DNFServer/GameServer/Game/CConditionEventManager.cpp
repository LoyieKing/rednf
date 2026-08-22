// df_game_r Game/ CConditionEventManager 实现（批3：从 GameStubs.cpp 迁移）。
#include "CConditionEventManager.h"

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"

CConditionEventManager::CConditionEventManager()
{
}

// ORIG 0x08335754
// _ZN22CConditionEventManager16GetCurEventIndexEv
// 若当前重复事件 0x71 处于进行中（IsEventing(0) 真）返回 CDataManager +0x4d98
// 的当前条件事件索引；否则返回 0。
int CConditionEventManager::GetCurEventIndex()
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x71);
    if (event->IsEventing((CUser*)0))
        return *(int*)((char*)G_CDataManager() + 0x4d98);
    return 0;
}