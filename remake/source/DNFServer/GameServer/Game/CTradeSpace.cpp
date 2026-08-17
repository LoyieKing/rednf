// ============================================================================
// df_game_r 还原 —— CTradeSpace（交易空间，G2-4 批次）
// 参照 docs/class_func_reports/CTradeSpace.md + ORIG 弱符号反汇编。
// 布局（ORIG CTradeSpace::CTradeSpace 0x082a416c / SetIDX 0x082a41f4 推导）：
//   +0x00 Mutex（0x18）          +0x18 int m_index
//   +0x1c char m_bTrading        +0x1d char m_bLocked
//   +0x20 CUser* m_pTraders[2]   +0x28 int m_nState[2]
//   +0x30 TradeSlot m_slots[2][26]（每槽 0x48，每行 0x798）
//   +0xf60 int m_nMoney[2]       （总尺寸 0xf70）
// 本 TU 只做 CTradeSpace 自身（含 TradeSlot 构造/Reset），跨类调用以真实符号
// 声明（头文件已有者直接使用；缺失者用 asm-label extern）。
// ============================================================================

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "CUser.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "InterfacePacketBuf.h"

// ---- 跨类符号声明（真实符号，定义在其它 TU；头文件缺失或签名不符者用
//      asm-label extern）----
extern "C" void sub_CUser_SetTradeSpace(void* user, int idx)
    asm("_ZN5CUser13SetTradeSpaceEi");
extern "C" void sub_CUser_IncrementTradeGold(void* user, unsigned int gold,
                                             unsigned int flag)
    asm("_ZN5CUser18IncrementTradeGoldEji");
extern "C" void sub_CUser_gainMoneyFromHackUser(void* user, unsigned long a,
                                                unsigned char b, unsigned long c,
                                                unsigned char d)
    asm("_ZN5CUser21gainMoneyFromHackUserEjyjy");
extern "C" void* sub_G_CGameManager() asm("_Z14G_CGameManagerv");
extern "C" void sub_CGameManager_PutTradeSpace(void* mgr, void* tradeSpace)
    asm("_ZN12CGameManager13PutTradeSpaceEP11CTradeSpace");

// cUserHistoryLog（CUser +0x79700；头文件类缺少交易方法，用 asm-label）
extern "C" void sub_cUserHistoryLog_SetTrader(void* log, const char* accName,
                                              const char* characName)
    asm("_ZN15cUserHistoryLog9SetTraderEPKcS1_");
extern "C" void sub_cUserHistoryLog_TradeBegin(void* log)
    asm("_ZN15cUserHistoryLog10TradeBeginEv");
extern "C" void sub_cUserHistoryLog_TradeEnd(void* log, int reason, int a, int b,
                                             int c, int d)
    asm("_ZN15cUserHistoryLog8TradeEndE15eTradeEndReasoniiii");

// DB_AvatarChangeOwner
extern "C" void sub_DB_AvatarChangeOwner_makeRequest(unsigned int a,
                                                     unsigned int b,
                                                     unsigned int c,
                                                     unsigned char d)
    asm("_ZN20DB_AvatarChangeOwner11makeRequestEjjjh");

// CSecu_ProtectionField（头文件按 3 参调用点形态声明，与本类调用点不符，asm）
extern "C" unsigned int sub_CSecu_ProtectionField_Check(void* field, CUser* user,
                                                        unsigned int type)
    asm("_ZN21CSecu_ProtectionField5CheckEP5CUser18SECURITY_PROTCTION");
extern "C" unsigned char sub_CSecu_ProtectionField_GetOppositeErr(void* field,
                                                                  unsigned int err)
    asm("_ZN21CSecu_ProtectionField14GetOppositeErrEi");
extern "C" void sub_SendChangeTradeState(PacketGuard& packet, CUser* user,
                                         CUser* target, int state)
    asm("_Z20SendChangeTradeStateR11PacketGuardP5CUserS2_15ENUM_TRADESTATE");
extern "C" void sub_CUserHistoryLog_TradeItemAddFail(void* log, int itemIdx,
                                                     int count)
    asm("_ZN15cUserHistoryLog16TradeItemAddFailEii");
namespace GlobalData
{
extern CSecu_ProtectionField* s_pSecuProtectionField;
}

// get_rand_int（CItemList.h 已有 C++ 链接声明）
int get_rand_int(int max);

