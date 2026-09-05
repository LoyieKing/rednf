#include "GameRand.h"
// ============================================================================
// df_game_r 还原 —— CTradeSpace（交易空间，G2-4 批次）
// 参照 docs/class_func_reports/CTradeSpace.md + ORIG 弱符号反汇编。
// 布局（ORIG CTradeSpace::CTradeSpace 0x082a416c / SetIDX 0x082a41f4 推导）：
//   +0x00 Mutex（0x18）          +0x18 int m_index
//   +0x1c char m_bTrading        +0x1d char m_bLocked
//   +0x20 CUser* m_pTraders[2]   +0x28 int m_nState[2]
//   +0x30 TradeSlot m_slots[2][27]（每槽 0x48，每行 0x798）
//   +0xf60 int m_nMoney[2]        +0xf68 int m_fieldF68[2]（生物转移计数）
//   （总尺寸 0xf70）
// 本 TU 只做 CTradeSpace 自身（含 TradeSlot 构造/Reset），跨类调用以真实符号
// 声明（头文件已有者直接使用；缺失者用 asm-label extern）。
// ============================================================================

#include "CUser.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "InterfacePacketBuf.h"
// 跨类符号：CUser/cUserHistoryLog(CUser.h)、CSecu_ProtectionField(CInventory.h)、
// Mutex(GameTypes.h)、CMailBoxHelper/G_CGameManager(CGameManager.h)、
// DB_AvatarChangeOwner(DB_AvatarChangeOwner.h)。
#include "CGameManager.h"
#include "DB_AvatarChangeOwner.h"
#include "CTradeSpace.h"   // 类/布局唯一声明点（CGameManager.h 亦 include）
#include "RDARScriptStringManager.h"   // RDARScriptStringManager/g_scriptStringManager_（CleanFAsm 新建）
#include "HistoryLog.h"            // HistoryLog::WriteTradeUp/Down（proceed_trade）
#include "CAuctionAveragePrice.h"  // CAuctionAveragePrice（proceed_trade 均价）
#include "CStreamGuard.h"          // CStreamGuard（零金币日志流）
#include "StreamPool.h"            // StreamPool::Acquire
#include "MsgQueueMgr.h"           // MsgQueueMgr::put
#include "GlobalData.h"            // s_stream_pool / s_msgq_mgr
#include "W6_HackPunishDeps.h"     // CAutoPunishRuleHackTypeMgr（checkTotalTradeRuleOver）
#include "SigTypes_E.h"            // SIG_SAVE_GOLD_ZERO_TRADE_LOG
#include "GameWorld.h"             // G_GameWorld/send_all（remove_item 广播）

extern Inven_Item g_emptySlot;   // ORIG 全局 0x943ddc0（Inven_Item.cpp 定义）

// DB_InsertCeraItemHistory（定义 DB_U3.cpp；本 TU 仅调用 makeRequest）
class DB_InsertCeraItemHistory
{
public:
    static int makeRequest(int itemIdx, unsigned int senderCharacNo,
                           unsigned int receiverCharacNo, char flag);
};

// ---- 交易状态枚举（ORIG ENUM_TRADESTATE；mangling 编码类型名 15ENUM_TRADESTATE）----
enum ENUM_TRADESTATE
{
    ENUM_TRADESTATE_0 = 0,  // 解除锁定
    ENUM_TRADESTATE_1 = 1,  // 锁定
    ENUM_TRADESTATE_2 = 2,
    ENUM_TRADESTATE_3 = 3,
    ENUM_TRADESTATE_4 = 4
};

// SendChangeTradeState（本 TU 实现，ORIG 0x852f0a0）
void SendChangeTradeState(PacketGuard& packet, CUser* user, CUser* target,
                          ENUM_TRADESTATE state)
{
    // ORIG 0x852f0a0：清空 → 头部(0, 0x11) → unique_id(short) → state(byte)
    // → finalize(true) → 双方各 Send 一次。
    packet.clear();
    packet.put_header(0, 0x11);
    packet.put_short(user->get_unique_id());
    packet.put_byte(state);
    packet.finalize(true);
    user->Send(packet);
    target->Send(packet);
}

// get_rand_int（CItemList.h 已有 C++ 链接声明）
int get_rand_int(int max);



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


// ---- 构造 / 析构（ORIG 0x082a416c / 0x082a41e0）----
// ORIG：m_bTrading(+0x1c) 直接初始化（在数组成员构造循环之前清零，
// 声明顺序 m_bTrading 先于 m_slotsData）；m_bLocked(+0x1d) 在 ctor 体末尾赋值。
CTradeSpace::CTradeSpace()
    : m_bTrading(false)
{
    m_bLocked = 0;
}
CTradeSpace::~CTradeSpace()
{
}

// ---- 槽位辅助（P49：-O0 下 static inline 也产真实符号 → 宏）----
// 基于真实二维数组成员索引（m_slotsData[2][27]，行步长 0x798、槽步长 0x48）。
#define TradeSlotAt(slots, trader, idx) (&((slots)[trader][idx]))

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
    m_bLocked = flag;
}

bool CTradeSpace::is_empty()   // CTradeSpace.h:67 bool（DCoreB 落，定义同步）
{
    return m_bTrading ^ 1;
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
        m_pTraders[0]->SetTradeSpace(-1);
        m_pTraders[0]->m_historyLog.SetTrader("", "");
    }
    if (m_pTraders[1] != 0) {
        m_pTraders[1]->SetTradeSpace(-1);
        m_pTraders[1]->m_historyLog.SetTrader("", "");
    }
    m_nMoney[0] = 0;
    m_nMoney[1] = 0;
    m_nState[0] = 2;
    m_nState[1] = 2;
    memset(&m_slotsData[0], 0, 0x798);
    memset(&m_slotsData[1], 0, 0x798);
    memset(m_fieldF68, 0, sizeof(m_fieldF68));
    unlock();
}

// ---- cancel_trade_by_dis @ 0x0852f086 ----
void CTradeSpace::cancel_trade_by_dis(CUser* user)
{
    cancel_trade_for_china(user);
}

CUser* CTradeSpace::GetOppositeUser(CUser* user)
{
    if (m_bTrading != 0) {
        if (m_pTraders[0] == user)
            return m_pTraders[1];
        else
            return m_pTraders[0];
    }
    return 0;
}

// ---- _CheckTrade4SameAvatar @ 0x08529d48 ----
int CTradeSpace::_CheckTrade4SameAvatar(const TradeSlot* slots,
                                        const Inven_Item& item)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (slots[i].m_item.m_addInfo == item.m_addInfo &&
            slots[i].m_item.m_addInfo2 == item.m_addInfo2 &&
            slots[i].m_item.m_fieldb == item.m_fieldb)
            return 1;
    }
    return 0;
}

// ---- check_item_exist @ 0x08529a18 ----
int CTradeSpace::check_item_exist(int traderIdx, int itemId)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (TradeSlotAt(m_slotsData, traderIdx, i)->m_item.m_addInfo == itemId)
            return i;
    }
    return -1;
}

int CTradeSpace::get_empty_itemslot(int traderIdx)
{
    for (int i = 3; i <= 0x1a; ++i) {
        if (TradeSlotAt(m_slotsData, traderIdx, i)->m_item.m_addInfo == 0)
            return i;
    }
    return -1;
}
bool CTradeSpace::check_trade_possibility()   // CTradeSpace.h:53 bool（DCoreB 落，定义同步；体仅 return 0/1）
{
    if (!m_bTrading)
        return 0;
    if (m_pTraders[0]->GetTradeSpace() != m_pTraders[1]->GetTradeSpace())
        return 0;
    if (m_pTraders[0]->get_state() != m_pTraders[1]->get_state())
        return 0;
    return 1;
}

// ---- _IsTradable @ 0x08529dce ----
int CTradeSpace::_IsTradable(const Inven_Item& item)
{
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    if (ci == 0)
        return 0;
    if (item.m_upgradeSep.IsTradeRestriction())
        return 0;
    unsigned int t = (unsigned int)ci->GetAttachType();
    if (t < 6) {
        unsigned int bit = 1u << t;
        if ((bit & 0x36) != 0)
            return 0;
        if ((bit & 8) != 0) {
            if (item.m_field0 == 1)
                return 1;
            return 0;
        }
    }
    return 1;
}

