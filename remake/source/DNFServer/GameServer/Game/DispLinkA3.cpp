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
    WongWork::CSecurityCard* card = user->getSecurityCard();
    if (card->isActivate() != 0)
    {
        card->setRequestState(WongWork::CSecurityCard::eRequestStateNone);
        card->incCancelCnt();
        unsigned int cancelCnt = (unsigned int)card->getCancelCnt();
        unsigned int accId = user->get_acc_id();
        DB_SecurityCardUpdateCancelCnt::makeRequest(accId, cancelCnt);
        (void)card->getCancelCnt();                       // ORIG 尾部读（结果丢弃）
    }
    else
    {
        Sanicova::CPad* pad = user->getPad();
        if (pad->isActivate() != 0)
        {
            pad->setRequestState(Sanicova::CPad::eRequestStateNone);
            pad->incCancelCnt();
            char const* webAddress = user->getWebAddress();
            unsigned int cancelCnt = (unsigned int)pad->getCancelCnt();
            unsigned int accId = user->get_acc_id();
            DB_PassPadUpdateCancelCnt::makeRequest(accId, cancelCnt,
                                                   (char*)webAddress);
            (void)pad->getCancelCnt();                    // ORIG 尾部读（结果丢弃）
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
Dispatcher_SDC_ActiveStatus_Damage_Check::Dispatcher_SDC_ActiveStatus_Damage_Check()
{
}

// ============================================================================
// 6. Dispatcher_UseCreatureEvolutionItem::process(CUser*, unsigned char, short, int)
//    ORIG @0x08228bcc [0x08228bcc, 0x08228e5d]
//    报告：docs/class_func_reports/Dispatcher_UseCreatureEvolutionItem/process.md
//
//    语义：按 (itemSpace, slot) 取背包引用 → find_item → 必须 stackable →
//          物品 +0x360 基怪 id 必须等于当前出战怪 GetCreatureIndex() →
//          CCreatureMgr::ItemEvolute(evolutionCreatureId)。
//    返回码：0 成功 / 1 ItemEvolute 失败 / 4 槽空 / 7 基怪不匹配 /
//            0x15 找不到 item / 0x17 非 stackable。
// ============================================================================
int Dispatcher_UseCreatureEvolutionItem::process(CUser* user, unsigned char space,
                                                 short slot, int evolutionCreatureId)
{
    // +0x009：GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space)（ORIG W 0x080f7845）
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)space);

    // +0x028：invenR = CUserCharacInfo::getCurCharacInvenR(user)（ORIG W 0x080da27e）
    CInventory* invenR = (CInventory*)user->getCurCharacInvenR();

    // +0x03e：ref = CInventory::GetInvenRef(invenType, slot) const（ORIG T 0x084fc1de）
    Inven_Item* ref = invenR->GetInvenRef(invenType, (int)slot);
    if (ref == 0)
    {
        return 4;
    }

    // +0x059：itemIdx = ref->m_addInfo（Inven_Item +0x02）
    int itemIdx = ref->m_addInfo;

    // +0x05e..0x06f：item = G_CDataManager()->find_item(itemIdx)（ORIG T 0x0835fa32）
    CDataManager* dataMgr = G_CDataManager();
    CItem* item = dataMgr->find_item(itemIdx);
    if (item == 0)
    {
        // +0x078..0x0ce：not found trace → return 0x15
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xe621, 5);
            trace("Dispatcher_UseCreatureEvolutionItem not found item_id(%d), charac_no(%d)\n",
                  itemIdx, characNo);
        }
        return 0x15;
    }

    // +0x0d3..0x0e1：if (!item->is_stackable()) → trace → return 0x17
    if (item->is_stackable() != true)
    {
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xe627, 5);
            trace("Dispatcher_UseCreatureEvolutionItem item_id(%d) is not stackable charac_no(%d)\n",
                  itemIdx, characNo);
        }
        return 0x17;
    }

    // +0x140：stackable 视图（CStackableItem 继承 CItem，+0x360 = 基怪 id）
    CStackableItem* stackable = (CStackableItem*)item;

    // +0x146..0x161：mgrW = getCurCharacInvenW(user)->GetCreatureMgrW()；idx = GetCreatureIndex()
    CInventory* invenW = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgrW = invenW->GetCreatureMgrW();
    int curCreatureIdx = mgrW->GetCreatureIndex();

    // +0x16a..0x206：cur_creatureid != item_base_creature_id(+0x360) → trace → return 7
    if (curCreatureIdx != stackable->m_int360)
    {
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        int baseCreatureId = stackable->m_int360;
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xe62f, 5);
            trace("Dispatcher_UseCreatureEvolutionItem cur_creatureid(%d) != item_base_creature_id(%d) charac_no(%d)\n",
                  curCreatureIdx, baseCreatureId, itemIdx, characNo);
        }
        return 7;
    }

    // +0x206..0x223：再次 getCurCharacInvenW()->GetCreatureMgrW() → ItemEvolute(id)
    CInventory* invenW2 = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgrW2 = invenW2->GetCreatureMgrW();
    bool evoluteOk = mgrW2->ItemEvolute(evolutionCreatureId) != 0;

    // +0x228..0x285：cHistoryTrace 成功/失败分支
    if (evoluteOk)
    {
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        {
            cHistoryTrace history;
            history("ItemEvolute Succ evolution_creature_id(%d), charac_no(%d)",
                    evolutionCreatureId, characNo);
        }
        return 0;
    }
    else
    {
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        {
            cHistoryTrace history;
            history("ItemEvolute Fail evolution_creature_id(%d), charac_no(%d)",
                    evolutionCreatureId, characNo);
        }
        return 1;
    }
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
