// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserMissingA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>  // memset

#include "CUser.h"                  // CUser/UserMercenaryInfoMgr/CHARAC_LOAD_MERCENARY/_Charac_info
#include "CInventory.h"             // CInventory::GetInvenRef(int,int) const、INVEN_TYPE_INVEN=0
#include "CDataManager.h"           // G_CDataManager / find_item(int) const
#include "CEquipItem.h"             // CEquipItem 完整类型（dynamic_cast 目标 + IsAvatarItem 槽 4）
#include "CFatigueBatteryHandle.h"  // 本类声明
#include "GameTypes.h"              // cMyTrace(name, line, flag) + operator()
#include "GlobalData.h"             // s_stream_pool / s_msgq_mgr
#include "StreamPool.h"             // StreamPool::Acquire(char const*, int)
#include "W8LT_DBTask.h"            // DB_UpdateBingoEvent 权威声明（makeRequest/dispatch）
#include "MsgQueueMgr.h"            // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "SigTypes.h"               // SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION
#include "DB_LoadCharacHotKeyOption.h"
#include "CItem.h"                // CItem::get_rarity（Reward 尾部）
#include "CItemList.h"            // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMailBoxHelper.h"       // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "CSystemTime.h"          // CSystemTime::getCurSec
#include "DB_MercenaryLogInfo.h"  // DB_MercenaryLogInfo::makeRequest
#include "PacketTypes.h"          // competition_reward_result 完整定义
#include "PvPTypes.h"             // CMTRand（probArea/probabilityRewardItem 局部 static）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include <stdio.h>                // sprintf / strncpy / strlen
#include "PacketGuard.h"           // PacketGuard（SendUnlimitChallengePacket）
#include "InterfacePacketBuf.h"    // put_header / put_byte / finalize
#include "CEventScriptMng.h"       // EventClassify::CEventScriptMng::process_level_up_reward
#include "Quest.h"               // Quest / RewardItemInfo 完整类型

// ----------------------------------------------------------------------------
// ORIG W 模板（本 TU 消费；符号 0x081b48da/<int> 0x081b4a66 / 0x081b495d）：
// 遍历累加 first 概率，dice<=累计 命中返回 second；未命中回退首元素 second。
// ----------------------------------------------------------------------------
template <typename T>
T FindRelativeProb(const std::vector<std::pair<int, T> >& vec, int dice)
{
    int sum = 0;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        sum += vec[i].first;
        if (dice <= sum)
            return vec[i].second;
    }
    return vec[0].second;
}

// ----------------------------------------------------------------------------
// ORIG W 0x081b495d：取 first<=level 中 first 最大的元素，返回其 second。
// ----------------------------------------------------------------------------
template <typename T>
T Find_max_value_lower_N(int level,
                         const std::vector<std::pair<int, T> >& vec)
{
    int bestIdx = 0;
    int bestFirst = 0;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        // [b28d-27] ORIG 0x081b495d：两条件各自 setcc 物化（P225 推广，`&&`
        // 合并 4.4 恒折 jg/jl 直跳）⇒ 拆嵌套 if。
        if (vec[i].first <= level)
        {
            if (vec[i].first >= bestFirst)
            {
                bestFirst = vec[i].first;
                bestIdx = i;
            }
        }
    }
    return vec[bestIdx].second;
}

// ----------------------------------------------------------------------------
// ORIG b 0x09407e80（.bss 全局 string，internal linkage；初值推断为空串，
// 运行期默认构造。GetRewardItem/Reward 用作兜底邮政消息）。
// ----------------------------------------------------------------------------
namespace MercenaryHardCodingSettingValue
{
    static std::string MERCENARY_BASE_POSTAL_MESSAGE;
    // ORIG r 0x08bb8754 = 0x2710 (10000)，_ZN31MercenaryHardCodingSettingValueL19ITEM_DROP_MAX_VALUEE
    static const int ITEM_DROP_MAX_VALUE = 10000;
}

#include "CUserMissingA2/CheckCompetitionAreaIndex.inc"


#include "CUserMissingA2/CheckLoadHotKeyCashedCharacType.inc"


#include "CUserMissingA2/GetCharacHotKeyType.inc"


#include "CUserMissingA2/CalcMercenaryAvatarBonus.inc"