// WongWork::CHackAnalyzer::addServerHackCnt（头文件参数类型名不符，asm）
namespace WongWork
{
extern "C" void sub_CHackAnalyzer_addServerHackCnt(void* analyzer, CUser* user,
                                                   int type, unsigned int a,
                                                   unsigned int b,
                                                   unsigned int c)
    asm("_ZN8WongWork13CHackAnalyzer16addServerHackCntEP5CUserNS_13ENUM_HACKTYPEEjjj");
extern "C" int sub_CAvatarItemMgr_GetExpireDate(void* mgr, int slot)
    asm("_ZN8WongWork14CAvatarItemMgr12GetExpireDateEi");
extern "C" char sub_CAvatarItemMgr_IsTempKey(void* mgr, int key)
    asm("_ZN8WongWork14CAvatarItemMgr9IsTempKeyEi");
}  // namespace WongWork

// user_creature::CCreatureMgr（头文件缺方法，asm）
namespace user_creature
{
extern "C" void sub_CCreatureMgr_ChangeSlotNo(void* mgr, Inven_Item* item,
                                              int a, int b)
    asm("_ZN13user_creature12CCreatureMgr12ChangeSlotNoEP10Inven_Itemii");
extern "C" void sub_CCreatureMgr_ChangeOwner(void* mgr, Inven_Item* item,
                                             int a, int b, int c, int d)
    asm("_ZN13user_creature12CCreatureMgr11ChangeOwnerEP10Inven_Itemiiii");
extern "C" int sub_CCreatureMgr_DeleteCreatureItem(void* mgr, int a, int b)
    asm("_ZN13user_creature12CCreatureMgr18DeleteCreatureItemEii");
extern "C" char sub_CCreatureMgr_IsCreatureEquipmentScope(void* mgr, int a)
    asm("_ZN13user_creature12CCreatureMgr24IsCreatureEquipmentScopeEi");
}  // namespace user_creature

// 邮件（send_lose_item_for_china；CMailBoxHelper 头文件类缺方法，asm）
class RDARScriptStringManager
{
public:
    const char* findString(int table, const char* key, bool* flag) const;
};
extern RDARScriptStringManager g_scriptStringManager_;
extern "C" void sub_CMailBoxHelper_ReqDBSendNewSystemMail(
    const char* title, const Inven_Item& item, unsigned int a,
    unsigned int characNo, const char* msg, int msgLen, unsigned int b,
    ENUM_SERVER_GROUP group, bool c, bool d)
    asm("_ZN8WongWork14CMailBoxHelper22ReqDBSendNewSystemMailEPKcRK10Inven_ItemjjS2_ij17ENUM_SERVER_GROUPbb");
extern "C" int sub_CUser_GetServerGroup(void* user)
    asm("_ZNK5CUser14GetServerGroupEv");
extern "C" void sub_Inven_Item_C1(void* item) asm("_ZN10Inven_ItemC1Ev");
extern "C" void sub_TradeSlot_C1(void* slot) asm("_ZN9TradeSlotC1Ev");

// ---- TradeSlot（0x48；Inven_Item 位于 +0x09）----
#pragma pack(push, 1)
struct TradeSlot
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
    char m_field08;  // +0x08
    Inven_Item m_item;  // +0x09（0x3d）
    char m_pad[0x48 - 0x09 - 0x3d];

    TradeSlot();
    void Reset();
};
#pragma pack(pop)

TradeSlot::TradeSlot()
{
    Reset();
}

void TradeSlot::Reset()
{
    m_item.Inven_Item::reset();
    m_field00 = 0;
    m_field04 = 0;
    m_field08 = 0;
}

// ---- CTradeSpace ----
class CTradeSpace
{
public:
    CTradeSpace();
    ~CTradeSpace();

    CUser* GetOppositeUser(CUser* user);
    int _CheckTrade4SameAvatar(const TradeSlot* slots, const Inven_Item& item);
    int _IsTradable(const Inven_Item& item);
    void _SaveAvatarItemOwnerChange(unsigned int a, unsigned int b,
                                    unsigned int c, unsigned char d);
    int add_item(int traderIdx, TradeSlot slot, int count);
    int cancel_trade();
    void cancel_trade_by_dis(CUser* user);
    int cancel_trade_for_china(CUser* user);
    int change_trade_state(CUser* user, int state);
    int checkCancelTrade();
    int checkTrade();
    int check_item_exist(int traderIdx, int itemId);
    int check_trade_possibility();
    int get_empty_itemslot(int traderIdx);
    int proceed_trade();
    int regist_item(CUser* user, int type, int a, int b);
    int remove_item(CUser* user, int a, int b, int c, int d);
    void reset();
    int send_lose_item_for_china(CUser* user, Inven_Item& item);
    void set_traders(CUser* a, CUser* b);