// ---- _SaveAvatarItemOwnerChange @ 0x0853084a ----
void CTradeSpace::_SaveAvatarItemOwnerChange(unsigned int a, unsigned int b,
                                             unsigned int c, unsigned char d)
{
    DB_AvatarChangeOwner::makeRequest(a, b, c, d);
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
        int idx = check_item_exist(traderIdx, slot.m_item.m_addInfo);
        if (idx == -1) {
            idx = get_empty_itemslot(traderIdx);
            if (idx != -1) {
                TradeSlot* dst = TradeSlotAt(m_slotsData, traderIdx, idx);
                if (dst != &slot)
                    memcpy(dst, &slot, 0x46);
                dst->m_item.set_add_info(count);
                if (999 < count) {
                    cMyTrace tr(__PRETTY_FUNCTION__, 0xde, 5);
                    tr("CTradeSpace::add_item, ITEM #%d, %d",
                       dst->m_item.m_addInfo, count);
                }
            }
        } else {
            TradeSlot* dst = TradeSlotAt(m_slotsData, traderIdx, idx);
            int newCount = dst->m_item.m_addInfo2 + count;
            dst->m_item.set_add_info(newCount);
            if (999 < newCount) {
                cMyTrace tr(__PRETTY_FUNCTION__, 0xd1, 5);
                tr("CTradeSpace::add_item, ITEM #%d, %d",
                   dst->m_item.m_addInfo, newCount);
            }
        }
        ret = idx;
    } else {
        int idx = get_empty_itemslot(traderIdx);
        if (idx != -1) {
            TradeSlot* dst = TradeSlotAt(m_slotsData, traderIdx, idx);
            if (dst != &slot)
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
    m_pTraders[0]->m_padf = (unsigned short)b->get_unique_id();
    m_pTraders[1] = b;
    m_pTraders[1]->m_padf = (unsigned short)a->get_unique_id();
    bool ok = (m_pTraders[1]->getCurCharacR() != 0) &&
              (m_pTraders[0]->getCurCharacR() != 0);
    if (ok) {
        const char* name1 = m_pTraders[1]->getCurCharacName();
        const char* acc1 = m_pTraders[1]->get_acc_name();
        m_pTraders[0]->m_historyLog.SetTrader(acc1, name1);
        const char* name0 = m_pTraders[0]->getCurCharacName();
        const char* acc0 = m_pTraders[0]->get_acc_name();
        m_pTraders[1]->m_historyLog.SetTrader(acc0, name0);
    }
    a->SetTradeSpace(m_index);
    b->SetTradeSpace(m_index);
    m_nMoney[0] = 0;
    m_nMoney[1] = 0;
    m_nState[0] = 0;
    m_nState[1] = 0;
    memset(&m_slotsData[0], 0, 0x798);
    memset(&m_slotsData[1], 0, 0x798);
    memset(m_fieldF68, 0, sizeof(m_fieldF68));
    unlock();
    a->m_historyLog.TradeBegin();
    b->m_historyLog.TradeBegin();
}

// ---- cancel_trade @ 0x0852effa ----
int CTradeSpace::cancel_trade()
{
    if (m_pTraders[0]->getCurCharacR() == 0 ||
        m_pTraders[1]->getCurCharacR() == 0) {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x788, 5);
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
    if (item.m_addInfo == 0)
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
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    size_t len = strlen(msg);
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, 0, characNo, msg, (int)len, 0, group, false, false);
    return 1;
}

// ============================================================================
// G4 交易域补齐：checkTrade / checkCancelTrade / change_trade_state /
// cancel_trade_for_china / proceed_trade / regist_item / remove_item
// （真实语义；跨类调用以 ORIG 符号为准；槽位访问统一走文件顶部 TradeSlotAt）
// ============================================================================

// ---- checkTrade @ 0x08530210 ----
// 校验交易可完成：对双方背包做副本，模拟把对方注册物品/金钱插入各自副本，
// 任一插入失败即整体不可完成（返回 0）。
int CTradeSpace::checkTrade()
{
    CInventory copy[2];
    char mem[2][0xc140];
    memset(mem, 0, sizeof(mem));
    copy[0].SetInventoryMemory(reinterpret_cast<InventoryMemory*>(mem[0]));
    copy[1].SetInventoryMemory(reinterpret_cast<InventoryMemory*>(mem[1]));
    // ORIG 经 getCurCharacR()+0xda 纯读取背包（无 enableSaveInven 副作用）
    copy[0].setCopy(*m_pTraders[0]->getCurCharacInvenR());
    copy[1].setCopy(*m_pTraders[1]->getCurCharacInvenR());
    for (int trader = 0; trader < 2; ++trader) {
        CInventory& dst = copy[1 - trader];
        if (m_nMoney[trader] > 0 && m_pTraders[1 - trader] != 0) {
            if (dst.gain_money(m_nMoney[trader], (eMoneyAddReason)1, 0, 0) !=
                m_nMoney[trader])
                return 0;
        }
        for (int s = 3; s <= 0x1a; ++s) {
            TradeSlot* slot = TradeSlotAt(m_slotsData, trader, s);
            if (slot->m_item.m_addInfo == 0)
                continue;
            int ret;
            Inven_Item item = slot->m_item;
            if (slot->m_item.m_field1 == (unsigned char)8) {
                ret = dst.insertAvatarIntoInventory(item, (eAvatarItemAddReason)1, 0);
            } else if (slot->m_field08 == 7) {
                ret = dst.insertItemIntoCreature(item, (eItemAddReason)0x1a, 0, 1);
            } else {
                ret = dst.insertItemIntoInventory(item, (eItemAddReason)1, 0, 1);
            }
            if (ret < 0) {
                if (slot->m_item.m_field1 == (unsigned char)8) {
                    cMyTrace tr(__PRETTY_FUNCTION__, 0xba7, 5);
                    tr("try insert_item(INVEN_TYPE_AVATAR) fail - fatal! ch=%d",
                       m_pTraders[1 - trader]->getCurCharacNo());
                } else {
                    cMyTrace tr(__PRETTY_FUNCTION__, 0xbab, 5);
                    tr("try insert_item(INVEN_TYPE_INVENTORY) fail - fatal! ch=%d",
                       m_pTraders[1 - trader]->getCurCharacNo());
                }
                return 0;
            }
        }
    }
    return 1;
}

// ---- checkCancelTrade @ 0x0852dbb8 ----
// 校验取消交易可行：对双方背包做副本，模拟把各交易者自己的注册物品/金钱
// 插回其自身副本，任一插入失败即整体不可取消（返回 0）。
int CTradeSpace::checkCancelTrade()
{
    if (m_pTraders[0]->getCurCharacR() == 0 || m_pTraders[1]->getCurCharacR() == 0) {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x674, 5);
        tr("CTradeSpace::cancel_trade()\t0 == m_pTraders[0]->getCurCharacR() || m_pTraders[1]->getCurCharacR()");
        return 0;
    }
    CInventory copy[2];
    char mem[2][0xc140];
    memset(mem, 0, sizeof(mem));
    copy[0].SetInventoryMemory(reinterpret_cast<InventoryMemory*>(mem[0]));
    copy[1].SetInventoryMemory(reinterpret_cast<InventoryMemory*>(mem[1]));
    // ORIG 经 getCurCharacR()+0xda 纯读取背包（无 enableSaveInven 副作用）
    copy[0].setCopy(*m_pTraders[0]->getCurCharacInvenR());
    copy[1].setCopy(*m_pTraders[1]->getCurCharacInvenR());
    for (int trader = 0; trader < 2; ++trader) {
        CInventory& own = copy[trader];
        if (m_nMoney[trader] > 0 && m_pTraders[trader] != 0)
            own.gain_money(m_nMoney[trader], (eMoneyAddReason)2, 0, 0);
        for (int s = 3; s <= 0x1a; ++s) {
            TradeSlot* slot = TradeSlotAt(m_slotsData, trader, s);
            if (slot->m_item.m_addInfo == 0)
                continue;
            int ret;
            Inven_Item item = slot->m_item;
            if (slot->m_field08 == 0) {
                ret = own.insertItemIntoInventory(item, (eItemAddReason)2, 0, 1);
            } else if (slot->m_field08 == 1) {
                ret = own.insertAvatarIntoInventory(item, (eAvatarItemAddReason)3, 0);
            } else if (slot->m_field08 == 7) {
                ret = own.insertItemIntoCreature(item, (eItemAddReason)0x1c, 0, 1);
            } else {
                ret = own.insertItemIntoEquipment(item, (eItemAddReason)2, 0, 1);
            }
            if (ret < 0) {
                cMyTrace tr(__PRETTY_FUNCTION__, 0x6af, 5);
                tr("try insert_item(INVEN_TYPE_INVENTORY) fail - [Fail]Insert_item - fatal! canceled ch=%d",
                   m_pTraders[trader]->getCurCharacNo());
                return 0;
            }
        }
    }
    return 1;
}

