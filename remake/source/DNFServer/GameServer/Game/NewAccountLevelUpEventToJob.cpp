// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NewAccountLevelUpEventToJob/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— NewAccountLevelUpEventToJob（新账号升级转职奖励事件）。
// 全部函数逐指令对照 docs/class_func_reports/NewAccountLevelUpEventToJob/*.md
// 的 ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   C1 0x081b8170 / D1 0x081b818c / D0 0x081b81bc / StartEvent() 0x081b81da /
//   StartEvent(Word_Param) 0x081b81f6 / EndEvent() 0x081b824c /
//   StartAction() 0x081b8246 / IsEventing 0x081b8816（objdump 转写）/
//   processLevelUpToJob 0x081b82a4 / sendNewAccountLevelUpToJobData 0x081b829e
#include "NewAccountLevelUpEventToJob.h"

#include <string.h>

#include "CUser.h"                    // CUser::getCharacLevelUpGift_AccountOnce
#include "CUserCharacInfo.h"          // getCurCharFirstGrowType/get_charac_job/
                                      // get_charac_level/getCurCharacNo
#include "CDataManager.h"             // G_CDataManager/m_levelUpReward/getItemList
#include "Inven_Item.h"               // Inven_Item（m_addInfo）
#include "CItemList.h"                // create_item/ENUM_ITEM_CREATE_TYPE
#include "CInventory.h"               // OS_API::snprintf
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "DB_EventAccountCharacterLevelUpOnceGift.h"
#include "GameTypes.h"                // cMyTrace/LogManager
#include "GameWorld.h"                // G_GameWorld/IsIntegratedPvPBaseChannel
#include "CEnvironment.h"             // G_CEnvironment/m_serverEnvir.m_serverGroup

// ORIG C1 0x081b8170：CEventBase::CEventBase() + vptr 设置，无自身字段。
#include "NewAccountLevelUpEventToJob/NewAccountLevelUpEventToJob.inc"
{
}

// ORIG D1 0x081b818c：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查。
#include "NewAccountLevelUpEventToJob/dtor_NewAccountLevelUpEventToJob.inc"
{
}

// ORIG 0x081b81da：仅 SetEventFlag(true)，无日志。
#include "NewAccountLevelUpEventToJob/StartEvent.inc"
{
    SetEventFlag(true);
}

// ORIG 0x081b81f6：虚调 StartEvent()（vptr+8）后
// cMyTrace(__PRETTY_FUNCTION__,0x18,0)("NewAccountLevelUpEvent Event Start!")。
#include "NewAccountLevelUpEventToJob/StartEvent_1.inc"
{
    // [N7 封顶][b28c-33 复核 22/22] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("NewAccountLevelUpEvent Event Start!");
}

// ORIG 0x081b824c：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x2b,0)。
#include "NewAccountLevelUpEventToJob/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2b, 0);
    trace("NewAccountLevelUpEvent Event End!");
}

// ORIG 0x081b8246：空函数（push ebp/mov esp,ebp/pop ebp/ret）。
#include "NewAccountLevelUpEventToJob/StartAction.inc"
{
}

// ORIG 0x081b8816：直调 CEventBase::IsEventing(this, user)（vtable+0x3c 覆盖）。
#include "NewAccountLevelUpEventToJob/IsEventing.inc"
{
    return CEventBase::IsEventing(user);
}

// ORIG 0x081b829e：空函数（push ebp/mov esp,ebp/pop ebp/ret）。
#include "NewAccountLevelUpEventToJob/sendNewAccountLevelUpToJobData.inc"
{
}