    void SetIDX(int idx);
    void lock();
    void unlock();
    char IsLocked();
    void _SetLock(bool flag);
    char is_empty();
    int get_index();

private:
    char m_mutex[0x18];       // +0x00
    int m_index;              // +0x18
    char m_bTrading;          // +0x1c
    char m_bLocked;           // +0x1d
    CUser* m_pTraders[2];     // +0x20
    int m_nState[2];          // +0x28
    char m_slotsData[2 * 0x798];  // +0x30（行距 0x798 = 27 * 0x48；手写构造）
    int m_nMoney[2];          // +0xf60
    char m_pad[0xf70 - 0xf68];
};

extern "C" void sub_Mutex_C1(void* m) asm("_ZN5MutexC1Ev");
extern "C" void sub_Mutex_D1(void* m) asm("_ZN5MutexD1Ev");

// ---- 构造 / 析构（ORIG 0x082a416c / 0x082a41e0）----
CTradeSpace::CTradeSpace()
{
    sub_Mutex_C1(m_mutex);
    m_bTrading = 0;
    m_bLocked = 0;
    for (int t = 1; t != -1; --t)
        for (int s = 0x1a; s != -1; --s)
            sub_TradeSlot_C1(&m_slotsData[t * 0x798 + s * 0x48]);
}

CTradeSpace::~CTradeSpace()
{
    sub_Mutex_D1(m_mutex);
}

// ---- 弱符号访问器族（ORIG 0x082a41f4 起）----
void CTradeSpace::SetIDX(int idx)
{
    m_index = idx;
}

void CTradeSpace::lock()
{
}

void CTradeSpace::unlock()
{
}

char CTradeSpace::IsLocked()
{
    return m_bLocked;
}

void CTradeSpace::_SetLock(bool flag)
{
    m_bLocked = flag ? 1 : 0;
}

char CTradeSpace::is_empty()
{
    return !m_bTrading;
}

int CTradeSpace::get_index()
{
    return m_index;
}

// ---- reset @ 0x085297f2 ----
void CTradeSpace::reset()
{
    lock();
    _SetLock(0);
    m_bTrading = 0;
    if (m_pTraders[0] != 0) {
        sub_CUser_SetTradeSpace(m_pTraders[0], -1);
        sub_cUserHistoryLog_SetTrader((char*)m_pTraders[0] + 0x79700, "", "");
    }
    if (m_pTraders[1] != 0) {
        sub_CUser_SetTradeSpace(m_pTraders[1], -1);
        sub_cUserHistoryLog_SetTrader((char*)m_pTraders[1] + 0x79700, "", "");
    }
    m_nMoney[0] = 0;
    m_nMoney[1] = 0;
    m_nState[0] = 2;
    m_nState[1] = 2;
    memset(&m_slotsData[0], 0, 0x798);
    memset(&m_slotsData[0x798], 0, 0x798);
    memset(m_pad, 0, sizeof(m_pad));
    unlock();
}

// ---- cancel_trade_by_dis @ 0x0852f086 ----
void CTradeSpace::cancel_trade_by_dis(CUser* user)
{
    cancel_trade_for_china(user);
}

// ---- GetOppositeUser @ 0x08529a72 ----
CUser* CTradeSpace::GetOppositeUser(CUser* user)
{
    if (!m_bTrading)
        return 0;
    if (m_pTraders[0] == user)
        return m_pTraders[1];
    return m_pTraders[0];
}

// ---- _CheckTrade4SameAvatar @ 0x08529d48 ----
int CTradeSpace::_CheckTrade4SameAvatar(const TradeSlot* slots,
                                        const Inven_Item& item)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (*(int*)((const char*)slots + i * 0x48 + 0xb) == *(int*)((const char*)&item + 2) &&
            *(int*)((const char*)slots + i * 0x48 + 0x10) == *(int*)((const char*)&item + 7) &&
            *(short*)((const char*)slots + i * 0x48 + 0x14) == *(short*)((const char*)&item + 0xb))
            return 1;
    }
    return 0;
}

// ---- check_item_exist @ 0x08529a18 ----
int CTradeSpace::check_item_exist(int traderIdx, int itemId)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (((TradeSlot*)&m_slotsData[traderIdx * 0x798])[i].m_item.m_addInfo == itemId)
            return i;
    }
    return -1;
}