// ---- cancel_trade_for_china @ 0x0852e544 ----
// 取消交易：金钱退回各自背包，注册物品按 TradeSlot.m_field08 分类插回
// （0=背包 / 1=头像 / 7=宠物 / 其它=装备），失败走邮件补偿；
// 构建取消回执包（header 0x10）发给非触发方交易者。
int CTradeSpace::cancel_trade_for_china(CUser* user)
{
    PacketGuard packet[2];
    for (int trader = 0; trader < 2; ++trader) {
        CUser* owner = m_pTraders[trader];
        if (owner == 0)
            continue;
        CInventory* inv = (CInventory*)owner->getCurCharacInvenW();
        InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet[trader];
        buf->clear();
        buf->put_header(0, 0x10);
        int cnt = 0;
        for (int s = 3; s <= 0x1a; ++s)
            if (TradeSlotAt(m_slotsData, trader, s)->m_item.m_addInfo != 0)
                cnt += 1;
        int countIdx = -1;  // ORIG local_60：count 字段占位（get_index 取写入偏移）
        int done = 0;       // ORIG local_2c：实际写回的条目数（金币+已处理槽）
        if (m_nMoney[trader] < 1) {
            countIdx = buf->get_index();
            buf->put_short(cnt);
        } else {
            cnt += 1;
            if (owner->getCurCharacR() == 0) {
                cMyTrace tr(__PRETTY_FUNCTION__, 0x71c, 5);
                tr("CTradeSpace::cancel_trade()\tm_pTraders[user_index]->getCurCharacR()");
                return 0;
            }
            inv->gain_money(m_nMoney[trader], (eMoneyAddReason)2, 1, 0);
            m_nMoney[trader] = 0;
            done += 1;  // ORIG 0x852e74f：金币条目计入实际数
            countIdx = buf->get_index();
            buf->put_short(cnt);
            buf->put_short(0);
            buf->put_byte(0);
            buf->put_short(0);
        }
        for (int s = 3; s <= 0x1a; ++s) {
            TradeSlot* slot = TradeSlotAt(m_slotsData, trader, s);
            // ORIG 条件：owner!=0 && owner->getCurCharacR()!=0 && 槽非空 才处理
            if (slot->m_item.m_addInfo == 0)
                continue;
            if (owner->getCurCharacR() == 0)
                continue;
            buf->put_short(s);
            Inven_Item item = slot->m_item;
            int ret = -1;
            unsigned char kind = (unsigned char)slot->m_field08;
            if (kind == 1) {
                ret = inv->insertAvatarIntoInventory(item, (eAvatarItemAddReason)3, 1);
                if (ret >= 0 && slot->m_field00 != ret)
                    inv->SendAvatarEvent(0x32, slot->m_field00 + 10, ret + 10,
                                         *(int*)((char*)slot + 0x40), 0);
                buf->put_byte(1);
            } else if (kind == 7) {
                ret = inv->insertItemIntoCreature(item, (eItemAddReason)0x1c, 1, 1);
                if (ret >= 0 && slot->m_field00 != ret)
                    inv->GetCreatureMgrW()->ChangeSlotNo(&slot->m_item, ret, 0x1c);
                buf->put_byte(7);
            } else if (kind == 0) {
                ret = inv->insertItemIntoInventory(item, (eItemAddReason)2, 1, 1);
                buf->put_byte(0);
            } else {
                ret = inv->insertItemIntoEquipment(item, (eItemAddReason)2, 1, 1);
                buf->put_byte(3);
            }
            if (ret < 0)
                send_lose_item_for_china(owner, slot->m_item);
            slot->m_item.reset();
            buf->put_short(ret);
            done += 1;
        }
        // ORIG 0x852ee57：槽循环后以实际条目数回填 count 占位
        buf->put_short(countIdx, done);
        buf->finalize(true);
    }
    for (int trader = 0; trader < 2; ++trader) {
        CUser* owner = m_pTraders[trader];
        if (owner == 0)
            continue;
        if (owner != user) {
            owner->Send(packet[trader]);
            owner->send_equip(0);
        }
        owner->m_historyLog.TradeEnd((eTradeEndReason)0, 0, 0, 0, 0);
    }
    return 1;
}

// ---- change_trade_state @ 0x0852f134 ----
int CTradeSpace::change_trade_state(CUser* user, int state)
{
    PacketGuard packet;  // ORIG 入口即构造（-0xc0），各分支复用
    int me, opp;
    CUser* target = 0;
    if (m_pTraders[0] == user) {
        target = m_pTraders[1];
        opp = 1;
        me = 0;
    } else {
        target = m_pTraders[0];
        opp = 0;
        me = 1;
    }
    if (target == 0) {
        ((InterfacePacketBuf*)&packet)->clear();
        ((InterfacePacketBuf*)&packet)->put_header(1, 0x1a);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_byte(1);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        user->Send(packet);
        return 1;
    }
    switch (state) {
    case 2:  // 取消交易
        if (cancel_trade()) {
            G_CGameManager()->PutTradeSpace(this);
        } else {
            cMyTrace(__PRETTY_FUNCTION__, 0x9db, 5)(
                "change_trade_state, cancel_trade Failed 1!  %d %d",
                user->getCurCharacNo(), target->getCurCharacNo());
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1, packet);
        }
        {
            // 取消后清点：若槽位仍有剩余物品且双方在场 → 逐项打日志
            int count = 0;
            for (int trader = 0; trader <= 1; ++trader)
                for (int slot = 3; slot <= 0x1a; ++slot)
                    if (TradeSlotAt(m_slotsData, trader, slot)->m_item.m_addInfo != 0)
                        ++count;
            if (count > 0 && m_pTraders[0] != 0 && m_pTraders[1] != 0) {
                cMyTrace(__PRETTY_FUNCTION__, 0x9eb, 5)(
                    "TRADE CANCELED! BUT, ITEM REMAINED! %d Count , Ch1 %d Ch2 %d",
                    count, m_pTraders[0]->get_charac_no(-1),
                    m_pTraders[1]->get_charac_no(-1));
                for (int trader = 0; trader <= 1; ++trader)
                    for (int slot = 3; slot <= 0x1a; ++slot) {
                        if (m_pTraders[trader] == 0)
                            continue;
                        TradeSlot* ts = TradeSlotAt(m_slotsData, trader, slot);
                        if (ts->m_item.m_addInfo == 0)
                            continue;
                        cMyTrace(__PRETTY_FUNCTION__, 0x9f8, 5)(
                            "Remained Item %d : %d x %d.",
                            m_pTraders[trader]->get_charac_no(-1),
                            ts->m_item.m_addInfo, ts->m_item.get_add_info());
                    }
            }
        }
        break;
    case 0:  // 解除锁定
        if (IsLocked()) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
        } else {
            m_nState[me] = 0;
            SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_0);
        }
        break;
    case 1:  // 锁定
        {
            unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x36);
            if (err != 0) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, (unsigned char)err);
                unsigned char oppErr =
                    GlobalData::s_pSecuProtectionField->GetOppositeErr(err);
                target->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, oppErr);
                if (err == 0x7b || err == 0x89) {
                    // 安全异常：给对端补发 0xb0 强退提示包
                    ((InterfacePacketBuf*)&packet)->clear();
                    ((InterfacePacketBuf*)&packet)->put_header(0, 0xb0);
                    ((InterfacePacketBuf*)&packet)->put_byte(1);
                    ((InterfacePacketBuf*)&packet)->finalize(true);
                    target->Send(packet);
                }
                return 0;
            }
            if (IsLocked()) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
                break;
            }
            // 对方背包空间校验（ORIG 内联：QuickEmptyCount + 分类型空格统计，
            // 非 checkTrade 的模拟插入）：
            // 遍历 me 的槽位，装备总是占格；非装备仅当对方背包中不存在该物品时占格。
            // 按 ITEM_TYPE(1..10) 累计需求，与对方各类型空格比较，缺口从快捷栏空格抵扣。
            int needCnt[0xb] = {0};
            int emptyCnt[0xb] = {0};
            int quickEmpty = target->getCurCharacInvenR()->QuickEmptyCount();
            for (int slot = 3; slot <= 0x1a; ++slot) {
                TradeSlot* ts = TradeSlotAt(m_slotsData, me, slot);
                if (ts->m_item.m_addInfo == 0)
                    continue;
                if (!ts->m_item.isEquipableItemType()) {
                    if (target->getCurCharacInvenR()->check_item_exist(
                            ts->m_item.m_addInfo) < 0)
                        needCnt[ts->m_item.m_field1]++;
                } else {
                    needCnt[ts->m_item.m_field1]++;
                }
            }
            for (int type = 1; type <= 10; ++type) {
                emptyCnt[type] = target->getCurCharacInvenR()->InvenEmptycount(
                    (Inven_Item::ITEM_TYPE)type);
                if (needCnt[type] > emptyCnt[type]) {
                    quickEmpty += emptyCnt[type] - needCnt[type];
                    if (quickEmpty < 0) {
                        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 4, packet);
                        return 0;
                    }
                }
            }
            m_nState[me] = 1;
            SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_1);
            if (m_nState[opp] == 1)
                _SetLock(1);
        }
        break;
    case 3:  // 完成交易
        {
            int goldMe = m_nMoney[me];
            int goldOpp = m_nMoney[opp];
            if (user != 0 && !user->GetReliablePerson() &&
                !user->IsAvailableCurCharacTradeGoldDaily(goldMe)) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 0x46, packet);
                return 0;
            }
            if (target != 0 && !target->GetReliablePerson() &&
                !target->IsAvailableCurCharacTradeGoldDaily(goldOpp)) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 0x47, packet);
                return 0;
            }
            if (m_nState[me] != 1) {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
                return 0;
            }
            m_nState[me] = 3;
            if (m_nState[opp] == 3) {
                bool bOppHasLow = false;  // 对方提交了低稀有度(<=1)非生物物品
                bool bMeHasLow = false;
                if (user != 0) {
                    for (int slot = 3; slot <= 0x1a; ++slot) {
                        TradeSlot* ts = TradeSlotAt(m_slotsData, opp, slot);
                        if (ts->m_item.m_addInfo == 0)
                            continue;
                        CItem* item =
                            G_CDataManager()->find_item(ts->m_item.m_addInfo);
                        if (item != 0 && !item->IsCreatureItem() &&
                            item->get_rarity() <= 1) {
                            bOppHasLow = true;
                            break;
                        }
                    }
                }
                if (target != 0) {
                    for (int slot = 3; slot <= 0x1a; ++slot) {
                        TradeSlot* ts = TradeSlotAt(m_slotsData, me, slot);
                        if (ts->m_item.m_addInfo == 0)
                            continue;
                        CItem* item =
                            G_CDataManager()->find_item(ts->m_item.m_addInfo);
                        if (item != 0 && !item->IsCreatureItem() &&
                            item->get_rarity() <= 1) {
                            bMeHasLow = true;
                            break;
                        }
                    }
                }
                if (proceed_trade()) {
                    G_CGameManager()->PutTradeSpace(this);
                    if (user != 0)
                        user->IncrementTradeGold(goldMe, bOppHasLow);
                    if (target != 0)
                        target->IncrementTradeGold(goldOpp, bMeHasLow);
                } else {
                    cMyTrace(__PRETTY_FUNCTION__, 0xaed, 5)(
                        "change_trade_state, proceed_trade Failed2! %d %d",
                        user->getCurCharacNo(), target->getCurCharacNo());
                    m_nState[me] = 0;
                    m_nState[opp] = 0;
                    SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_0);
                    SendChangeTradeState(packet, target, user, ENUM_TRADESTATE_0);
                }
                // 防 hack：金币倒卖概率检测（ORIG 使用“金额大于浮点阈值”，不是相等判断）。
                unsigned short fatigueMe = user->getCurCharacUsedFatigue();
                unsigned short fatigueOpp = target->getCurCharacUsedFatigue();
                int r1 = get_rand_int(0x19);
                int r2 = get_rand_int(0x14);
                // ORIG 常量：_DAT_08c98630=0.01、_DAT_08c98638=0.7；
                // fucompp + 条件码表示 goldMe > threshold。
                int totalMoney =
                    user->getCurCharacInvenR()->get_money() + goldMe;
                if ((double)goldMe >
                        (0.7 + 0.01 * (double)r1) * (double)totalMoney &&
                    goldMe > 0x270f) {
                    int etc = user->SetETC(0x2c0, target->get_acc_id());
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0x2c0, 1,
                        (unsigned)etc, 0);
                    if (fatigueMe >= r2 + 0x78) {
                        etc = user->SetETC(0x2bd, target->get_acc_id());
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0x2bd, 1,
                            (unsigned)etc, 0);
                    }
                    etc = target->SetETC(0x2c1, user->get_acc_id());
                    target->getHackAnalyzer()->addServerHackCnt(
                        target, (WongWork::ENUM_HACKTYPE)0x2c1, 1,
                        (unsigned)etc, 0);
                }
                // ORIG 常量：_DAT_08c98640=0.9；同样是严格大于阈值。
                totalMoney =
                    target->getCurCharacInvenR()->get_money() + goldOpp;
                if ((double)goldOpp > 0.9 * (double)totalMoney &&
                    goldOpp > 0x270f) {
                    int etc = target->SetETC(0x2c0, user->get_acc_id());
                    target->getHackAnalyzer()->addServerHackCnt(
                        target, (WongWork::ENUM_HACKTYPE)0x2c0, 1,
                        (unsigned)etc, 0);
                    if (fatigueOpp > 0x77) {
                        etc = target->SetETC(0x2bd, user->get_acc_id());
                        target->getHackAnalyzer()->addServerHackCnt(
                            target, (WongWork::ENUM_HACKTYPE)0x2bd, 1,
                            (unsigned)etc, 0);
                    }
                    etc = user->SetETC(0x2c1, target->get_acc_id());
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0x2c1, 1,
                        (unsigned)etc, 0);
                }
            }
        }
        break;
    case 4:  // 双方确认后最终完成：解锁并清状态
        if (IsLocked()) {
            m_nState[me] = 0;
            m_nState[opp] = 0;
            SendChangeTradeState(packet, user, target, ENUM_TRADESTATE_0);
            SendChangeTradeState(packet, target, user, ENUM_TRADESTATE_0);
            _SetLock(false);
        } else {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 7, packet);
        }
        break;
    default:
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1, packet);
        return 1;
    }
    return 0;
}