#include "CUserMissingA2/setAvatarTypeBonus.inc"


#include "CUserMissingA2/FindIndexByCharacNo.inc"


#include "CUserMissingA2/CheckCharacIndex.inc"


#include "CUserMissingA2/IsWaitingStateCharac.inc"


#include "CUserMissingA2/CheckValidAppend.inc"


#include "CUserMissingA2/ReturnCharac.inc"


#include "CUserMissingA2/CompetitionCharac.inc"


#include "CUserMissingA2/Return_Mercenary_Charac.inc"


#include "CUserMissingA2/GetCompetitionState.inc"


#include "CUserMissingA2/CHARAC_LOAD_MERCENARY.inc"


#include "CUserMissingA2/Reset.inc"


#include "CUserMissingA2/makeRequest.inc"
                                            int hotkeyType)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x749a), true);
    guard.operator*()->operator<<(800);   // SIG_LOAD_CHARAC_HOTKEY_OPTION opcode
    guard.operator*()->operator<<(uid);

    struct View
    {
        unsigned int m_accId;             // +0x00（dispatch 作 SQL where 键）
        int m_hotkeyType;                 // +0x04
    };
    View* v = reinterpret_cast<View*>(
        guard.operator->()->GetInBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION>());
    v->m_accId = accId;
    v->m_hotkeyType = hotkeyType;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// CHARAC_LOAD_MERCENARY 竞技域 15 函数（ORIG 报告：docs/class_func_reports/
// CHARAC_LOAD_MERCENARY/*.md；alter/save_complete 报告缺失按 objdump 取证）。
// ============================================================================

#include "CUserMissingA2/CheckCompetitionPeriodIndex.inc"


#include "CUserMissingA2/GetAvatarBonus.inc"


#include "CUserMissingA2/GetPeriodBonus.inc"


#include "CUserMissingA2/CalcItemDropRate.inc"


#include "CUserMissingA2/GetRewardCritical.inc"


#include "CUserMissingA2/GetCompetitionPeriodSecond.inc"


#include "CUserMissingA2/GetCompetitionPeriodTerm.inc"


#include "CUserMissingA2/SendMessageToCUser.inc"


#include "CUserMissingA2/alter.inc"


#include "CUserMissingA2/save_complete.inc"


#include "CUserMissingA2/GetRewardGold.inc"


#include "CUserMissingA2/GetRewardItem.inc"


#include "CUserMissingA2/Reward.inc"


#include "CUserMissingA2/Competition.inc"


// ============================================================================
// Wave15 批：CUser 仅ORIG 强符号补齐（逐报告核对；真实语义/签名）
// ============================================================================

#include "CUserMissingA2/GetETC.inc"


#include "CUserMissingA2/get_SoloPlay_mode.inc"


#include "CUserMissingA2/set_MailMsgData.inc"


#include "CUserMissingA2/SendUnlimitChallengePacket.inc"


#include "CUserMissingA2/GetItemLimitInfo.inc"


#include "CUserMissingA2/IsLimitItemUsable.inc"


#include "CUserMissingA2/processGrowTypeEvent.inc"


#include "CUserMissingA2/Competition_Mercenary_Charac.inc"


#include "CUserMissingA2/updateBingoData.inc"


// ----------------------------------------------------------------------------
// ORIG T 0x08651de6 _ZN5CUser17UpdateBloodRecordEv：
// 遍历当前角色 _Charac_info.m_bloodBestRecords（+0xecc），每项经 stream 发
// SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD（sig+0=characNo、+4=m_time、+8=m_round、
// +0xc=key）到 LOGDB 队列；末尾 setBloodBestRecordUpdate(false)。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 3）：ORIG 0x08651de6。
// [b28d-solo-CUserMissingA2] 已修复为 **AE**（127/127；p167 24/24 CLEAR）：
// [b28d-solo-CUserMissingA2] 迭代器族归位：ORIG 0x08651de6 用**非 const**
// `_Rb_tree_iterator`（p167：MISSING 非 const begin/end/ptEv/ppEv/ne，EXTRA 我方
// const 版全族）⇒ 原码 cur 为非 const 指针；本函数只读遍历，用 const_cast 取回
// 非 const 视图（不改共享头，避免全仓重编译）。循环条件形 = `it != map.end()`
// （ORIG end 调用点在循环底部，无 end 具名缓存）。
// [b28d-solo-CUserMissingA2] store 序：ORIG 4 处 store 全为 RHS-first + disp 直存
// （`mov RHS,%edx; mov sig,%eax; mov %edx,disp(%eax)`）⇒ 原码是**具名视图结构成员
// 赋值**（裸 `*(unsigned int*)((char*)sig+K)` 产 lea+ebx 跨 call 缓存，DIFF）；
// 字段类型须与 RHS 精确同型（probe /tmp/b28d_solo/probe_view4.cpp fK 逐条实证）。
struct BloodBestRecordSigView
{
    unsigned int m_characNo;   // +0x00（getCurCharacNo 返回 unsigned int，同型免中转）
    unsigned int m_time;       // +0x04（stBloodBestRecord::m_time）
    int m_round;               // +0x08（stBloodBestRecord::m_round）
    int m_key;                 // +0x0c（pair::first）
};