// ORIG T 0x081b82a4 sz 0x515。
// 新账号升级转职奖励：查奖励表（growup 聚合 / 单项）→ 账号级 once-gift 多重
// 映射去重（同 job 区间内已存在同 level 则中止）→ 插入 (level, job) → DB
// 上报 → 逐条 create_item + 邮寄（标题 game_server_msg_285，正文
// snprintf(game_server_msg_286, level)），失败走 logFormat。成功返回 1，
// 前置失败返回 0。
// b28d-31: N 封顶（N82/P197）——ORIG 0x081b82a4；351 vs 353（帧/槽）。
#include "NewAccountLevelUpEventToJob/processLevelUpToJob.inc"
{
    // ORIG：全部 getter 在实参位内联求值（P24），不设持久局部 —— key 构造
    // firstGrow→esi(movsbl)、job→ebx 跨调用缓存、level→eax 直传；去重循环
    // 的 ==level 与 makeRequest/snprintf 的 level/job 各使用点重新调用 getter。
    LevelUpRewardItemTableKey key(user.get_charac_level(),
                                  user.get_charac_job(),
                                  user.getCurCharFirstGrowType());
    std::vector<LevelUpRewardItemData> rewards;    // vector C1 0x081b8c00
    if (growup)
    {
        // [P1T5 · .h 申请证据] ORIG 0x081b830a `call _Z14G_CDataManagerv` 后
        // 0x081b830f `lea 0xb5f8(%eax),%edx` ⇒ ORIG 的 m_levelUpReward 偏移 = 0xb5f8；
        // 我方 CDataManager.h:3796 注释 + 实测 `lea 0x50ac(%eax),%edx`（本 TU 0x1b 处）。
        // 差 0x6534（≈25.9KB）⇒ CDataManager 成员布局缺口，属共享头（P321-S2），
        // 本泳道只报告不改。注：m_itemList（本函数后半 create_item 处）同样受影响。
        if (!G_CDataManager()->m_levelUpReward.get_data_when_growup(key, rewards))
            return false;                          // ebx=0 出口
    }
    else
    {
        LevelUpRewardItemData data;                // C1 0x081b88bc
        if (!G_CDataManager()->m_levelUpReward.get_data(key, data))
            return false;
        rewards.push_back(data);                   // push_back 0x081b8a52
    }
    // getCharacLevelUpGift_AccountOnce（T 0x08691e5c）返回 &m_levelUpGift
    // （+0x8ec04，+0x00 即 multimap<int,int>）；empty/lower_bound/upper_bound/
    // insert 均以该指针为 this。迭代器经默认构造 + 赋值（临时中转拷贝，
    // P28），upper_bound 结果缓存，循环体内 it!=end() 防御检查。
    std::multimap<int, int>& gifts =
        *static_cast<std::multimap<int, int>*>(
            user.getCharacLevelUpGift_AccountOnce());
    if (!gifts.empty())
    {
        // [P1T5 修法 i4 · 已落] ORIG 循环尾 0x81b8477 调 `ppEi`（后置 ++，实参 0 +
        // 返回临时 -0x44），非 `ppEv` ⇒ 改 `it++`（丢弃返回值，语义等价）。
        // 四态实测（均以「引用缓存 + 本件」为底，ORIG=351/46 块/p167 差集 4 项）：
        //  ①两步声明+前置(基线) 353/gap −2/blocks 46/p167 3 项，TU identical=42/missing=22；
        //  ②两步声明+后置（**本落地形**）357/gap −6/**blocks 34/p167 0 项**，identical=42/missing=21；
        //  ③声明即初始化+前缀 348/gap +3/blocks 46，identical=**41**/missing=22；
        //  ④声明即初始化+后置 352/gap −1/blocks 30，identical=**41**/missing=20。
        // ⇒ ③④ 的 C1Ev 消失会让「`_Rb_tree_iterator C1Ev` 与 ORIG identical」那一项
        // 从账上掉（n1_v 单件实测 base=IDENTICAL、形B=MISSING_IN_OURS）⇒ 取 ②：
        // 四数不劣化（identical 42/ae 73/near 0/diff 2 同基线，missing 22→21 改善），
        // 且 blocks 46→34、p167 差集 4→0；代价是 gap −2→−6（C1Ev 调用 3 条 +
        // ORIG 确有的中转拷贝 2 条，均为符号级对齐所需）。
        // 残余：8 字节栈槽整体位移（我方 body −0x1ad vs ORIG −0x1b5）。
        std::multimap<int, int>::iterator it;
        it = gifts.lower_bound(user.get_charac_job());
        std::multimap<int, int>::iterator upper =
            gifts.upper_bound(user.get_charac_job());
        while (it != upper)
        {
            if (it != gifts.end() && it->second == user.get_charac_level())
                return false;                      // 该等级已领取 → 中止
            it++;   // [P1T5] ORIG 0x81b8477 为后置 `ppEi`（实参 0 + 返回临时 -0x44）。
        }
    }
    gifts.insert(std::make_pair(user.get_charac_job(),
                                user.get_charac_level()));
    DB_EventAccountCharacterLevelUpOnceGift::makeRequest(
        user.get_acc_id(), static_cast<short>(user.GetServerGroupToPvP()),
        user.getCurCharacNo(), static_cast<short>(user.get_charac_job()),
        static_cast<short>(user.get_charac_level()));
    char body[256] = { '\0' };                     // -0x1b5 起 0x100 清零（P4 内联 memset）
    OS_API::snprintf(body, 0xff,
                     g_scriptStringManager_.findString(4, "game_server_msg_286",
                                                       (bool*)0),
                     user.get_charac_level());
    for (std::vector<LevelUpRewardItemData>::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        // [P1T5 · p167] ORIG 255 只调一次 `__normal_iterator::de` 并把返回地址
        // 缓存到槽 -0x1c（`mov %eax,-0x1c(%ebp)`，0x81b8632），随后两处经
        // `mov -0x1c(%ebp),%eax` 访问 ⇒ 源码为循环内引用缓存形（非 `it->`），
        // 且该引用**声明早于 Inven_Item item**（ORIG de 调用 0x81b862d 先于
        // Inven_Item::C1Ev 0x81b863e）。
        const LevelUpRewardItemData& d = *it;
        Inven_Item item;
        item.m_addInfo = d.m_field0;               // -0xb3 ← data.f0
        G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0,
                                                  item, d.m_field4);
        // findString("game_server_msg_285") 在循环体内实参位（ORIG 每次迭代调）。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_285",
                                                  (bool*)0),
                item, 0, user.getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0,
                static_cast<ENUM_SERVER_GROUP>(user.GetServerGroupToPvP()), 0,
                0) != 0)
        {
            LogManager::logFormat(
                1, "NewAccountLevelUpEventToJob.cpp",
                __PRETTY_FUNCTION__,
                0x7f, "NewAccountLevelUpEventToJob Event Start!");
        }
    }
    return true;                                   // ebx=1
}