// 执行交易（ORIG 完整转写）：防 hack 金币/头像上报 → 双方金币/物品互换 →
// 零金币日志 → 物品转移（头像/生物/普通三分支）→ 发包 → 更新列表 → 收尾日志。
// 失败物品不回补（ORIG 直接 reset 槽位并返回 0）。
int CTradeSpace::proceed_trade()
{
    if (!IsLocked())
        return 0;
    if (!checkTrade())
        return 0;

    PacketGuard packet[2];                        // -0xa8 起 2 个
    std::map<CUser*, std::vector<int> > updateMap;  // -0xc4
    int avgPrice[2] = {0, 0};                     // -0x90/-0x8c（拍卖均价累计）
    int tradeLogMoney[2] = {0, 0};                // -0x88/-0x84（TradeEnd 用，恒 0）

    // ---- 第一段：防 hack 上报（trader 0..1）----
    for (int trader = 0; trader <= 1; ++trader)
    {
        CUser* src = m_pTraders[trader];          // -0x60
        CUser* dst = m_pTraders[1 - trader];      // -0x5c
        char ip[16];
        ((XNuclear::CHades*)dst->getHades())->GetIP(ip);
        int money = m_nMoney[trader];             // +0xf60
        int srcMoney = src->getCurCharacInvenR()->get_money();
        int accId = dst->get_acc_id();
        src->getAcccountHackInfo()->sendGold(0, (unsigned)accId, (unsigned)money,
                                             (unsigned)srcMoney, 0, ip);
        for (int slot = 3; slot <= 0x1a; ++slot)
        {
            // ORIG 0x853087a：遍历 (1-trader) 行 —— 对方注册、将转给 src 的
            // 头像物品（反编译 C：this + slot*0x48 + (1-local_68)*0x798 + 0x3b；
            // 汇编 0x8530a3a sub $0x1,%ebx? 实为 1-local_68 行）。
            int itemIdx = TradeSlotAt(m_slotsData, 1 - trader, slot)
                              ->m_item.m_addInfo;
            if (src->getAcccountHackInfo()->isAvatar((unsigned)itemIdx))
                src->getAcccountHackInfo()->sendAvatar(0, (unsigned)accId);
        }
    }

    // ---- 第二段：金币转移 + 物品转移（trader 0..1）----
    for (int trader = 0; trader <= 1; ++trader)
    {
        int count = 0;
        packet[trader].clear();
        packet[trader].put_header(0, 0x12);
        for (int slot = 3; slot <= 0x1a; ++slot)
        {
            if (TradeSlotAt(m_slotsData, trader, slot)->m_item.m_addInfo != 0)
                ++count;
        }
        int money = m_nMoney[trader];
        if (money > 0)
        {
            ++count;
            packet[trader].put_short(count);
            packet[trader].put_short(0);
            packet[trader].put_short(0);
            CUser* dst = m_pTraders[1 - trader];
            if (dst)
                dst->getCurCharacInvenW()->gain_money(
                    money, (eMoneyAddReason)1, 1, 0);
            if (money % 10000 != 0)
            {
                int accId = m_pTraders[trader]->get_acc_id();
                int etc = m_pTraders[1 - trader]->SetETC(0x2c4, accId);
                CUser* u = m_pTraders[1 - trader];
                u->getHackAnalyzer()->addServerHackCnt(
                    u, (WongWork::ENUM_HACKTYPE)0x2c4, 1, (unsigned)etc, 0);
            }
            CUser* src = m_pTraders[trader];
            if ((src->isHackUser() || src->isHackUserTimer()) && money != 0)
            {
                int accId = src->get_acc_id();
                m_pTraders[1 - trader]->gainMoneyFromHackUser(
                    (unsigned)accId, (unsigned long long)money, 1, 0);
            }
            if (m_pTraders[trader] && m_pTraders[1 - trader] &&
                m_pTraders[trader]->getCurCharacMoney() == 0)
            {
                CUser* to = m_pTraders[1 - trader];   // -0x50
                CUser* from = m_pTraders[trader];     // -0x4c
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("item_trade.cpp", 0xc51), true);
                guard.operator*()->operator<<(0xa1);
                guard.operator*()->operator<<(-1);
                SIG_SAVE_GOLD_ZERO_TRADE_LOG* log =
                    guard->GetInBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>();
                log->m_to_m_id = (unsigned)to->get_acc_id();
                std::strncpy(log->m_to_nat_ip, to->m_network.get_str_ip(), 0x10);
                to->m_network.GetPeerIP2(log->m_to_peer_ip, 0x14);
                log->m_to_nat_type = to->m_network.get_nat_type();
                log->m_from_m_id = (unsigned)from->get_acc_id();
                std::strncpy(log->m_from_nat_ip, from->m_network.get_str_ip(),
                             0x10);
                from->m_network.GetPeerIP2(log->m_from_peer_ip, 0x14);
                log->m_from_nat_type = from->m_network.get_nat_type();
                std::strncpy(log->m_to_mac_addr, to->m_network.get_mac_addr(),
                             0x41);
                std::strncpy(log->m_from_mac_addr,
                             from->m_network.get_mac_addr(), 0x41);
                log->m_injury_money = (unsigned)m_nMoney[trader];
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
            }
            m_nMoney[trader] = 0;
            // ORIG 0x853087a：WriteTradeUp/Down 第三参在清零后读成员（恒 0），
            // 金币金额不进物品交易日志。
            HistoryLog::WriteTradeUp(
                (FILE*)m_pTraders[trader]->m_field796f8, 0,
                m_nMoney[trader]);
            HistoryLog::WriteTradeDown(
                (FILE*)m_pTraders[1 - trader]->m_field796f8, 0,
                m_nMoney[trader]);
        }
        else
        {
            packet[trader].put_short(count);
        }

        for (int slot = 3; slot <= 0x1a; ++slot)
        {
            CUser* dst = m_pTraders[1 - trader];
            if (!dst)
                continue;
            TradeSlot* ts = TradeSlotAt(m_slotsData, trader, slot);
            if (ts->m_item.m_addInfo == 0)
                continue;

            // 拍卖均价累计
            CAuctionAveragePrice* avgMgr =
                G_CGameManager()->GetAuctionAveragePriceManager();
            if (avgMgr)
            {
                Inven_Item* item = &ts->m_item;
                unsigned char upgrade = item->GetUpgrade();
                int avg = avgMgr->GetAveragePrice(item->m_addInfo, upgrade);
                if (item->m_field1 == 2 || item->m_field1 == 3)
                    avg *= item->m_addInfo2;
                avgPrice[trader] += avg;
            }

            int ret = -1;
            if (ts->m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR)  // 头像
            {
                ret = dst->getCurCharacInvenW()->insertAvatarIntoInventory(
                    ts->m_item, (eAvatarItemAddReason)1, false);
                if (ret >= 0)
                {
                    TradeSlot* itemTs = TradeSlotAt(m_slotsData, trader, slot);
                    unsigned int avatarUid = (unsigned)itemTs->m_item.m_addInfo2;
                    unsigned int characNo =
                        (unsigned)dst->getCurCharacNo();
                    _SaveAvatarItemOwnerChange(
                        itemTs->m_item.m_addInfo2, itemTs->m_field04,
                        characNo, (unsigned char)ret);
                    DB_InsertCeraItemHistory::makeRequest(
                        itemTs->m_item.m_addInfo2, itemTs->m_field04,
                        characNo, 2);
                    // ORIG 0x853087a：EraseAvatarItem 于发送方(trader)的
                    // CAvatarItemMgrW，AddAvatarItem 于接收方(dst)的 mgr
                    //（汇编 0x8531400..0x8531456：Erase 侧取 (local_68+8)*4，
                    //  Add 侧取 (9-local_68)*4）。
                    WongWork::CAvatarItemMgr* srcMgr =
                        m_pTraders[trader]->getCurCharacInvenW()
                            ->GetAvatarItemMgrW();
                    WongWork::Avatar_Item* av =
                        srcMgr->EraseAvatarItem((int)avatarUid);
                    WongWork::CAvatarItemMgr* dstMgr =
                        dst->getCurCharacInvenW()->GetAvatarItemMgrW();
                    dstMgr->AddAvatarItem((int)avatarUid, av);
                    const char* agency = av->GetIPGAgencyNo();
                    m_pTraders[trader]->m_historyLog.AvatarItemDel(
                        itemTs->m_item.m_addInfo, (int)avatarUid, agency,
                        (eAvatarItemDelReason)0);
                    m_pTraders[1 - trader]->m_historyLog.AvatarItemAdd(
                        itemTs->m_item.m_addInfo, (int)avatarUid, agency,
                        (eAvatarItemAddReason)1);
                }
            }
            else if (ts->m_field08 == 7)  // 生物
            {
                ret = dst->getCurCharacInvenW()->insertItemIntoInventory(
                    ts->m_item, (eItemAddReason)0x1a, false, true);
                if (ret >= 0)
                {
                    if (ts->m_item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                    {
                        int creatureId = ts->m_item.m_addInfo2;
                        if (dst->getCurCharacInvenW()->GetCreatureMgrW()
                                ->FindCreatureItem(creatureId) == 0)
                            return 0;
                    }
                    int characNo =
                        m_pTraders[trader]->getCurCharacR()->m_characNo;
                    int size =
                        m_pTraders[trader]->getCurCharacInvenW()
                            ->GetCreatureMgrR()->GetCreatureItemSize() +
                        m_fieldF68[trader];
                    dst->getCurCharacInvenW()->GetCreatureMgrW()->ChangeOwner(
                        &ts->m_item, ret, 0x1a, characNo, size);
                    m_fieldF68[1 - trader] += 1;
                    int addInfo = ts->m_item.get_add_info();
                    dst->getCurCharacInvenW()->GetCreatureMgrW()
                        ->DeleteCreatureItem(addInfo, 0x14);
                    int type = ts->m_item.m_field1;
                    int addInfo2 = ts->m_item.get_add_info();
                    m_pTraders[1 - trader]->m_historyLog.CreatureItemDel(
                        (INVEN_TYPE)3, ts->m_item.m_addInfo, addInfo2, 0,
                        type, (eItemDelReason)0x14);
                }
            }
            else  // 普通物品
            {
                if (IsTradeLimitAttachTypeItem(ts->m_item))
                {
                    DecreaseTradeLimitCount(ts->m_item);
                    m_pTraders[1 - trader]->m_historyLog.ChangeItemLimitCount(
                        1, slot, ts->m_item.m_addInfo, -1, 2);
                }
                ret = dst->getCurCharacInvenW()->insertItemIntoInventory(
                    ts->m_item, (eItemAddReason)1, true, true);
                updateMap[m_pTraders[1 - trader]].push_back(ret);
            }

            if (ret < 0)
            {
                int characNo = dst->getCurCharacNo();
                cMyTrace tr(__PRETTY_FUNCTION__, 0xce5, 5);
                tr("insert_item(INVEN_TYPE_INVENTORY) fail - fatal! %d",
                   characNo);
                ts->m_item.reset();
                return 0;
            }
            else
            {
                HistoryLog::WriteTradeUp(
                    (FILE*)m_pTraders[trader]->m_field796f8,
                    ts->m_item.m_addInfo, ts->m_item.get_add_info());
                HistoryLog::WriteTradeDown(
                    (FILE*)m_pTraders[1 - trader]->m_field796f8,
                    ts->m_item.m_addInfo, ts->m_item.get_add_info());
                ts->m_item.reset();
                packet[trader].put_short(slot);
                packet[trader].put_short(ret);
            }
        }
        packet[trader].finalize(true);
    }

    // ---- 第三段：发包 + 交易计数 ----
    for (int trader = 0; trader <= 1; ++trader)
    {
        m_pTraders[1 - trader]->Send(packet[trader]);
        m_pTraders[1 - trader]->incTradeCount();
    }

    // ---- 第四段：更新物品列表 ----
    for (std::map<CUser*, std::vector<int> >::iterator it =
             updateMap.begin();
         it != updateMap.end(); ++it)
    {
        CUser* user = it->first;
        if (!user)
            return 0;
        std::vector<int>& vec = it->second;
        for (std::vector<int>::iterator vit = vec.begin(); vit != vec.end();
             ++vit)
        {
            user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, *vit);
        }
    }

    for (int trader = 0; trader <= 1; ++trader)
    {
        m_pTraders[trader]->m_historyLog.TradeEnd(
            (eTradeEndReason)2, avgPrice[trader], tradeLogMoney[trader],
            avgPrice[1 - trader], tradeLogMoney[1 - trader]);
        m_pTraders[trader]->GetGoldControl()->TradeComplete(0, 0);
    }
    return 1;
}
// ---- regist_item @ 0x08529e7e ----
// 注册物品/金钱到交易槽（ORIG 0x08529e7e，2021 条完整转写）：
//   type==0 && a==0 → 金钱（use_money 扣钱 → m_nMoney[trader] += b）；
//   type==0 其余    → 背包物品；type==1 → 头像；type==3 → 装备栏；
//   type==7         → 宠物。取物校验 → delete_item 扣除 → add_item 入槽 →
//   checkTrade/checkCancelTrade 成功发包、失败回滚归还。
int CTradeSpace::regist_item(CUser* user, int type, int a, int b)
{
    if (IsLocked())
        return -0x63;
    if (b <= 0)
        return -0x62;

    CUser* opp;
    int trader;
    int other;
    if (m_pTraders[0] == user) {
        opp = m_pTraders[1];
        trader = 0;
        other = 1;
    } else {
        opp = m_pTraders[0];
        trader = 1;
        other = 0;
    }
    if (opp == 0)
        return -1;

    TradeSlot slot;
    slot.m_field08 = (char)type;

    if (user->getCurCharacR() == 0) {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x147, 5);
        tr("CTradeSpace::regist_item()\t0 == owner->getCurCharacR()");
        return -2;
    }
    if (type != 0 && (user->isHackUser() || opp->isHackUser()))
        return -0x153;

    PacketGuard packet;

    if (type == 0) {
        if (a < 0 || a > 0x138) {
            LogManager::logFormat(
                1, "item_trade.cpp", __PRETTY_FUNCTION__, 0x15e,
                "slot_no(%d) < 0 || slot_no > Inven_Item::MAX_INVEN_SLOT", a);
            return -3;
        }
        if (a == 0) {
            // 金钱交易：反外挂总量规则 → 扣钱 → 入交易金额 → 发包
            if (GlobalData::g_autoPunishRuleHackTypeMgr->checkTotalTradeRuleOver(
                    user->getTotalHackUserTradeCnt(),
                    user->getTotalHackUserTradeGold()))
                return -0x16a;
            if (!user->getCurCharacInvenW()->use_money(b, (eMoneySubReason)2, 1))
                return -5;
            m_nMoney[trader] += b;
            if (checkCancelTrade() && checkTrade()) {
                packet.clear();
                packet.put_header(0, 0xf);
                packet.put_short(a);
                packet.put_int(0);
                packet.put_int(m_nMoney[trader]);
                packet.put_byte(0);
                packet.put_short(0);
                packet.put_int(0);
                packet.put_byte(0);
                packet.put_short(0);
                Inven_Item emptyItem;  // ORIG 0x943ddc0 静态空物品
                packet.put_packet(emptyItem);
                packet.finalize(true);
                opp->Send(packet);
                m_nState[trader] = 0;
                m_nState[other] = 0;
                return 0;
            }
            m_nMoney[trader] -= b;
            user->getCurCharacInvenW()->gain_money(b, (eMoneyAddReason)12, 1, 0);
            return -4;
        }
        // 背包物品
        if (user->isHackUser() || opp->isHackUser())
            return -0x1a1;
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, a);
        if (slot.m_item.m_addInfo == 0) {
            cMyTrace tr(__PRETTY_FUNCTION__, 0x1a8, 5);
            tr("CTradeSpace::regist_item, item find failed(1), ch=%d %d %d %d",
               user->getCurCharacNo(), slot.m_item.m_addInfo,
               slot.m_item.m_addInfo2, a);
        }
        if (!_IsTradable(slot.m_item))
            return -0x66;
        if (slot.m_item.m_field1 != 1) {
            if (slot.m_item.m_addInfo2 < b) {
                cMyTrace tr(__PRETTY_FUNCTION__, 0x1b5, 5);
                tr("CTradeSpace::regist_item, item count failed(1), ch=%d %d %d %d",
                   user->getCurCharacNo(), slot.m_item.m_addInfo,
                   slot.m_item.m_addInfo2, b);
                return -8;
            }
        }
        if (b <= 0) {
            cMyTrace tr(__PRETTY_FUNCTION__, 0x1be, 5);
            tr("CTradeSpace::regist_item, item count failed(2), ch=%d %d %d %d",
               user->getCurCharacNo(), slot.m_item.m_addInfo,
               slot.m_item.m_addInfo2, b);
            return -9;
        }
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, a, b, (eItemDelReason)2, 1)) {
            cMyTrace tr(__PRETTY_FUNCTION__, 0x1c8, 5);
            tr("CTradeSpace::regist_item, delete_item failed, ch=%d %d %d",
               user->getCurCharacNo(), a, b);
            return -10;
        }
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            return -1;
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            packet.put_int(
                TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_addInfo2);
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(slot.m_item.m_amp.getAbilityType());
            packet.put_short(slot.m_item.m_amp.getAbilityValue());
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            return slotIdx;
        }
        {
            cMyTrace tr(__PRETTY_FUNCTION__, 0x1fc, 5);
            tr("CTradeSpace::regist_item, (checkTrade() && checkCancelTrade()) failed, ch=%d %d %d",
               user->getCurCharacNo(), a, b);
        }
        // 回滚：槽位扣减/清空后 insert_item_special_slot 归还。
        // insert_item_special_slot(item, count, slot)：ORIG 0x8529e7e 处实参为
        // (param_4 数量, param_3 原槽)（汇编 esp+0x44=param_4、esp+0x48=param_3）。
        if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.isEquipableItemType()) {
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        } else {
            int cnt = TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_addInfo2;
            if (cnt > b)
                TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_addInfo2 =
                    cnt - b;
            else
                TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        }
        if (user->getCurCharacInvenW()->insert_item_special_slot(slot.m_item, b, a,
                                            (eMoneyAddReason)9,
                                            (eItemAddReason)0x10) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -11;
        }
        return -12;
    }

    if (type == 3) {
        // 装备栏（0..0x19）
        if (a < 0 || a > 0x19) {
            LogManager::logFormat(
                1, "item_trade.cpp", __PRETTY_FUNCTION__, 0x266,
                "slot_no(%d) < 0 || slot_no > ENUM_EQUIPMENTTYPE_TOTAL_MAX", a);
            return -13;
        }
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_INVEN, a);
        if (slot.m_item.m_addInfo == 0)
            return -14;
        if (!_IsTradable(slot.m_item))
            return -0x66;
        if (a <= 9) {
            // 头部槽：临时 key / 过期校验
            if (WongWork::CAvatarItemMgr::IsTempKey(slot.m_item.m_addInfo2))
                return -0x66;
            slot.m_field00 = a;
            slot.m_field04 = user->getCurCharacNo();
            int expire =
                user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpireDate(slot.m_item.m_addInfo2);
            if (expire != 0 && GlobalData::s_systemTime_.getCurSec() > expire)
                return -0x64;
        }
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_INVEN, a, 1, (eItemDelReason)2, 1)) {
            cMyTrace tr(__PRETTY_FUNCTION__, 0x281, 5);
            tr("CTradeSpace::regist_item 2, delete_item failed, ch=%d, %d %d",
               user->getCurCharacNo(), a, b);
            return -15;
        }
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            return -1;
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8)
                packet.put_int(user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                    slot.m_item.m_addInfo2, OS_API::GetDateTimeTick()));
            else
                packet.put_int(TradeSlotAt(m_slotsData, trader, slotIdx)
                                   ->m_item.m_addInfo2);
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(slot.m_item.m_amp.getAbilityType());
            packet.put_short(slot.m_item.m_amp.getAbilityValue());
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                packet.put_int(0x1e);
                packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                                      slot.m_item.m_addInfo2),
                                  0x1e);
                packet.put_int(4);
                stAvatarExpansionInfo_t info;
                void* pExp = user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    slot.m_item.m_addInfo2);
                if (pExp != 0)
                    packet.put_binary((const char*)pExp, 4);
                else {
                    info.init();
                    packet.put_binary((const char*)&info, 4);
                }
            }
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            return slotIdx;
        }
        // 回滚：insert_item_special_slot 归还。
        // ORIG 0x8529e7e（type==3 失败路径）实参 = (slot.m_item, param_4, param_3)，
        // 即 (数量=用户提交 b, 槽位=原装备槽 a)——与 type0 同布局（esp+0x44/0x48）。
        TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        if (user->getCurCharacInvenW()->insert_item_special_slot(slot.m_item, b, a,
                                            (eMoneyAddReason)9,
                                            (eItemAddReason)0x10) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -16;
        }
        return -17;
    }

    if (type == 1) {
        // 头像
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, a);
        if (b <= 0)
            return -9;
        slot.m_field00 = a;
        slot.m_field04 = user->getCurCharacNo();
        int expire =
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpireDate(slot.m_item.m_addInfo2);
        if (expire != 0 && GlobalData::s_systemTime_.getCurSec() > expire)
            return -0x64;
        if (WongWork::CAvatarItemMgr::IsTempKey(slot.m_item.m_addInfo2))
            return -0x66;
        if (!_IsTradable(slot.m_item))
            return -0x66;
        for (int i = 3; i <= 0x1a; ++i) {
            if (TradeSlotAt(m_slotsData, trader, i)->m_item.m_addInfo2 ==
                slot.m_item.m_addInfo2)
                return -0x67;
        }
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_AVATAR, a, b, (eItemDelReason)1, 1))
            return -10;
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            return -1;
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            packet.put_int(user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                slot.m_item.m_addInfo2, OS_API::GetDateTimeTick()));
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(0);
            packet.put_short(0);
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                packet.put_int(0x1e);
                packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                                      slot.m_item.m_addInfo2),
                                  0x1e);
                packet.put_int(4);
                stAvatarExpansionInfo_t info;
                void* pExp = user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    slot.m_item.m_addInfo2);
                if (pExp != 0)
                    packet.put_binary((const char*)pExp, 4);
                else {
                    info.init();
                    packet.put_binary((const char*)&info, 4);
                }
            }
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            return slotIdx;
        }
        // 回滚：insertAvatarIntoSpecificSlot 归还
        TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        if (user->getCurCharacInvenW()->insertAvatarIntoSpecificSlot(slot.m_item, a,
                                                (eAvatarItemAddReason)4, 1) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -11;
        }
        return -12;
    }

    if (type == 7) {
        // 宠物
        slot.m_item = user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_CREATURE, a);
        if (b <= 0)
            return -9;
        slot.m_field00 = a;
        slot.m_field04 = user->getCurCharacNo();
        if (!_IsTradable(slot.m_item))
            return -0x66;
        if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_CREATURE, a, b, (eItemDelReason)0x15, 1))
            return -10;
        int slotIdx = add_item(trader, slot, b);
        if (slotIdx == -1)
            return -1;
        if (checkTrade() && checkCancelTrade()) {
            m_nState[trader] = 0;
            m_nState[other] = 0;
            packet.clear();
            packet.put_header(0, 0xf);
            packet.put_short(slotIdx);
            packet.put_int(slot.m_item.m_addInfo);
            packet.put_int(
                TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_addInfo2);
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(0);
            packet.put_short(0);
            if (slot.m_item.m_field1 == 5) {
                int tick = OS_API::GetDateTimeTick();
                packet.put_int(user->getCurCharacInvenR()->GetCreatureMgrR()->GetRemainDate(
                    slot.m_item.m_addInfo2, tick));
            }
            packet.put_packet(slot.m_item);
            packet.finalize(true);
            opp->Send(packet);
            return slotIdx;
        }
        // 回滚：insertCreatureIntoSpecificSlot 归还
        TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        if (user->getCurCharacInvenW()->insertCreatureIntoSpecificSlot(slot.m_item, a,
                                                  (eItemAddReason)0x1d, 1) < 0) {
            user->m_historyLog.TradeItemAddFail(slot.m_item.m_addInfo, b);
            return -11;
        }
        return -12;
    }

    return -0x14;
}
// ============================================================================
// ============================================================================
// remove_item @ 0x0852bfcc —— ORIG 完整转写（O=1767 vs N=357）
// 语义：按 type 分派移除交易槽（0=金币/物品、1=头像、3=装备、7=生物），
// 物品放回 user 背包（插入失败返回负值，不回滚槽位）。
// 参数：trade_slot_no / type / target_slot(pos) / count(num)。
// 反汇编要点（TradeSlot::m_item @ +0x09，槽位副本 0x46 字节）：
//   - m_nMoney[trader] 编译为 (this + (0x3d8+trader)*4)
//   - 金币包 put_int(m_nMoney>0 ? 0 : -1)；物品包固定 put_int(-1)/put_int(0)
//   - type==0 且 trade_slot_no==0 时移除金币；否则按物品处理
//   - case 1/7 移除后 TradeSlot::Reset()；case 0/3 手动清部分字段
//   - case 0 非装备物品：仅 槽内数量>count 时部分扣减；count==数量时全清槽
//     （包回 -1/0，同 EQUIP 全清路径；0x852cbc5/0x852ccd0 段）
//   - case 3 校验 check_able_To_equip 后直接写 m_invenItem[target_slot]
//   - 广播：case 3 且 trade_slot_no<=0xb 且 user 状态非 5/8 时 make_basic_info+send_all
// ============================================================================
int CTradeSpace::remove_item(CUser* user, int trade_slot_no, int type,
                             int target_slot, int count)
{
    if (IsLocked())
        return -1;
    if (count <= 0)
        return -0x62;

    PacketGuard packet;
    int trader;
    int other;
    CUser* otherUser;
    if (m_pTraders[0] == user) {
        otherUser = m_pTraders[1];
        trader = 0;
        other = 1;
    } else {
        otherUser = m_pTraders[0];
        trader = 1;
        other = 0;
    }
    m_nState[trader] = 0;
    m_nState[other] = 0;

    if (otherUser == 0 || trade_slot_no > 0x1a)
        return -1;
    if (m_pTraders[trader]->getCurCharacR() == 0 ||
        user->getCurCharacR() == 0) {
        cMyTrace tr("int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x45a, 5);
        tr("CTradeSpace::remove_item()\t0 == "
           "m_pTraders[owner_index]->getCurCharacR() %x || 0 == "
           "owner->getCurCharacR() %x",
           m_pTraders[trader]->getCurCharacR(), user->getCurCharacR());
        return -1;
    }

    TradeSlot* tradeSlot = TradeSlotAt(m_slotsData, trader, trade_slot_no);
    if (tradeSlot->m_field08 != type)
        return -1;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    switch (type) {
    case 0:
        if (trade_slot_no == 0) {
            // ---- 金币 ----
            if (m_nMoney[trader] - count < 0)
                return -1;
            user->getCurCharacInvenW()->gain_money(
                count, (eMoneyAddReason)1, 1, 0);
            m_nMoney[trader] -= count;
            buf->clear();
            buf->put_header(0, 0xf);
            buf->put_short(trade_slot_no);
            buf->put_int(m_nMoney[trader] > 0 ? 0 : -1);
            buf->put_int(m_nMoney[trader]);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_packet(g_emptySlot);
            buf->finalize(true);
            otherUser->Send(packet);
            m_nState[trader] = 0;
            m_nState[other] = 0;
            return 0;
        }
        {
            // ---- 物品（trade_slot_no != 0）----
            TradeSlot copy = *tradeSlot;
            int ret = -1;
            int slotItemType =
                m_pTraders[trader]->getCurCharacInvenR()->GetSlotItemType(
                    target_slot);
            if (slotItemType != 0 &&
                slotItemType != copy.m_item.m_field1)
                return -1;
            if (copy.m_item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP) {
                if (copy.m_field08 != 0)
                    return -1;
                if (m_pTraders[trader]->getCurCharacInvenR()
                        ->check_empty_count(
                            (Inven_Item::ITEM_TYPE)copy.m_item.m_field1, 1)) {
                    ret = m_pTraders[trader]->getCurCharacInvenW()
                              ->insert_item_special_slot(
                                  copy.m_item, count, target_slot,
                                  (eMoneyAddReason)9, (eItemAddReason)0x10);
                    if (ret < 0) {
                        LogManager::logFormat(
                            1, "item_trade.cpp",
                            "int CTradeSpace::remove_item(CUser*, int, int, "
                            "int, int)",
                            0x4a5,
                            "FATAL_ERROR USER[%s] ITEMID[%d] -> SLOT[%d]",
                            user->getCurCharacName(), copy.m_item.m_addInfo,
                            target_slot);
                        return ret;
                    }
                } else {
                    LogManager::logFormat(
                        1, "item_trade.cpp",
                        "int CTradeSpace::remove_item(CUser*, int, int, int, "
                        "int)",
                        0x4ac, "[TRADE]FATAL_ERROR USER[%s] ITEMID[%d]",
                        user->getCurCharacName(), copy.m_item.m_addInfo);
                    return -1;
                }
            } else {
                // ---- 普通物品 ----
                if (copy.m_item.get_add_info() < count)
                    return -1;
                copy.m_item.set_add_info(count);
                if (count > 0x3e7) {
                    cMyTrace tr(
                        "int CTradeSpace::remove_item(CUser*, int, int, int, "
                        "int)",
                        0x4b8, 5);
                    tr("CTradeSpace::remove_item, moveitem.item.set_add_info "
                       ", ITEM #%d, %d",
                       copy.m_item.m_addInfo, count);
                }
                int itemIdx = m_pTraders[trader]->getCurCharacInvenR()
                                  ->check_item_exist(copy.m_item.m_addInfo);
                if (itemIdx != -1) {
                    ret = m_pTraders[trader]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  copy.m_item, (eItemAddReason)0x10, 1, 1);
                    if (ret < 0) {
                        LogManager::logFormat(
                            1, "item_trade.cpp",
                            "int CTradeSpace::remove_item(CUser*, int, int, "
                            "int, int)",
                            0x4cc, "[TRADE]FATAL_ERROR USER[%s] ITEMID[%d]",
                            user->getCurCharacName(), copy.m_item.m_addInfo);
                        return ret;
                    }
                } else {
                    if (m_pTraders[trader]->getCurCharacInvenR()
                            ->check_empty_count(
                                (Inven_Item::ITEM_TYPE)copy.m_item.m_field1,
                                1)) {
                        ret = m_pTraders[trader]->getCurCharacInvenW()
                                  ->insert_item_special_slot(
                                      copy.m_item, count, target_slot,
                                      (eMoneyAddReason)9,
                                      (eItemAddReason)0x10);
                        if (ret < 0)
                            return ret;
                    } else {
                        LogManager::logFormat(
                            1, "item_trade.cpp",
                            "int CTradeSpace::remove_item(CUser*, int, int, "
                            "int, int)",
                            0x4db, "FATAL_ERROR USER[%s] ITEMID[%d]",
                            user->getCurCharacName(), copy.m_item.m_addInfo);
                        return -1;
                    }
                }
            }
            m_nState[trader] = 0;
            m_nState[other] = 0;
            if (ret == -1)
                return ret;
            buf->clear();
            buf->put_header(0, 0xf);
            buf->put_short(trade_slot_no);
            // ORIG 0x852bfcc：EQUIP→全清；非EQUIP仅槽内数量>count 时扣减，否则全清。
            if (tradeSlot->m_item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP) {
                // 完全移除（清空槽位）
                buf->put_int(-1);
                buf->put_int(0);
                tradeSlot->m_field00 = 0;
                tradeSlot->m_field04 = 0;
                tradeSlot->m_field08 = 0;
                tradeSlot->m_item.m_field1 = 0;
                tradeSlot->m_item.m_addInfo = 0;
                tradeSlot->m_item.set_add_info(0);
                tradeSlot->m_item.ResetItemAttr();
                tradeSlot->m_item.m_fieldb = 0;
                tradeSlot->m_item.m_fieldd = 0;
            } else if (tradeSlot->m_item.get_add_info() > count) {
                // 部分移除（槽位数量 -= count）
                buf->put_int(copy.m_item.m_addInfo);
                int newAddInfo = tradeSlot->m_item.get_add_info() - count;
                buf->put_int(newAddInfo);
                tradeSlot->m_item.set_add_info(newAddInfo);
                if (newAddInfo > 0x3e7) {
                    cMyTrace tr(
                        "int CTradeSpace::remove_item(CUser*, int, int, int, "
                        "int)",
                        0x503, 5);
                    tr("CTradeSpace::remove_item, "
                       "m_ItemSlot[owner_index][trade_slot_no].item."
                       "set_add_info , ITEM #%d, %d",
                       tradeSlot->m_item.m_addInfo, newAddInfo);
                }
            } else {
                // 数量取尽（count == 槽内数量）：包回 -1/0 并整体清槽（同 EQUIP 全清）
                buf->put_int(-1);
                buf->put_int(0);
                tradeSlot->m_field00 = 0;
                tradeSlot->m_field04 = 0;
                tradeSlot->m_field08 = 0;
                tradeSlot->m_item.m_field1 = 0;
                tradeSlot->m_item.m_addInfo = 0;
                tradeSlot->m_item.set_add_info(0);
                tradeSlot->m_item.ResetItemAttr();
                tradeSlot->m_item.m_fieldb = 0;
                tradeSlot->m_item.m_fieldd = 0;
            }
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_packet(g_emptySlot);
            buf->finalize(true);
            otherUser->Send(packet);
            return ret;
        }
    case 1:  // ---- 头像 ----
        {
            TradeSlot copy = *tradeSlot;
            if (copy.m_field08 != 1)
                return -1;
            int ret = m_pTraders[trader]->getCurCharacInvenW()
                          ->insertAvatarIntoSpecificSlot(
                              copy.m_item, target_slot,
                              (eAvatarItemAddReason)4, 1);
            if (ret < 0)
                return ret;
            tradeSlot->Reset();
            if (copy.m_field00 != ret) {
                m_pTraders[trader]->getCurCharacInvenW()->SendAvatarEvent(
                    0x32, copy.m_field00 + 0xa, ret + 0xa,
                    copy.m_item.m_addInfo2, 0);
            }
            buf->clear();
            buf->put_header(0, 0xf);
            buf->put_short(trade_slot_no);
            buf->put_int(-1);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_packet(g_emptySlot);
            buf->finalize(true);
            otherUser->Send(packet);
            return ret;
        }
    case 3:  // ---- 装备 ----
        {
            if (trade_slot_no == 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x59a, "trade_slot_no == 0");
                return -1;
            }
            TradeSlot copy = *tradeSlot;
            if (copy.m_field08 != 3) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5a2, "moveitem.item_space != ENUM_ITEMSPACE_EQUIPPED");
                return -1;
            }
            if (copy.m_item.m_addInfo == 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5a8, "moveitem.item.item_id == 0");
                return -1;
            }
            if (copy.m_item.m_field1 == Inven_Item::ITEM_TYPE_AVATAR) {
                if (target_slot > 9)
                    return -1;
            } else {
                if (target_slot <= 9 || target_slot > 0x15)
                    return -1;
            }
            Inven_Item slotItem;
            GetInvenSlot(&slotItem,
                         const_cast<CInventory*>(user->getCurCharacInvenR()),
                         0, target_slot);
            if (slotItem.m_addInfo != 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5bb, "moveitem.item.item_id == 0");
                return -1;
            }
            CEquipItem* equipRef =
                (CEquipItem*)G_CDataManager()->find_item(
                    copy.m_item.m_addInfo);
            if (equipRef == 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5c3, "equip_ref == NULL");
                return -1;
            }
            int res = equipRef->check_able_To_equip(
                (char)target_slot, user->get_charac_job(),
                user->get_charac_level());
            if (res != 0) {
                LogManager::logFormat(
                    1, "item_trade.cpp",
                    "int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                    0x5ca, "equip_ref->check_able_To_equip return false res(%d)",
                    res);
                return -1;
            }
            user->getCurCharacInvenW()->m_invenItem[target_slot] =
                copy.m_item;
            buf->clear();
            buf->put_header(0, 0xf);
            buf->put_short(trade_slot_no);
            buf->put_int(-1);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_packet(g_emptySlot);
            buf->finalize(true);
            otherUser->Send(packet);
            tradeSlot->m_field00 = 0;
            tradeSlot->m_field04 = 0;
            tradeSlot->m_field08 = 0;
            tradeSlot->m_item.m_field1 = 0;
            tradeSlot->m_item.m_addInfo = 0;
            tradeSlot->m_item.set_add_info(0);
            tradeSlot->m_item.ResetItemAttr();
            tradeSlot->m_item.m_fieldb = 0;
            tradeSlot->m_item.m_fieldd = 0;
            m_nState[trader] = 0;
            m_nState[other] = 0;
            if (trade_slot_no <= 0xb) {
                int st = user->get_state();
                if (st != 5 && st != 8) {
                    buf->clear();
                    buf->put_header(0, 2);
                    buf->put_byte(0);
                    buf->put_short(1);
                    user->make_basic_info((char*)&packet, 0);
                    buf->finalize(true);
                    G_GameWorld()->send_all(packet, user);
                }
            }
            return target_slot;
        }
    case 7:  // ---- 生物 ----
        {
            TradeSlot copy = *tradeSlot;
            if (copy.m_field08 != 7)
                return -1;
            if (copy.m_item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP) {
                if (!m_pTraders[trader]->getCurCharacInvenW()
                         ->GetCreatureMgrW()
                         ->IsCreatureEquipmentScope(target_slot))
                    return -1;
            }
            int ret = m_pTraders[trader]->getCurCharacInvenW()
                          ->insertCreatureIntoSpecificSlot(
                              copy.m_item, target_slot,
                              (eItemAddReason)0x1d, 1);
            if (ret < 0)
                return ret;
            tradeSlot->Reset();
            if (copy.m_field00 == ret) {
                m_pTraders[trader]->getCurCharacInvenW()->WriteCreatureLog(
                    (INVEN_TYPE)3, copy.m_item.m_addInfo,
                    copy.m_item.m_addInfo2, 1, copy.m_item.m_field1,
                    (eItemAddReason)0x1d);
            }
            else {
                m_pTraders[trader]->getCurCharacInvenW()
                    ->GetCreatureMgrW()
                    ->ChangeSlotNo(&copy.m_item, ret, 0x1d);
            }
            buf->clear();
            buf->put_header(0, 0xf);
            buf->put_short(trade_slot_no);
            buf->put_int(-1);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_int(0);
            buf->put_byte(0);
            buf->put_short(0);
            buf->put_packet(g_emptySlot);
            buf->finalize(true);
            otherUser->Send(packet);
            return ret;
        }
    default:
        return -1;
    }
}