#include "CUserMissingA2/UpdateBloodRecord.inc"


#include "CUserMissingA2/SetEquipment.inc"


#include "CUserMissingA2/processFatigueCharac.inc"


#include "CUserMissingA2/processAwakenReward.inc"


// ----------------------------------------------------------------------------
// ORIG T 0x0868e36a _ZN5CUser17SaveMercenaryDataEv：
// m_mercenaryMgr.m_characs 空则返 1；否则 stream 写 0x2b3+GetUID，
// SIG_MERCENARY_DB_DATA 清零后逐项（+0x24 脏标记）memcpy 0x54 到
// db+4+row*0x54 并 save_complete()；入 LOGDB 队列，返回 1。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（p167 MISSING 4）：ORIG 0x0868e36a；ORIG 多 4 条调用（STL 族），
// 我方实现语义等价但实例/形式不同（N89 类）+ 帧差。
// [b28d-solo-CUserMissingA2] 复验 136/132：p167 20/20 CLEAR；残余为槽序
// （4.4 首次使用序 vs 4.1）+ `imul $0x54` 强度归约（4.4 恒 shl/add 链）+
// `*db` store 序 ⇒ N82/N101 封顶。
// [b28d-solo-CUserMissingA2] p167 归位：ORIG 0x0868e36a 调 CStreamGuard::operator*
// ×2 + operator-> ×1（代理链）、operator[] ×3（P157 逐次重读，非引用缓存）、
// 结构整体赋值 `rep movsl 0x15`（非 memcpy）；row 为每次从 *db 读的具名局部。
struct MercenaryDbHeadView
{
    unsigned int m_count;   // +0x00（SIG_MERCENARY_DB_DATA 首 dword = 行数）
};

#include "CUserMissingA2/SaveMercenaryData.inc"


// ----------------------------------------------------------------------------
// ORIG T 0x0868f726 _ZN5CUser20updatePcRoomPlayTimeEv：
// 取事件脚本管理器 get_event_entity(0x5c) 为 CPcRoomPlayTime；verify 通过
// （==0）时 collectPcRoomPlayTime() 并 DB_SavePcRoomPlayTimeEvent::makeRequest
// (accId, totalPlayTime, rewardCnt)。
// ----------------------------------------------------------------------------
class DB_SavePcRoomPlayTimeEvent
{
public:
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};

#include "CUserMissingA2/updatePcRoomPlayTime.inc"


#include "CUserMissingA2/_fnStatQuestClear.inc"


// ----------------------------------------------------------------------------
// ORIG _ZL14elixir_item_id（df_game_r .rodata 8×int；值 1039..1046 实证）。
// 精灵药水道具 id 表，calc_quest_stat 的匹配键集。
// ----------------------------------------------------------------------------
// [AnonLane2 2026-09-11 匿名轴] 改文件级 static（P204③）：ORIG 侧该表为
// `_ZL14elixir_item_id`（每 TU 一份 local static；df_game_r 全库 350+ 份同形，
// 我方 AlignDeadConsts.cpp 亦以 `static const unsigned char` 对齐该名），
// 匿名命名空间版 `_ZN12_GLOBAL__N_1L14elixir_item_idE` 为 only-NEW。
static const int elixir_item_id[8] = { 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046 };

#include "CUserMissingA2/calc_quest_stat.inc"

