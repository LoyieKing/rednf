// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DispLinkA3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Dispatchers.h"

#include "CUser.h"            // get_state / getSecurityCard / getPad / getWebAddress /
                              //   get_acc_id；CUserCharacInfo 继承链
#include "CUserCharacInfo.h"  // getCurCharacInvenR / getCurCharacInvenW / getCurCharacNo
#include "CInventory.h"       // GetInvenTypeFromItemSpace / GetInvenRef / GetCreatureMgrW；
                              //   user_creature::CCreatureMgr::{GetCreatureIndex,ItemEvolute}
#include "CDataManager.h"     // G_CDataManager / CDataManager::find_item
#include "CItem.h"            // CItem::is_stackable
#include "CStackableItem.h"   // CStackableItem::m_int360（+0x360 基怪 id）
#include "GameTypes.h"        // cMyTrace
#include "cHistoryTrace.h"    // cHistoryTrace

// ---- DB 取消计数请求类（ORIG 符号 _ZN25DB_PassPadUpdateCancelCnt11makeRequestEjjPKc /
//      _ZN30DB_SecurityCardUpdateCancelCnt11makeRequestEjj；定义在 DB_AccountCargoSync.cpp
//      @0x0842403a / 0x0842c112）。注：DB_AccountCargoSync.h 中当前声明为非 static 成员，
//      而 ORIG 调用方（本函数与 CUser.cpp:7187/7192）均为静态调用形态；符号一致可链接，
//      头文件声明需后续统一为 static（已报告，不在本 TU 修改）。
class DB_SecurityCardUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt);
};

class DB_PassPadUpdateCancelCnt
{
public:
    // ORIG T 0x0842403a（mangled EjjPc 非 const）。
    static void makeRequest(unsigned int accId, unsigned int cancelCnt,
                            char* webAddress);
};

// ============================================================================
// 1. Dispatcher_SecretShopBuyItem::check_error
//    ORIG @0x081d26c0 [0x081d26c0, 0x081d26e9]
//    语义：channel state 必须为 5（dungeon 内）；否则 0x7fffffff（上层静默）。
// ============================================================================
#include "DispLinkA3/check_error.inc"
{
    (void)msg;
    (void)param;
    if (user->get_state() != 5)
    {
        return (int)0x7fffffff;
    }
    return 0;
}

// ============================================================================
// 2. Dispatcher_SecretShopOpenClose::check_error
//    ORIG @0x081d288a [0x081d288a, 0x081d28b3]，与 SecretShopBuyItem 同型。
// ============================================================================
#include "DispLinkA3/check_error_1.inc"
{
    (void)msg;
    (void)param;
    if (user->get_state() != 5)
    {
        return (int)0x7fffffff;
    }
    return 0;
}

// ============================================================================
// 3. Dispatcher_SecurityCardAuthCancel::process(CUser*)
//    ORIG @0x0820a580 [0x0820a580, 0x0820a6bb]
//    语义：安全卡激活 → 清请求状态 + 取消计数自增 + DB_SecurityCardUpdateCancelCnt；
//          否则若密保 Pad 激活 → 同型走 CPad + DB_PassPadUpdateCancelCnt（带 web 地址）。
//          两条路径尾部的 getCancelCnt() 读取调用在 ORIG 中存在（内联读），保留。
// ============================================================================
#include "DispLinkA3/process.inc"
{
    // [b23S3C] P48：ORIG getSecurityCard ×5 / getPad ×5 每用点独立重取
    // （p167 MISSING 各 -4 即此），card/pad 无具名槽。
    if (user->getSecurityCard()->isActivate() != 0)
    {
        user->getSecurityCard()->setRequestState(
            WongWork::CSecurityCard::eRequestStateNone);
        user->getSecurityCard()->incCancelCnt();
        DB_SecurityCardUpdateCancelCnt::makeRequest(
            user->get_acc_id(),
            (unsigned int)user->getSecurityCard()->getCancelCnt());
        // [b23S3C] ORIG +0x08c：尾部 getCancelCnt 后 `cmp $0x9,%eax` 死比较
        // （结果弃置、无分支）⇒ 比较表达式语句形。
        if (user->getSecurityCard()->getCancelCnt() == 9)  // ORIG cmp $9（死比较岛）
        {
        }
    }
    else
    {
        if (user->getPad()->isActivate() != 0)
        {
            user->getPad()->setRequestState(Sanicova::CPad::eRequestStateNone);
            user->getPad()->incCancelCnt();
            DB_PassPadUpdateCancelCnt::makeRequest(
                user->get_acc_id(),
                (unsigned int)user->getPad()->getCancelCnt(),
                (char*)user->getWebAddress());
            if (user->getPad()->getCancelCnt() == 9)      // ORIG +0x12d cmp $9（死）
            {
            }
        }
    }
    return 1;
}

// ============================================================================
// 4. Dispatcher_SelectUltimateDifficulty::process(CUser*, MSG_BASE&, ParamBase&)
//    ORIG @0x0823426c W：push ebp; mov esp,ebp; mov $0x0,%eax; pop ebp; ret —— 空体。
//    实际业务逻辑在 dispatch_sig（Dispatchers.cpp 已还原）。
// ============================================================================
#include "DispLinkA3/process_1.inc"
{
    (void)user; (void)msg; (void)param;
    return 0;
}

// ============================================================================
// 5. Dispatcher_SDC_ActiveStatus_Damage_Check ctor
//    ORIG @0x08596674 W：call IPacketDispatcher<MSG_BASE,ParamBase,ch_state0>::C1 + 设 vptr。
//    类无新增数据成员；空定义由编译器生成同形代码（vptr 由基类多态隐式写入）。
// ============================================================================
#include "DispLinkA3/Dispatcher_SDC_ActiveStatus_Damage_Check.inc"
{
}



// ============================================================================
// 7. Dispatcher_ValidateScriptHash::check_error(CUser*, MSG_BASE&)
//    ORIG @0x0826435a [0x0826435a, 0x08264363]：mov $0x0,%eax; pop ebp; ret。
// ============================================================================
#include "DispLinkA3/check_error_2.inc"
{
    (void)user;
    (void)msg;
    return 0;
}