// ---- get_empty_itemslot @ 0x085299c0 ----
int CTradeSpace::get_empty_itemslot(int traderIdx)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (((TradeSlot*)&m_slotsData[traderIdx * 0x798])[i].m_item.m_addInfo == 0)
            return i;
    }
    return -1;
}

// ---- check_trade_possibility @ 0x0852993c ----
int CTradeSpace::check_trade_possibility()
{
    if (m_bTrading == 1) {
        int i2 = m_pTraders[0]->GetTradeSpace();
        int i3 = m_pTraders[1]->GetTradeSpace();
        if (i2 == i3) {
            int s2 = m_pTraders[0]->get_state();
            int s3 = m_pTraders[1]->get_state();
            if (s2 == s3)
                return 1;
            return 0;
        }
        return 0;
    }
    return 0;
}

// ---- _IsTradable @ 0x08529dce ----
int CTradeSpace::_IsTradable(const Inven_Item& item)
{
    CDataManager* dm = G_CDataManager();
    CItem* ci = dm->find_item(*(int*)((const char*)&item + 2));
    if (ci == 0)
        return 0;
    if (item.m_upgradeSep.IsTradeRestriction())
        return 0;
    unsigned int t = (unsigned int)ci->GetAttachType();
    if (t < 6) {
        t = 1u << t;
        if ((t & 0x36) != 0)
            return 0;
        if (((t & 8) != 0) && (item.m_field0 != 1))
            return 0;
    }
    return 1;
}

// ---- _SaveAvatarItemOwnerChange @ 0x0853084a ----
void CTradeSpace::_SaveAvatarItemOwnerChange(unsigned int a, unsigned int b,
                                             unsigned int c, unsigned char d)
{
    sub_DB_AvatarChangeOwner_makeRequest(a, b, c, d);
}

// ---- add_item @ 0x08529aa2 ----
int CTradeSpace::add_item(int traderIdx, TradeSlot slot, int count)
{
    int ret;
    if (count < 0)
        return -1;
    ret = -1;
    lock();
    if (!slot.m_item.isEquipableItemType()) {
        int idx = check_item_exist(traderIdx, *(int*)((char*)&slot + 0x1b));
        if (idx == -1) {
            idx = get_empty_itemslot(traderIdx);
            if (idx != -1) {
                char* dst = &m_slotsData[traderIdx * 0x798 + idx * 0x48];
                if (dst != (char*)&slot)
                    memcpy(dst, &slot, 0x46);
                ((TradeSlot*)dst)->m_item.set_add_info(count);
                if (999 < count) {
                    cMyTrace tr("int CTradeSpace::add_item(int, TradeSlot, int)", 0xde, 5);
                    tr("CTradeSpace::add_item, ITEM #%d, %d",
                       *(int*)(dst + 0x3b), count);
                }
            }
        } else {
            char* dst = &m_slotsData[traderIdx * 0x798 + idx * 0x48];
            int newCount = *(int*)(dst + 0x40) + count;
            ((TradeSlot*)dst)->m_item.set_add_info(newCount);
            if (999 < newCount) {
                cMyTrace tr("int CTradeSpace::add_item(int, TradeSlot, int)", 0xd1, 5);
                tr("CTradeSpace::add_item, ITEM #%d, %d",
                   *(int*)(dst + 0x3b), newCount);
            }
        }
        ret = idx;
    } else {
        int idx = get_empty_itemslot(traderIdx);
        if (idx != -1) {
            char* dst = &m_slotsData[traderIdx * 0x798 + idx * 0x48];
            if (dst != (char*)&slot)
                memcpy(dst, &slot, 0x46);
        }
        ret = idx;
    }
    unlock();
    return ret;
}

