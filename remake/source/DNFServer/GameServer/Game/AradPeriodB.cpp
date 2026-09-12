// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AradPeriodB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstring>

#include "Arad_EventPeriodDataManager.h" // 类主布局 / EventRewardItemArad /
                                         // SearchEventPeriod / SRewardPeriodData
#include "Arad_DataManager.h"            // ARAD::PRINT_ERROR
#include "MoreTypes.h"                   // arad::SigSaveRewardEventItem
#include "CUser.h"                       // get_charac_no / GetUID / GetServerGroup /
                                         // get_acc_id
#include "Inven_Item.h"                  // Inven_Item::reset
#include "CDataManager.h"                // G_CDataManager / find_item
#include "CMailBoxHelper.h"              // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"     // g_scriptStringManager_ / findString
#include "GlobalData.h"                  // s_stream_pool / s_msgq_mgr
#include "LogManager.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "Arad_InternalDispatcher.h"     // DISPATCHER::make_internal_stream_jpn

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】FnRewardInterface 类与本类 operator() 定义前置，
// 使 `_GLOBAL__I_` 名等于 ORIG 的 `_ZN4ARAD17FnRewardInterfaceclEimsj`（P327）。
// make_data 仍留在下方命名空间块（类已在此声明）。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
namespace ARAD { namespace ARAD_ITEM {
#include "AradPeriodB/arad_make_item.inc"
} }

namespace ARAD {

// ---- FnRewardInterface -----------------------------------------------------
class FnRewardInterface
{
public:
    inline FnRewardInterface(CUser* user, bool recordCharacNo)   // ORIG W 0x0819a8c8
        : m_user(user), m_recordCharacNo(recordCharacNo)
    {
    }

    virtual void GetMailString(char* mailBuf, char* titleBuf) = 0;

    EventRewardItemArad make_data(int eventCode);              // T 0x08194e58
    bool operator()(int eventCode, itemindex_t itemIndex,        // [R13fixE2] ORIG prett 0x08b9c9e0
                    short count, arad_uint32 param);           // T 0x08194ac8

protected:
    CUser* m_user;
    bool   m_recordCharacNo;
};

#include "AradPeriodB/operator.inc"


}  // namespace ARAD

// ============================================================================
// 【报告缺失,objdump】ARAD::ARAD_ITEM::arad_make_item —— ORIG T 0x08197375
// (133B)：idx 为 0/-1 或 count==0 返 false；find_item(idx) 取物品；out.reset()
// 后 vtable+8 make_item(out) 填充；再写 m_addInfo(+2)=idx、m_addInfo2(+7)=count。
// ============================================================================
namespace ARAD {
namespace ARAD_ITEM {

#include "AradPeriodB/arad_make_item_1.inc"


}  // namespace ARAD_ITEM

#include "AradPeriodB/find_item.inc"


}  // namespace ARAD

#include "AradPeriodB/SRewardPeriodData.inc"
    int eventCode, unsigned long f4, short f8,
    unsigned int beginTime, unsigned int endTime)
{
    m_eventCode = eventCode;
    m_field4    = (unsigned int)f4;
    m_field8    = f8;
    m_beginTime = beginTime;
    m_endTime   = endTime;
}

namespace ARAD {

// ============================================================================
// FnReward 函数对象族 —— 布局由 ORIG C1/W 反汇编实证：
//   FnRewardInterface{ vptr; +4 CUser*; +8 bool }（C1 0x0819a8c8：vptr→user→bool）
//   FnRewardMomoji : FnRewardInterface（C1 0x0819a900：基(b=true)后再写自身 vptr；
//                    ZTV 只有槽0=GetMailString，MakeMailString 为唯一虚函数）
//   FnRewardLethe 非多态独立类 { +0 CUser* }（C1 0x0819a8f2 仅存指针）
// ============================================================================

#include "AradPeriodB/make_data.inc"
{
    if (m_recordCharacNo != 0)
    {
        return EventRewardItemArad(eventCode, m_user->get_acc_id(),
                                   m_user->get_charac_no(-1));
    }
    return EventRewardItemArad(eventCode, m_user->get_acc_id(), 0);
}

class FnRewardMomoji : public FnRewardInterface
{
public:
    inline explicit FnRewardMomoji(CUser* user)                 // ORIG W 0x0819a900
        : FnRewardInterface(user, true)
    {
    }

    virtual void GetMailString(char* mailBuf, char* titleBuf);
};

}  // namespace ARAD

#include "AradPeriodB/GetMailString.inc"
{
    std::strncpy(titleBuf, g_scriptStringManager_.findString(4, "momiji_event_title", (bool*)0), 0x14);
    std::strncpy(mailBuf, g_scriptStringManager_.findString(4, "momiji_event_mail", (bool*)0), 0xff);
}

namespace ARAD {

// ---- FnRewardLethe ---------------------------------------------------------
class FnRewardLethe
{
public:
    inline explicit FnRewardLethe(CUser* user)                  // ORIG W 0x0819a8f2
        : m_user(user)
    {
    }

    EventRewardItemArad make_data(int eventCode);              // T 0x08195340
    bool operator()(int eventCode, itemindex_t itemIndex,        // [R13fixE2] ORIG prett 0x08b9c980
                    short count, arad_uint32 param);           // T 0x08194ee2

private:
    CUser* m_user;
};

#include "AradPeriodB/make_data_1.inc"


#include "AradPeriodB/operator_1.inc"


}  // namespace ARAD

// ============================================================================
// ARAD::Arad_EventPeriodDataManager::eventReward<F> 模板主函数 +
// 两个显式实例化（ORIG W 0x0819abc8 / 0x0819adb6，各 493B 逐指令等价转写）。
// [b28d-solo-AradPeriodB] 判词：**AE**（check_tu routed；两实例各 147=147；
// p167 17=17 CLEAR ×2）。
// 修法：入口守卫合并短路物化岛（P214①）、it!=end/时间窗/fn() 正向包裹共享尾块
// （P222/P214①）、time 具名缓存槽 -0xc（P0）。
// 槽序翻正关键：pred 仅作 find_if 第三实参 ⇒ 去具名局部、内联匿名临时
// `SearchEventPeriod(eventCode)`，4.4 即把 fn/data/pred 排成 ORIG 的 pred -0x1c /
// fn -0x20 / data -0x24..-0x2c（4.1 先声明深方向）。具名 pred 时 4.4 恒先声明浅
// （fn -0x1c/pred -0x2c）⇒ NEAR；翻声明序 NEAR→DIFF 劣化。
// 新范式候选（P0 右值内联 + P212 槽序）：谓词类局部若仅作单次调用实参，内联为
// 匿名临时可同时消槽并把同块具名对象槽序翻回 ORIG 方向。
// ============================================================================
template <typename F>
#include "AradPeriodB/eventReward.inc"


template bool ARAD::Arad_EventPeriodDataManager::eventReward<ARAD::FnRewardLethe>(
    ENUM_REPEAT_EVENT_CODE, unsigned int, CUser*);
template bool ARAD::Arad_EventPeriodDataManager::eventReward<ARAD::FnRewardMomoji>(
    ENUM_REPEAT_EVENT_CODE, unsigned int, CUser*);

#include "AradPeriodB/getCurRewardItem.inc"

