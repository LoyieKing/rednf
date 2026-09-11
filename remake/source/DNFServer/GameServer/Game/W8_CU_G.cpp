// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8_CU_G/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "Inven_Item.h"
#include "CDataManager.h"                    // G_CDataManager / getItemList(+0x0c)
#include "GlobalData.h"                      // s_systemTime_/s_capchaDataManager/
                                             // g_autoPunishRuleHackTypeMgr/s_guild_proxy_mgr/s_monitor_proxy_mgr/s_statisticMgr
#include "CStatisticMgr.h"                   // 本 TU 新建权威视图
#include "CGuildServerProxy.h"               // SendCharLevelGrowType
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"                 // GetServerProxy<T> 完整模板
#include "CInventory.h"                      // insertItemIntoInventory / md5_* 声明（md5_context 兼容视图）
#include "GameWorld.h"
#include "TimerQueue.h"                      // TIMER_MESSAGE/InsertTimer/G_TimerQueue
#include "GameTypes.h"                       // cMyTrace
#include "Divide_C_ClassIP_EndIP.h"
#include "CUserCharacInfo.h"                 // ContinuousItemInfo/CSystemTime 等
#include "CSystemTime.h"
#include "CCapchaDataManager.h"              // WongWork::CCapchaDataManager + MSG_UDP_CAPCHA_CERTIFY
#include "CMailBoxHelper.h"                  // ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"         // g_scriptStringManager_/findString(4,key,0)
#include "Arad_DataManager.h"                // ARAD::Singleton<Arad_DataManager>::Get/findGameScript
#include "Arad_EventPeriodDataManager.h"     // isApplied/checkCreateTime（producer 批提供定义）
#include "CUserProc.h"                       // APSystem::CUserProc::ClearActionAndSendtoUser
#include "CLevelDungeonPlayStatistic.h"
#include "CExpertJob.h"                      // expert_job::CExpertJob::IncreaseExpertJobExp(static)
#include "SkillSlot.h"                      // SkillSlot::get_remain_*_at_index
#include "CParty.h"                         // send_to_party（GetParty 完整类型）
#include "W6_HackPunishDeps.h"               // CAutoPunishRuleHackTypeMgr::checkTotalTradeRuleOver

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
// 外部 DB 请求类最小声明区（owner=DB 域批次，定义不在本批）；
// namespace/class 形态与 ORIG mangled 对齐，均 nm 实证。
// ---------------------------------------------------------------------------
// b28d-solo-W8_CU_G：权威声明见 W7LinkDB.cpp:28/44 与 CUserOnlyOrig.cpp:93 ——
// 两者均 **static void**。ORIG 调用点 0x0867fc32/0x0867fcfb 的 (%esp) 是第 1 实参
// （get_acc_id()），非 this 槽 ⇒ 静态调用（P88）；原非静态声明多压临时对象 this，
// 使全部实参右移 4B。
struct DB_AutoPunishedSecondUserData {       // ORIG makeRequest T 0x0843a790 _ZN...makeRequestEjyjj
    static void makeRequest(unsigned int accId, unsigned long long gold,
                            unsigned int uid, unsigned int who);
};
struct DB_InsertAutoPunishSecondUser {       // ORIG makeRequest T 0x0842e34e _ZN...makeRequestEjjyb
    static void makeRequest(unsigned int accId, unsigned int who,
                            unsigned long long gold, bool punishFlag);
};
// 其余五类同 W7LinkDB.cpp:36/50/60/66/72 权威声明：均 static void（原 int 非静态
// 声明使调用点多压临时对象 this + 返回值形态错，P88）。
struct DB_HackIPSave {                       // ORIG makeRequest T 0x08436ff8 _ZN...makeRequestEPKcS1_jj
    static void makeRequest(const char* startIp, const char* rawIp,
                            unsigned int a, unsigned int b);
};
struct DB_InsertAutoPunishFirstUser {        // ORIG makeRequest T 0x0842dd7c _ZN...makeRequestEjPKcjjjjtj
    static void makeRequest(unsigned int accId, const char* ip,
                            unsigned int p1, unsigned int p2, unsigned int p3,
                            unsigned int p4, unsigned short type, unsigned int p6);
};
struct DB_ForceSaveSimpleSSO {               // ORIG makeRequest T 0x084281ae _ZN...makeRequestEji
    static void makeRequest(unsigned int accId, int errCnt);
};
struct DB_CountOfHumanCertify {              // ORIG makeRequest T 0x084283ca _ZN...makeRequestEj
    static void makeRequest(unsigned int accId);
};
struct DB_SaveGiveAvengerTitleFlag {         // ORIG makeRequest T 0x084422dc _ZN...makeRequestEijj
    static void makeRequest(int uid, unsigned int accId, unsigned int characNo);
};


#include "W8_CU_G/stLookOuts.inc"
#include "W8_CU_G/tyIPCounters.inc"

#include "W8_CU_G/getTotalHackUserTradeGold.inc"


#include "W8_CU_G/getTotalHackUserTradeCnt.inc"


#include "W8_CU_G/incTotalHackUserTradeCnt.inc"


#include "W8_CU_G/incTotalHackUserTradeGold.inc"


#include "W8_CU_G/getCharacAntibotSerialNum.inc"


#include "W8_CU_G/gainMoneyFromHackUser.inc"


#include "W8_CU_G/setHackUser.inc"


#include "W8_CU_G/getHashCode.inc"


#include "W8_CU_G/checkHumanCertify.inc"


#include "W8_CU_G/VerifyCleanPadVeriData.inc"


#include "W8_CU_G/reqSendMailCertify.inc"


#include "W8_CU_G/sendSecuRewardItem.inc"


#include "W8_CU_G/SetAccountCargoDBData.inc"


#include "W8_CU_G/VerifyPresentAvengerTitle.inc"


#include "W8_CU_G/rewardExp.inc"


#include "W8_CU_G/gain_exp_sp.inc"


#include "W8_CU_G/getInformNoticeFlag.inc"


#include "W8_CU_G/handleInformNoticeFlag.inc"


#include "W8_CU_G/loadInformNoticeFlag.inc"


#include "W8_CU_G/getInformNoticeFlagIndex.inc"


#include "W8_CU_G/sendInformNoticeFlag.inc"


#include "W8_CU_G/SendEffectItemInfo.inc"


#include "W8_CU_G/SendCoolTimeItemInfo.inc"


#include "W8_CU_G/LoadEffectItemInfo.inc"


#include "W8_CU_G/LoadCoolTimeItemInfo.inc"