// ---- set_traders @ 0x085295f0 ----
void CTradeSpace::set_traders(CUser* a, CUser* b)
{
    lock();
    m_bTrading = 1;
    _SetLock(0);
    m_pTraders[0] = a;
    *(unsigned int*)((char*)m_pTraders[0] + 0x8d25c) =
        (unsigned int)b->get_unique_id() & 0xffff;
    m_pTraders[1] = b;
    *(unsigned int*)((char*)m_pTraders[1] + 0x8d25c) =
        (unsigned int)a->get_unique_id() & 0xffff;
    bool ok = (m_pTraders[1]->getCurCharacR() != 0) &&
              (m_pTraders[0]->getCurCharacR() != 0);
    if (ok) {
        const char* name1 = m_pTraders[1]->getCurCharacName();
        const char* acc1 = m_pTraders[1]->get_acc_name();
        sub_cUserHistoryLog_SetTrader((char*)m_pTraders[0] + 0x79700, acc1, name1);
        const char* name0 = m_pTraders[0]->getCurCharacName();
        const char* acc0 = m_pTraders[0]->get_acc_name();
        sub_cUserHistoryLog_SetTrader((char*)m_pTraders[1] + 0x79700, acc0, name0);
    }
    sub_CUser_SetTradeSpace(a, m_index);
    sub_CUser_SetTradeSpace(b, m_index);
    m_nMoney[0] = 0;
    m_nMoney[1] = 0;
    m_nState[0] = 0;
    m_nState[1] = 0;
    memset(&m_slotsData[0], 0, 0x798);
    memset(&m_slotsData[0x798], 0, 0x798);
    memset(m_pad, 0, sizeof(m_pad));
    unlock();
    sub_cUserHistoryLog_TradeBegin((char*)a + 0x79700);
    sub_cUserHistoryLog_TradeBegin((char*)b + 0x79700);
}

// ---- cancel_trade @ 0x0852effa ----
int CTradeSpace::cancel_trade()
{
    bool fail = false;
    if (m_pTraders[0]->getCurCharacR() != 0) {
        if (m_pTraders[1]->getCurCharacR() != 0)
            fail = false;
        else
            fail = true;
    } else {
        fail = true;
    }
    if (fail) {
        cMyTrace tr("bool CTradeSpace::cancel_trade()", 0x788, 5);
        tr("CTradeSpace::cancel_trade()\t0 == m_pTraders[0]->getCurCharacR() || m_pTraders[1]->getCurCharacR()");
        return 0;
    }
    return cancel_trade_for_china(0);
}

// ---- send_lose_item_for_china @ 0x0852e3a8 ----
int CTradeSpace::send_lose_item_for_china(CUser* user, Inven_Item& item)
{
    char title[41];
    char msg[0xff];
    if (*(int*)((char*)&item + 2) == 0)
        return 0;
    {
        char* q = title;
        int n = 0x1d;
        if (((unsigned int)q & 1) != 0) {
            *q = 0;
            ++q;
            --n;
        }
        if (((unsigned int)q & 2) != 0) {
            *(short*)q = 0;
            q += 2;
            n -= 2;
        }
        unsigned int m = n & 0xfffffffc;
        unsigned int k = 0;
        do {
            *(int*)(q + k) = 0;
            k += 4;
        } while (k < m);
        q += k;
        if ((n & 2) != 0) {
            *(short*)q = 0;
            q += 2;
        }
        if ((n & 1) != 0)
            *q = 0;
    }
    {
        char* q = msg;
        for (int i = 0x3f; i != 0; --i) {
            q[0] = 0;
            q[1] = 0;
            q[2] = 0;
            q[3] = 0;
            q += 4;
        }
        q[0] = 0;
        q[1] = 0;
        q[2] = 0;
    }
    const char* s1 = g_scriptStringManager_.findString(4, "game_server_msg_287", 0);
    strncpy(title, s1, 0x1d);
    const char* s2 = g_scriptStringManager_.findString(4, "game_server_msg_288", 0);
    strncpy(msg, s2, 0xff);
    ENUM_SERVER_GROUP group = (ENUM_SERVER_GROUP)sub_CUser_GetServerGroup(user);
    size_t len = strlen(msg);
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    sub_CMailBoxHelper_ReqDBSendNewSystemMail(
        title, item, 0, characNo, msg, (int)len, 0, group, 0, 0);
    return 1;
}

// ============================================================================
// G4 交易域补齐：checkTrade / checkCancelTrade / change_trade_state /
// cancel_trade_for_china / proceed_trade / regist_item / remove_item
// （真实语义；跨类调用以 ORIG 符号为准）
// ============================================================================

// ---- 槽位辅助 ----
static inline TradeSlot* TradeSlotAt(char* slots, int trader, int idx)
{
    return (TradeSlot*)&slots[trader * 0x798 + idx * 0x48];
}

// ---- checkTrade @ 0x08530210 ----
// 校验交易可完成：双方物品/金钱都能进入对方背包。
int CTradeSpace::checkTrade()
{
    if (!m_bTrading)
        return 0;
    if (m_pTraders[0] == 0 || m_pTraders[1] == 0)
        return 0;
    if (m_pTraders[0]->getCurCharacR() == 0 || m_pTraders[1]->getCurCharacR() == 0)
        return 0;
    for (int trader = 0; trader < 2; ++trader) {
        CInventory* dst =
            (CInventory*)m_pTraders[1 - trader]->getCurCharacInvenW();
        int need = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            TradeSlot* slot = TradeSlotAt(m_slotsData, trader, s);
            if (slot->m_item.m_addInfo != 0)
                need += 1;
        }
        if (m_nMoney[trader] > 0)
            need += 1;
        if (dst->QuickEmptyCount() < need)
            return 0;
    }
    return 1;
}

