// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 EventRewardQuickPartyB2.cpp 拆出。
//   ORIG TU     ：RDARScriptQuickParty.cpp
//   ORIG 期望键：_GLOBAL__I__ZN10QuickParty23getRandomBuffTypeByNameERKSs
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <map>
#include <string>
#include "CDataManager.h"             // eventReward / itemData
#include "CQuickPartyRewardManager.h" // namespace QuickParty / QuickPartyData / import 声明
#include "CEnvironment.h"             // less_TCHAR / SpinLock
#include "DNFLexWrapper.h"            // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "StrikerScript.h"            // ContentsType

namespace QuickParty
{

RandomBuffType getRandomBuffTypeByName(const std::string& name)
{
    RandomBuffType type = (RandomBuffType)0xb;
    if      (name == "[increase exp]")       type = RANDOM_BUFF_INCREASE_EXP;
    else if (name == "[increase stats]")     type = RANDOM_BUFF_INCREASE_STATS;
    else if (name == "[increase speed]")     type = RANDOM_BUFF_INCREASE_SPEED;
    else if (name == "[increase dropprob]")  type = RANDOM_BUFF_INCREASE_DROPPROB;
    else if (name == "[increase allattack]") type = RANDOM_BUFF_INCREASE_ALLATTACK;
    else if (name == "[increase allresist]") type = RANDOM_BUFF_INCREASE_ALLRESIST;
    else if (name == "[increase broken]")    type = RANDOM_BUFF_INCREASE_BROKEN;
    else if (name == "[increase mobhp]")     type = RANDOM_BUFF_INCREASE_MOBHP;
    else if (name == "[increase antidevil]") type = RANDOM_BUFF_INCREASE_ANTIDEVIL;
    else if (name == "[increase coinlimit]") type = RANDOM_BUFF_INCREASE_COINLIMIT;
    else if (name == "[return ticket]")      type = RANDOM_BUFF_RETURN_TICKET;
    return type;
}

}  // namespace QuickParty
