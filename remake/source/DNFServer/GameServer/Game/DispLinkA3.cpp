// ============================================================================
// DispLinkA3.cpp —— LinkHelper.DispLinkA3 独占 TU
// 核销 /tmp/undef_r21.txt 中 7 类 8 个 Dispatcher 符号（调用方 Dispatchers.cpp，
// 本 TU 不改它）。类声明均在 Dispatchers.h（include，勿改）。
//
// 符号清单（ORIG 地址 / 报告路径见各函数头注释）：
//   1. Dispatcher_SecretShopBuyItem::check_error(CUser*, MSG_BASE&, ParamBase&)
//        ORIG _ZN28Dispatcher_SecretShopBuyItem11check_errorEP5CUserR8MSG_BASER9ParamBase
//        @0x081d26c0 T  报告 docs/class_func_reports/Dispatcher_SecretShopBuyItem/check_error.md
//   2. Dispatcher_SecretShopOpenClose::check_error(CUser*, MSG_BASE&, ParamBase&)
//        ORIG _ZN30Dispatcher_SecretShopOpenClose11check_errorEP5CUserR8MSG_BASER9ParamBase
//        @0x081d288a T  报告 docs/class_func_reports/Dispatcher_SecretShopOpenClose/check_error.md
//   3. Dispatcher_SecurityCardAuthCancel::process(CUser*)
//        ORIG _ZN33Dispatcher_SecurityCardAuthCancel7processEP5CUser @0x0820a580 T
//        报告 docs/class_func_reports/Dispatcher_SecurityCardAuthCancel/process.md
//   4. Dispatcher_SelectUltimateDifficulty::process(CUser*, MSG_BASE&, ParamBase&)
//        ORIG _ZN35Dispatcher_SelectUltimateDifficulty7processEP5CUserR8MSG_BASER9ParamBase
//        @0x0823426c W（ORIG 即空体 return 0，逐指令核对 0x0823426c..0x08234276）
//   5. Dispatcher_SDC_ActiveStatus_Damage_Check::Dispatcher_SDC_ActiveStatus_Damage_Check()
//        ORIG _ZN40Dispatcher_SDC_ActiveStatus_Damage_CheckC1Ev @0x08596674 W
//        （objdump：仅调 IPacketDispatcher<MSG_BASE,ParamBase,ch_state0>::C1 + 设 vptr；
//         类无数据成员，正常空定义由编译器生成同形代码）
//   6. Dispatcher_UseCreatureEvolutionItem::process(CUser*, unsigned char, short, int)
//        ORIG _ZN35Dispatcher_UseCreatureEvolutionItem7processEP5CUserhsi @0x08228bcc T
//        报告 docs/class_func_reports/Dispatcher_UseCreatureEvolutionItem/process.md
//   7. Dispatcher_ValidateScriptHash::check_error(CUser*, MSG_BASE&)
//        ORIG _ZN29Dispatcher_ValidateScriptHash11check_errorEP5CUserR8MSG_BASE @0x0826435a T
//        报告 docs/class_func_reports/Dispatcher_ValidateScriptHash/check_error.md
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
int Dispatcher_SecretShopBuyItem::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
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
int Dispatcher_SecretShopOpenClose::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
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
int Dispatcher_SecurityCardAuthCancel::process(CUser* user)
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
int Dispatcher_SelectUltimateDifficulty::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)user; (void)msg; (void)param;
    return 0;
}

// ============================================================================
// 5. Dispatcher_SDC_ActiveStatus_Damage_Check ctor
//    ORIG @0x08596674 W：call IPacketDispatcher<MSG_BASE,ParamBase,ch_state0>::C1 + 设 vptr。
//    类无新增数据成员；空定义由编译器生成同形代码（vptr 由基类多态隐式写入）。
// ============================================================================
Dispatcher_SDC_ActiveStatus_Damage_Check::Dispatcher_SDC_ActiveStatus_Damage_Check() throw()
{
}



// ============================================================================
// 7. Dispatcher_ValidateScriptHash::check_error(CUser*, MSG_BASE&)
//    ORIG @0x0826435a [0x0826435a, 0x08264363]：mov $0x0,%eax; pop ebp; ret。
// ============================================================================
int Dispatcher_ValidateScriptHash::check_error(CUser* user, MSG_BASE& msg)
{
    (void)user;
    (void)msg;
    return 0;
}