// ---- checkCancelTrade @ 0x0852dbb8 ----
// 校验取消交易可行：双方注册物品/金钱都能退回各自背包。
int CTradeSpace::checkCancelTrade()
{
    if (m_pTraders[0]->getCurCharacR() == 0 ||
        m_pTraders[1]->getCurCharacR() == 0) {
        cMyTrace tr("bool CTradeSpace::checkCancelTrade()", 0x674, 5);
        tr("CTradeSpace::cancel_trade()\t0 == m_pTraders[0]->getCurCharacR() || m_pTraders[1]->getCurCharacR()");
        return 0;
    }
    for (int trader = 0; trader < 2; ++trader) {
        CInventory* inv = (CInventory*)m_pTraders[trader]->getCurCharacInvenW();
        int need = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            TradeSlot* slot = TradeSlotAt(m_slotsData, trader, s);
            if (slot->m_item.m_addInfo != 0)
                need += 1;
        }
        if (m_nMoney[trader] > 0)
            need += 1;
        if (inv->QuickEmptyCount() < need)
            return 0;
    }
    return 1;
}

// ---- cancel_trade_for_china @ 0x0852e544 ----
// 取消交易：金钱退回各自背包，注册物品按类型插回，失败走邮件补偿。
int CTradeSpace::cancel_trade_for_china(CUser* user)
{
    for (int trader = 0; trader < 2; ++trader) {
        CUser* owner = m_pTraders[trader];
        if (owner == 0)
            continue;
        CInventory* inv = (CInventory*)owner->getCurCharacInvenW();
        if (m_nMoney[trader] >= 1) {
            inv->gain_money(m_nMoney[trader], (eMoneyAddReason)2, 1, 0);
            m_nMoney[trader] = 0;
        }
        for (int s = 3; s <= 0x1a; ++s) {
            TradeSlot* slot = TradeSlotAt(m_slotsData, trader, s);
            if (slot->m_item.m_addInfo != 0) {
                Inven_Item item = slot->m_item;
                int ret = -1;
                if (slot->m_item.m_field1 == 1) {
                    ret = inv->insertItemIntoEquipment(item, (eItemAddReason)2, 1, 1);
                } else if (slot->m_item.isAvatarItemType()) {
                    ret = inv->insertAvatarIntoInventory(
                        item, (eAvatarItemAddReason)3, 1);
                } else if (slot->m_item.IsCreatureItemType()) {
                    ret = inv->insertItemIntoCreature(item, (eItemAddReason)0x1c, 1, 1);
                } else {
                    ret = inv->insertItemIntoInventory(item, (eItemAddReason)2, 1, 1);
                }
                if (ret < 0)
                    send_lose_item_for_china(owner, slot->m_item);
                slot->m_item.reset();
            }
        }
    }
    for (int trader = 0; trader < 2; ++trader) {
        CUser* owner = m_pTraders[trader];
        if (owner == 0)
            continue;
        if (owner != user) {
            // 通知对方交易已取消（物品清单回执）
            PacketGuard packet;
            InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
            buf->put_header(0, 0x10);
            buf->put_short(0);
            buf->finalize(true);
            owner->Send(packet);
        }
        sub_cUserHistoryLog_TradeEnd((char*)owner + 0x79700, 0, 0, 0, 0, 0);
    }
    return 1;
}

