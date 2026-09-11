// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureB_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <math.h>

#include "MySQL.h"
#include "DBMgr.h"
#include "GlobalData.h"
#include "GameTypes.h"        // cMyTrace
#include "CInventory.h"       // NumberToString
#include "WongWorkEvents_A.h" // EventCommon::InsertEventItem2EventTable
#include "CUser.h"
#include "AvatarCoin.h"
#include "DataManagerInitDepends.h" // TOD_UserState
#include "TimerQueue.h"
#include "TimerDispatcher.h"  // TimerStartMatching
#include "CAdvanceAltarLog.h" // advanceAltarstatistics_
#include "advancealtar.h"    // advancealtar::HistoryLog（advanceAltarstatistics_ 类静态成员声明）

// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1 串直证：ORIG PF 串
// `void WongWork::EventATI::ArrangeATIEventItem(memberIdentificationNumber_t, unsigned int)`
// / EventMuz 同构，类型名**无限定** ⇒ typedef 必须在全局作用域（先例
// DB_Login.cpp:89 / CGuildServerProxy.cpp:151）。= unsigned int，mangled 不变。
typedef unsigned int memberIdentificationNumber_t;

// ---------------------------------------------------------------------------
// WongWork::DBCommon::GetIdentity —— ORIG T 0x083f9ad4（报告 DBCommon/GetIdentity.md）
// seLect @@identity → exec(true)/fetch/get_uint(0)；任一步失败返回 0。
// ---------------------------------------------------------------------------
namespace WongWork {

class DBCommon
{
public:
    static unsigned int GetIdentity(MySQL* db);  // ORIG T 0x083f9ad4
};

#include "LinkClosureB_Misc/GetIdentity.inc"
{
    unsigned int identity;  // ORIG 无预清 store（仅 get_uint 出参写回）
    db->set_query("seLect @@identity");
    if (!db->exec(true))
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_uint(0, identity))
        return 0;
    return identity;
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::CWebEvent::GetItemType —— ORIG T 0x086c1aba（报告+objdump 核对）
// kind==2 离散表分派；kind==3 {8→2, 1→1}；其余 0。
// ---------------------------------------------------------------------------
namespace WongWork {

// CWebEvent 权威声明已收敛至 GlobalData.h:274(Main 沉降仲裁,含 GetItemType);
// 本 TU 的局部 class 视图已删除,仅保留 ORIG T 定义。

#include "LinkClosureB_Misc/GetItemType.inc"


}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventATI —— ORIG 事件族（报告 WongWork__EventATI/*）。
// 类与 DB_Login.cpp 的调用视图一致（全静态成员，无数据成员）。
// ---------------------------------------------------------------------------
namespace WongWork {

// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1 串直证：ORIG 的 Event 族 PF 串无
// static（`void WongWork::EventATI::ArrangeATIEventItem(memberIdentificationNumber_t,
// unsigned int)`），而 ORIG 里另有 22 条 `static void NS::Class::method(...)` 串
// （实证 `static void DB_LogBuyItem::makeRequest(...)`）⇒ GCC 会给静态成员打印
// static。ORIG 的 Event 族是 **namespace**（非 class 静态成员）。改后 mangled 不变、
// 调用写法不变、PF 串去 static。Check*/Update* 不用 __PRETTY_FUNCTION__ ⇒ 无串影响。
namespace EventATI
{
    int CheckAlreadyGetATIItem(unsigned int uid, int itemNo);
    void UpdateEventATIInfo(unsigned int uid, int itemNo);
    void ArrangeATIEventItem(memberIdentificationNumber_t mid, unsigned int serverId);
}

#include "LinkClosureB_Misc/CheckAlreadyGetATIItem.inc"


#include "LinkClosureB_Misc/UpdateEventATIInfo.inc"


#include "LinkClosureB_Misc/ArrangeATIEventItem.inc"


}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventMuz —— ORIG 事件族（报告 WongWork__EventMuz/*）
// ---------------------------------------------------------------------------
namespace WongWork {

// [L3FLinkClosureQuickParty 2026-09-11] 同 EventATI：ORIG Event 族为 namespace。
namespace EventMuz
{
    int CheckAlreadyGetMuzItem(unsigned int uid, int itemNo);
    void UpdateEventMuzInfo(unsigned int uid, int itemNo);
    void ArrangeMuzEventItem(memberIdentificationNumber_t mid, unsigned int serverId);
}

#include "LinkClosureB_Misc/CheckAlreadyGetMuzItem.inc"


#include "LinkClosureB_Misc/UpdateEventMuzInfo.inc"


#include "LinkClosureB_Misc/ArrangeMuzEventItem.inc"


}  // namespace WongWork

#include "LinkClosureB_Misc/SubLog.inc"


#include "LinkClosureB_Misc/getLastEnterLayer.inc"


// ---------------------------------------------------------------------------
// advanceAltarstatistics_ —— ORIG B @0x09400300 全局实例（构造/析构随全局
// __cxa 链）。唯一定义点；extern 声明在 CAdvanceAltarLog.h。
// ---------------------------------------------------------------------------
CAdvanceAltarLog advancealtar::HistoryLog::advanceAltarstatistics_;  // ORIG B 0x09400300

#include "LinkClosureB_Misc/GetItemDropRate.inc"


#include "LinkClosureB_Misc/ChangeItemRarity.inc"