// ---- change_trade_state @ 0x0852f134 ----
int CTradeSpace::change_trade_state(CUser* user, int state)
{
    int me = (m_pTraders[0] == user) ? 0 : 1;
    int opp = 1 - me;
    CUser* target = m_pTraders[opp];
    if (target == 0) {
        PacketGuard packet;
        InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
        buf->put_header(1, 0x1a);
        buf->put_byte(0);
        buf->put_byte(1);
        buf->finalize(true);
        user->Send(packet);
        return 1;
    }
    switch (state) {
    case 0:  // 解除锁定
        if (!IsLocked()) {
            m_nState[me] = 0;
            PacketGuard packet;
            sub_SendChangeTradeState(packet, user, target, 0);
        } else {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7);
        }
        break;
    case 1:  // 锁定
        {
            unsigned int err = CSecu_ProtectionField::Check(
                GlobalData::s_pSecuProtectionField, user, (SECURITY_PROTCTION)0x36);
            if (err != 0) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, (unsigned char)err);
                unsigned char oppErr =
                    sub_CSecu_ProtectionField_GetOppositeErr(
                        GlobalData::s_pSecuProtectionField, err);
                target->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, oppErr);
                return 0;
            }
            if (IsLocked()) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7);
                break;
            }
            // 校验对方背包空间（真实语义：checkTrade 模拟插入）
            if (checkTrade()) {
                m_nState[me] = 1;
                PacketGuard packet;
                sub_SendChangeTradeState(packet, user, target, 1);
                if (m_nState[opp] == 1)
                    _SetLock(1);
            } else {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 4);
            }
        }
        break;
    case 2:  // 取消交易
        if (cancel_trade() == 0) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1);
        } else {
            void* mgr = sub_G_CGameManager();
            sub_CGameManager_PutTradeSpace(mgr, this);
        }
        break;
    case 3:  // 完成交易
        if (IsLocked() && checkTrade()) {
            // 可靠人 / 每日交易金币限制（ORIG 语义）
            unsigned int gold = m_nMoney[me] + m_nMoney[opp];
            if (gold > 0 && user->getCurCharacR() != 0 &&
                !user->IsAvailableCurCharacTradeGoldDaily(gold)) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 0x1f);
                break;
            }
            if (proceed_trade() == 0)
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1);
        } else {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1);
        }
        break;
    default:
        return 0;
    }
    return 1;
}

// ---- proceed_trade @ 0x0853087a ----
// 执行交易：双方注册物品/金钱互换，失败物品走邮件补偿。
int CTradeSpace::proceed_trade()
{
    if (!IsLocked())
        return 0;
    if (!checkTrade())
        return 0;
    for (int trader = 0; trader < 2; ++trader) {
        CUser* dst = m_pTraders[1 - trader];
        CInventory* dstInven = (CInventory*)dst->getCurCharacInvenW();
        if (m_nMoney[trader] > 0)
            dstInven->gain_money(m_nMoney[trader], (eMoneyAddReason)1, 1, 0);
        m_nMoney[trader] = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            TradeSlot* slot = TradeSlotAt(m_slotsData, trader, s);
            if (slot->m_item.m_addInfo != 0) {
                Inven_Item item = slot->m_item;
                int ret = -1;
                if (item.m_field1 == 1) {
                    ret = dstInven->insertItemIntoEquipment(item, (eItemAddReason)2, 1, 1);
                } else if (item.isAvatarItemType()) {
                    ret = dstInven->insertAvatarIntoInventory(
                        item, (eAvatarItemAddReason)1, 1);
                } else if (item.IsCreatureItemType()) {
                    ret = dstInven->insertItemIntoCreature(item, (eItemAddReason)0x1a, 1, 1);
                } else {
                    ret = dstInven->insertItemIntoInventory(item, (eItemAddReason)1, 1, 1);
                }
                if (ret < 0)
                    send_lose_item_for_china(dst, slot->m_item);
                slot->m_item.reset();
            }
        }
    }
    reset();
    return 1;
}

// ---- regist_item @ 0x08529e7e ----
// 注册物品到交易槽：按来源空间类型取物，校验后扣除并加入交易。
int CTradeSpace::regist_item(CUser* user, int type, int a, int b)
{
    int trader = (m_pTraders[0] == user) ? 0 : 1;
    if (m_pTraders[0] == 0 || m_pTraders[1] == 0)
        return -0x66;
    if (user->getCurCharacR() == 0 || m_pTraders[1 - trader]->getCurCharacR() == 0)
        return -0x66;
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    Inven_Item item;

    if (type == 3) {  // 装备栏
        if (a < 0 || a > 0x19)
            return -0xd;
        Inven_Item* src = inven->GetInvenRef(1, a);
        if (src == 0 || src->m_addInfo == 0)
            return -0xe;
        item = *src;
    } else if (type == 7) {  // 宠物栏
        if (b < 1)
            return -9;
        Inven_Item* src = inven->GetInvenRef(3, a);
        if (src == 0 || src->m_addInfo == 0)
            return -9;
        item = *src;
    } else {  // 背包 / 头像
        Inven_Item* src = inven->GetInvenRef(type, a);
        if (src == 0 || src->m_addInfo == 0)
            return -0x66;
        item = *src;
    }

    if (item.m_addInfo == 0)
        return -0x66;
    if (!_IsTradable(item))
        return -0x66;
    if (type != 3 && type != 7 && !item.isEquipableItemType()) {
        if (b < 1 || item.m_addInfo2 < b)
            return -8;
    }

    // 从来源扣除
    int delRet = -1;
    if (type == 3) {
        delRet = inven->delete_item((INVEN_TYPE)1, a, 1, (eItemDelReason)0x1b, 1);
    } else if (type == 7) {
        delRet = inven->delete_item((INVEN_TYPE)3, a, 1, (eItemDelReason)0x1b, 1);
    } else {
        if (item.isAvatarItemType())
            delRet = inven->delete_item((INVEN_TYPE)2, a, 1, (eItemDelReason)0x1b, 1);
        else
            delRet = inven->delete_item((INVEN_TYPE)0, a, b, (eItemDelReason)0x1b, 1);
    }
    if (delRet != 1) {
        sub_CUserHistoryLog_TradeItemAddFail((char*)user + 0x79700,
                                             item.m_addInfo, b);
        return -10;
    }

    TradeSlot slotItem;
    slotItem.m_field00 = type;
    slotItem.m_field04 = a;
    slotItem.m_field08 =
        (char)(type == 3 ? 3 : (type == 7 ? 7 : (item.isAvatarItemType() ? 1 : 0)));
    slotItem.m_item = item;
    slotItem.m_item.set_add_info(b);
    int slotNo = add_item(trader, slotItem, b);
    if (slotNo < 0) {
        // 回滚：物品插回背包
        int back = inven->insert_item_special_slot(item, 0, 0,
                                                   (eMoneyAddReason)0,
                                                   (eItemAddReason)0);
        if (back < 0)
            sub_CUserHistoryLog_TradeItemAddFail((char*)user + 0x79700,
                                                 item.m_addInfo, b);
        return -0xb;
    }
    m_nState[trader] = 0;
    m_nState[1 - trader] = 0;

    // 通知双方槽位更新
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0xf);
    buf->put_short(slotNo);
    buf->put_int(item.m_addInfo);
    buf->put_int(item.m_addInfo2);
    buf->put_byte(item.m_field0);
    buf->put_short(item.m_fieldb);
    buf->put_int(0);
    buf->put_byte(0);
    buf->put_short(0);
    buf->put_packet(item);
    buf->finalize(true);
    user->Send(packet);
    if (m_pTraders[1 - trader] != 0)
        m_pTraders[1 - trader]->Send(packet);
    return slotNo;
}

// ---- remove_item @ 0x0852bfcc ----
// 移除交易物品：从交易槽删除并返还玩家背包。
int CTradeSpace::remove_item(CUser* user, int a, int b, int c, int d)
{
    int trader = (m_pTraders[0] == user) ? 0 : 1;
    if (m_pTraders[0] == 0 || m_pTraders[1] == 0)
        return -1;
    if (a < 3 || a > 0x1a)
        return -1;
    TradeSlot* slot = TradeSlotAt(m_slotsData, trader, a);
    if (slot->m_item.m_addInfo == 0)
        return -1;

    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    Inven_Item item = slot->m_item;
    int ret = -1;
    if (item.m_field1 == 1) {
        ret = inven->insertItemIntoEquipment(item, (eItemAddReason)2, 1, 1);
    } else if (item.isAvatarItemType()) {
        ret = inven->insertAvatarIntoInventory(item, (eAvatarItemAddReason)0x14, 1);
    } else if (item.IsCreatureItemType()) {
        ret = inven->insertItemIntoCreature(item, (eItemAddReason)0x1d, 1, 1);
    } else {
        ret = inven->insertItemIntoInventory(item, (eItemAddReason)2, 1, 1);
    }
    if (ret < 0) {
        send_lose_item_for_china(user, slot->m_item);
        slot->m_item.reset();
        return -1;
    }
    slot->m_item.reset();
    m_nState[trader] = 0;
    m_nState[1 - trader] = 0;

    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0xf);
    buf->put_short(a);
    buf->put_int(item.m_addInfo);
    buf->put_int(item.m_addInfo2);
    buf->put_byte(item.m_field0);
    buf->put_short(item.m_fieldb);
    buf->put_int(0);
    buf->put_byte(0);
    buf->put_short(0);
    buf->put_packet(item);
    buf->finalize(true);
    user->Send(packet);
    if (m_pTraders[1 - trader] != 0)
        m_pTraders[1 - trader]->Send(packet);
    return ret;
}
