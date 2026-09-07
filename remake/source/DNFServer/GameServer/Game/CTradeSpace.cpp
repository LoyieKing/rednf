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
    // ORIG 0x8529aa2（O=190）：单槽 local_14 = idx = 返回值；equipable 分支
    // 物理在前（+0x3b test-true fallthrough）；dst 每用点全链重算（P157）；
    // 已存在路径 newCount 落 local_10 槽（+0xe8/+0xee add mem 形）。
    int idx;
    if (count < 0)
        return -1;
    idx = -1;
    lock();
    if (slot.m_item.isEquipableItemType()) {
        idx = get_empty_itemslot(traderIdx);
        if (idx != -1) {
            // ORIG +0x5a：单次全链计算，cmp 与 memcpy 共用（local dst 形）
            TradeSlot* dst = &m_slotsData[traderIdx][idx];
            if (dst != &slot)
                memcpy(dst, &slot, 0x46);
        }
    } else {
        idx = check_item_exist(traderIdx, slot.m_item.m_addInfo);
        if (idx == -1) {
            idx = get_empty_itemslot(traderIdx);
            if (idx != -1) {
                TradeSlot* dst = &m_slotsData[traderIdx][idx];  // +0x1b7
                if (dst != &slot)
                    memcpy(dst, &slot, 0x46);
                // set_add_info/trace 各自重算全链（+0x1f3/+0x22c，P157）
                m_slotsData[traderIdx][idx].m_item.set_add_info(count);
                if (count > 999) {
                    cMyTrace(__PRETTY_FUNCTION__, 0xde, 5)(
                        "CTradeSpace::add_item, ITEM #%d, %d",
                        m_slotsData[traderIdx][idx].m_item.m_addInfo, count);
                }
            }
        } else {
            // +0xc5 全链读 m_addInfo2；+0x109 重算链 set_add_info
            int newCount =
                m_slotsData[traderIdx][idx].m_item.m_addInfo2 + count;
            m_slotsData[traderIdx][idx].m_item.set_add_info(newCount);
            if (newCount > 999) {
                cMyTrace(__PRETTY_FUNCTION__, 0xd1, 5)(
                    "CTradeSpace::add_item, ITEM #%d, %d",
                    m_slotsData[traderIdx][idx].m_item.m_addInfo, newCount);
            }
        }
    }
    unlock();
    return idx;
}

// ---- set_traders @ 0x085295f0 ----
void CTradeSpace::set_traders(CUser* a, CUser* b)
{
    lock();
    m_bTrading = 1;
    _SetLock(0);
    // ORIG +0x2c-0x6c：赋值→重读→写 m_padf 交替（无缓存）；
    // m_pTraders[0]=a 写入为 ORIG 实有（+0x32 mov %edx,0x20(%eax)）
    m_pTraders[0] = a;
    m_pTraders[0]->m_padf = (unsigned short)b->get_unique_id();
    m_pTraders[1] = b;
    m_pTraders[1]->m_padf = (unsigned short)a->get_unique_id();
    // ORIG +0x7b-0x11a：&& 短路边 je 假岛、characName 先于 acc_name 求值
    // （R2L 内联实参，P134 无中间槽）；不命名 bool/指针局部避免 spill 帧膨胀
    if (m_pTraders[1]->getCurCharacR() != 0 &&
        m_pTraders[0]->getCurCharacR() != 0) {
        m_pTraders[0]->m_historyLog.SetTrader(
            m_pTraders[1]->get_acc_name(),
            m_pTraders[1]->getCurCharacName());
        m_pTraders[1]->m_historyLog.SetTrader(
            m_pTraders[0]->get_acc_name(),
            m_pTraders[0]->getCurCharacName());
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
bool CTradeSpace::cancel_trade()
{
    if (m_pTraders[0]->getCurCharacR() == 0 ||
        m_pTraders[1]->getCurCharacR() == 0) {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x788, 5);
        tr("CTradeSpace::cancel_trade()\t0 == m_pTraders[0]->getCurCharacR() || m_pTraders[1]->getCurCharacR()");
        return false;
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
    // ORIG +0x24..：4.1 将 memset(title,0,0x1d)/memset(msg,0,0xff) 内联展开为
    // 寄存器基址对齐循环（ebx/ecx/edx，无栈槽迭代器）。4.4 对字面 memset 发
    // call（-35 假 we-smaller）或手展成槽变量形（+31）；此处按 ORIG 展开语义
    // 手写，形态差记跨版本结构性残差（expansion 寄存器分配不可源码控制）。
    {
        // 4.1 memset 内联展开循环变量全程 callee-saved（ebx/edx/ecx）；
        // register 形促 4.4 保留寄存器分配（P87），避免栈槽迭代器 +31
        register char* q = title;
        register int n = 0x1d;
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
        register unsigned int m = n & 0xfffffffc;
        register unsigned int k = 0;
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
        register char* q = msg;
        for (register int i = 0x3f; i != 0; --i) {
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
// 残差豁免（O=425/N=424）：SetInventoryMemory 实参 lea 装载位差 1 条。
// 校验交易可完成（ORIG 0x8530210，O=425）：双方各造 CInventory 副本
//（setCopy 直取 characR()->m_inven，+0xda 成员地址，无 InvenR getter），
// 模拟把 trader 登记的物品/金钱插入**对方** copy[1-trader]；
// 金钱分支 gain_money 返回值 != 提交额即失败；物品三分支
//（item.m_field1==8 头像 / m_field08==7 宠物 / else 背包），失败按
// field1==8 双 trace（0xba7/0xbab）。bool 返回（PRETTY @0x8c98444）。
bool CTradeSpace::checkTrade()
{
    CInventory copy[2];
    // ORIG +0xe4-0x120：mem 构造与 Set 交替（非并列声明后统一 Set）
    InventoryMemory mem0;
    copy[0].SetInventoryMemory(&mem0);
    InventoryMemory mem1;
    copy[1].SetInventoryMemory(&mem1);
    // CUser.h _Charac_info+0xda 内嵌 CInventory（m_padda 声明域，CUserCharacInfo
    // 头同区 m_inven 视图）；ORIG 0x85302f9 直取 characR()+0xda 传 setCopy
    copy[0].setCopy(*(const CInventory*)m_pTraders[0]->getCurCharacR()->m_padda);
    copy[1].setCopy(*(const CInventory*)m_pTraders[1]->getCurCharacR()->m_padda);
    int count = 0;
    for (int trader = 0; trader <= 1; ++trader) {
        count = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            if (m_slotsData[trader][s].m_item.m_addInfo != 0)
                ++count;
        }
        // ORIG +0x??：money>0 → ++count → m_pTraders[1-trader] 判空 →
        // gain_money 到对方副本；返回值 != 提交额 → false
        if (m_nMoney[trader] > 0) {
            ++count;
            if (m_pTraders[1 - trader] != 0) {
                int got = copy[1 - trader].gain_money(
                    m_nMoney[trader], (eMoneyAddReason)1, 0, 0);
                if (m_nMoney[trader] != got)
                    return false;
            }
        }
        for (int s = 3; s <= 0x1a; ++s) {
            // 条件序：对方指针在前、槽非空在后（ORIG +0x1f9-0x214）
            if (m_pTraders[1 - trader] != 0 &&
                m_slotsData[trader][s].m_item.m_addInfo != 0) {
                int ret;
                if (m_slotsData[trader][s].m_item.m_field1 ==
                    Inven_Item::ITEM_TYPE_AVATAR) {
                    ret = copy[1 - trader].insertAvatarIntoInventory(
                        m_slotsData[trader][s].m_item,
                        (eAvatarItemAddReason)1, 0);
                } else if (m_slotsData[trader][s].m_field08 == 7) {
                    ret = copy[1 - trader].insertItemIntoCreature(
                        m_slotsData[trader][s].m_item, (eItemAddReason)0x1a,
                        0, 1);
                } else {
                    ret = copy[1 - trader].insertItemIntoInventory(
                        m_slotsData[trader][s].m_item, (eItemAddReason)1, 0,
                        1);
                }
                if (ret < 0) {
                    if (m_slotsData[trader][s].m_item.m_field1 ==
                        Inven_Item::ITEM_TYPE_AVATAR)
                        cMyTrace(__PRETTY_FUNCTION__, 0xba7, 5)(
                            "try insert_item(INVEN_TYPE_AVATAR) fail - "
                            "fatal! ch=%d",
                            m_pTraders[1 - trader]->getCurCharacNo());
                    else
                        cMyTrace(__PRETTY_FUNCTION__, 0xbab, 5)(
                            "try insert_item(INVEN_TYPE_INVENTORY) fail - "
                            "fatal! ch=%d",
                            m_pTraders[1 - trader]->getCurCharacNo());
                    return false;
                }
            }
        }
    }
    return true;
}

// ---- checkCancelTrade @ 0x0852dbb8 ----
// 残差豁免（O=533/N=528）：ORIG 空 else 的 nop;jmp 对齐岛（+0x756 等，
// nop-strip 后消）+ Set 实参 lea 位差。
// 校验取消交易可行（ORIG 0x852dbb8，O=533）：双方各造一个 CInventory 副本
//（SetInventoryMemory 挂独立 InventoryMemory + setCopy(getCurCharacInvenRefR)），
// 模拟把各交易者自己登记的物品按 m_field08（0=背包/1=头像/7=宠物/else=装备）
// 与金钱插回其自身副本；任一插入失败 → 该分支专属 trace 并返回 false。
// 形态要点（逐点对 ORIG 汇编）：
//   - 双 mem 独立对象（ctor→Set 交替，+0xdc..0x120 序列），非 char[]+memset
//   - count(local_34) 跨外圈共享；money 分支 ++count 位于 && 短路内
//   - item 循环条件：槽非空在前、owner 判空在后（+0x23f-0x253）
//   - 四分支失败 trace 各独立字符串（0x6af/0x6b9/0x6c3/0x6d8），
//     characNo 先于 trace 构造求值（链式，P37）
// §6.2 巨件核销：P167 35/35 [0缺0多]；gap 5→4；真修：P191 trader 双初始化
//   （int trader=0; + for(trader=0;…) 复现 ORIG +0x189/+0x190 双 movl，O533/N529）。
//   残差 4 条归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：
//   [dead-store] +0x253 gain_money 返回值 mov %eax,-0x20 死存，4.4 对未读
//     局部恒不占槽（技巧31 负结论），源码不可复现 +1；
//   [跨版本空体岛] +0x756/+0x759 两条失败路径各挂 nop;jmp;nop 中继
//     （负例表「空 else nop;jmp 岛 vs jump-threading」行），4.4 恒直达自增 +3。
bool CTradeSpace::checkCancelTrade()
{
    if (m_pTraders[0]->getCurCharacR() == 0 ||
        m_pTraders[1]->getCurCharacR() == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x674, 5)(
            "CTradeSpace::cancel_trade()\t0 == m_pTraders[0]->getCurCharacR()"
            " || m_pTraders[1]->getCurCharacR()");
        return false;
    }
    CInventory copy[2];
    // ORIG +0xe4-0x120：mem 构造与 Set 交替（非并列声明后统一 Set）
    InventoryMemory mem0;
    copy[0].SetInventoryMemory(&mem0);
    InventoryMemory mem1;
    copy[1].SetInventoryMemory(&mem1);
    copy[0].setCopy(*m_pTraders[0]->getCurCharacInvenRefR());
    copy[1].setCopy(*m_pTraders[1]->getCurCharacInvenRefR());
    int count = 0;
    // ORIG +0x189/+0x190：trader 声明置 0 + for init 双写（P191 双初始化形）
    int trader = 0;
    for (trader = 0; trader <= 1; ++trader) {
        count = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            if (m_slotsData[trader][s].m_item.m_addInfo != 0)
                ++count;
        }
        // ORIG +0x20e：嵌套两 if（逗号/&& 混写会诱发 bool 物化岛）
        if (m_nMoney[trader] > 0) {
            ++count;
            if (m_pTraders[trader] != 0)
                copy[trader].gain_money(m_nMoney[trader],
                                        (eMoneyAddReason)2, 0, 0);
        }
        for (int s = 3; s <= 0x1a; ++s) {
            if (m_slotsData[trader][s].m_item.m_addInfo != 0 &&
                m_pTraders[trader] != 0) {
                int ret;
                if (m_slotsData[trader][s].m_field08 == 0) {
                    ret = copy[trader].insertItemIntoInventory(
                        m_slotsData[trader][s].m_item, (eItemAddReason)2, 0,
                        1);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6af, 5)(
                            "try insert_item(INVEN_TYPE_INVENTORY) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                } else if (m_slotsData[trader][s].m_field08 == 1) {
                    ret = copy[trader].insertAvatarIntoInventory(
                        m_slotsData[trader][s].m_item,
                        (eAvatarItemAddReason)3, 0);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6b9, 5)(
                            "try insert_item(ENUM_ITEMSPACE_AVATAR) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                } else if (m_slotsData[trader][s].m_field08 == 7) {
                    ret = copy[trader].insertItemIntoCreature(
                        m_slotsData[trader][s].m_item, (eItemAddReason)0x1c,
                        0, 1);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6c3, 5)(
                            "try insert_item(ENUM_ITEMSPACE_CREATURE) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                } else {
                    ret = copy[trader].insertItemIntoEquipment(
                        m_slotsData[trader][s].m_item, (eItemAddReason)2, 0,
                        1);
                    if (ret < 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0x6d8, 5)(
                            "try insert_item(INVEN_TYPE_EQUIP) fail - "
                            "[Fail]Insert_item - fatal! canceled ch=%d",
                            m_pTraders[trader]->getCurCharacNo());
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

// ---- cancel_trade_for_china @ 0x0852e544 ----
// 取消交易（ORIG 0x803 条指令完整转写）：金钱退回各自背包，注册物品按
// m_field08 分类插回（1=头像 / 7=宠物 / 0=背包 / 其它=装备），失败走邮件补偿；
// 构建取消回执包（header 0x10，count 占位 get_index+回填 put_short(idx,done)）
// 发给非触发方交易者。
// 形态（P157/P160）：owner 判空、getCurCharacR、槽字段、insert receiver 全部
//   每用点重读（ORIG +0x30c-0x367 条件三连每次全链寻址）；packet 视图内联
//   lea；无 Inven_Item item 栈拷贝（ORIG struct 实参直接从槽 push）。
//   返回值 bool（ORIG __PRETTY_FUNCTION__@+0x181 串为 bool，P159/P27）。
// 残差豁免（O=803/N=814，canon 级 ~11，跨版本结构性）：
//   [setcc-fold] +0x336 gCharacR!=0 判定：ORIG `test;sete;test;jne` 物化岛，
//     4.4 恒直判 je；+0x367 addInfo!=0 反向（ORIG 直判、4.4 生 mov$1/$0 岛）。
//   [jump-threading] 类型四分支物理序：ORIG inv(+0x3d0)→avatar(+0x4c2)→
//     creature(+0x669)→equip(+0x776)（4.1 选 ==0 case fall-through），4.4 对
//     同一源码布局 avatar 直落；等长块序差（diff 锚定重排）。
//   [N18b] put_short(count) 前 count 槽值：ORIG 预载 ecx 后 `mov %ecx,E`
//     （+0x241/+0x2ea），4.4 即时 `mov S,%eax; mov %eax,E` +1×2。
// §6.2 巨件核销：P167 47/47 [0缺0多]；gap −11→−11（O803/N814）；
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：上述
//   [setcc-fold]/[jump-threading 块序重排 @@-337,28+329,189/-403,157+556,25]/[N18b]
//   结构性等价残差，P167 调用点 multiset 全等、语义逐分支对照完整，N 封顶。
bool CTradeSpace::cancel_trade_for_china(CUser* user)
{
    PacketGuard packet[2];                      // -0x58
    int user_index = 0;                         // -0x2c（ORIG 仅循环前 +0x0c/0x13 置零）
    int count;                                  // -0x30（每外圈头 +0x88 置 0）
    int done;                                   // -0x28（循环内 +0x117 置 0）
    int countIdx;                               // -0x5c（循环内 +0x11e 置 0）
    int ret;                                    // -0x24（循环内 +0x2f9 置 0）
    for (; user_index <= 1; ++user_index) {
        count = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            if (m_slotsData[user_index][s].m_item.m_addInfo != 0)
                ++count;
        }
        ((InterfacePacketBuf*)&packet[user_index])->clear();
        ((InterfacePacketBuf*)&packet[user_index])->put_header(0, 0x10);
        done = 0;
        countIdx = 0;
        // ORIG +0x136：money<=0 跳 else（仅占位 put_short(count)）
        if (m_nMoney[user_index] > 0) {
            ++count;
            if (m_pTraders[user_index] != 0) {
                if (m_pTraders[user_index]->getCurCharacR() == 0) {
                    cMyTrace(__PRETTY_FUNCTION__, 0x71c, 5)(
                        "CTradeSpace::cancel_trade()\t"
                        "m_pTraders[user_index]->getCurCharacR()");
                    return false;
                }
                m_pTraders[user_index]->getCurCharacInvenW()->gain_money(
                    m_nMoney[user_index], (eMoneyAddReason)2, 1, 0);
                m_nMoney[user_index] = 0;
                ++done;  // ORIG +0x20b：金币条目计入实际数
            }
            countIdx = packet[user_index].get_index();
            ((InterfacePacketBuf*)&packet[user_index])->put_short(count);
            ((InterfacePacketBuf*)&packet[user_index])->put_short(0);
            ((InterfacePacketBuf*)&packet[user_index])->put_byte(0);
            ((InterfacePacketBuf*)&packet[user_index])->put_short(0);
        } else {
            countIdx = packet[user_index].get_index();
            ((InterfacePacketBuf*)&packet[user_index])->put_short(count);
        }
        for (int s = 3; s <= 0x1a; ++s) {
            // ORIG +0x30c：owner、getCurCharacR、槽非空三连（每次重读）
            if (m_pTraders[user_index] != 0 &&
                m_pTraders[user_index]->getCurCharacR() != 0 &&
                m_slotsData[user_index][s].m_item.m_addInfo != 0) {
                ((InterfacePacketBuf*)&packet[user_index])->put_short(s);
                if (m_slotsData[user_index][s].m_field08 == 1) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertAvatarIntoInventory(
                                  m_slotsData[user_index][s].m_item,
                                  (eAvatarItemAddReason)3, 1);
                    if (ret >= 0 &&
                        m_slotsData[user_index][s].m_field00 != ret) {
                        m_pTraders[user_index]->getCurCharacInvenR()
                            ->SendAvatarEvent(
                                0x32,
                                m_slotsData[user_index][s].m_field00 + 10,
                                ret + 10,
                                m_slotsData[user_index][s].m_item.m_addInfo2,
                                0);
                    }
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(1);
                } else if (m_slotsData[user_index][s].m_field08 == 7) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoCreature(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)0x1c, 1, 1);
                    if (ret >= 0 &&
                        m_slotsData[user_index][s].m_field00 != ret) {
                        m_pTraders[user_index]->getCurCharacInvenW()
                            ->GetCreatureMgrW()
                            ->ChangeSlotNo(
                                &m_slotsData[user_index][s].m_item, ret,
                                0x1c);
                    }
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(7);
                } else if (m_slotsData[user_index][s].m_field08 == 0) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)2, 1, 1);
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(0);
                } else {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoEquipment(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)2, 1, 1);
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(3);
                }
                // ORIG +0x863：插入失败 → 邮件补偿 + 槽 item reset + 回写 ret
                if (ret < 0)
                    send_lose_item_for_china(
                        m_pTraders[user_index],
                        m_slotsData[user_index][s].m_item);
                m_slotsData[user_index][s].m_item.reset();
                ((InterfacePacketBuf*)&packet[user_index])->put_short(ret);
                ++done;
            }
        }
        // ORIG +0x936：以实际条目数回填 count 占位（put_short(int&,int)）
        packet[user_index].put_short(countIdx, done);
        packet[user_index].finalize(true);
    }
    for (int i = 0; i <= 1; ++i) {
        if (m_pTraders[i] != 0 && m_pTraders[i] != user) {
            m_pTraders[i]->Send(packet[i]);
            m_pTraders[i]->send_equip(0);
        }
    }
    for (int i = 0; i <= 1; ++i) {
        if (m_pTraders[i] != 0)
            m_pTraders[i]->m_historyLog.TradeEnd((eTradeEndReason)0, 0, 0, 0,
                                                 0);
    }
    return true;
}

// ---- change_trade_state @ 0x0852f134 ----
// 残差豁免（O=1096/N=1098，+2）：bool hack 汇聚岛与 4.4 折叠的 ±差异、
// [jump-threading] case2 cancel 成功/失败块序。float/double 阈值块按
// ORIG Ghidra 精度（第一块 float 0.7+0.01*r1，第二块 double 0.9）。
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
            for (int trader = 0; trader <= 1; ++trader) {
                for (int slot = 3; slot <= 0x1a; ++slot) {
                    if (m_slotsData[trader][slot].m_item.m_addInfo != 0)
                        ++count;
                }
            }
            if (count > 0 && m_pTraders[0] != 0 && m_pTraders[1] != 0) {
                cMyTrace(__PRETTY_FUNCTION__, 0x9eb, 5)(
                    "TRADE CANCELED! BUT, ITEM REMAINED! %d Count , "
                    "Ch1 %d Ch2 %d",
                    count, m_pTraders[0]->get_charac_no(-1),
                    m_pTraders[1]->get_charac_no(-1));
                for (int trader = 0; trader <= 1; ++trader) {
                    if (m_pTraders[trader] != 0) {
                        for (int slot = 3; slot <= 0x1a; ++slot) {
                            // ORIG +0x2e6-0x3b1：add_info/addInfo/characNo
                            // 三独立链求值（R2L），串无句号
                            if (m_slotsData[trader][slot].m_item
                                    .m_addInfo != 0) {
                                cMyTrace(__PRETTY_FUNCTION__, 0x9f8, 5)(
                                    "Remained Item %d : %d x %d",
                                    m_pTraders[trader]->get_charac_no(-1),
                                    m_slotsData[trader][slot]
                                        .m_item.m_addInfo,
                                    m_slotsData[trader][slot]
                                        .m_item.get_add_info());
                            }
                        }
                    }
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
                // ORIG +0x5ef-0x6d7：链上直读；isEquipable 正判 je→占格块
                if (m_slotsData[me][slot].m_item.m_addInfo != 0) {
                    if (m_slotsData[me][slot].m_item.isEquipableItemType()) {
                        needCnt[m_slotsData[me][slot].m_item.m_field1]++;
                    } else {
                        if (target->getCurCharacInvenR()->check_item_exist(
                                m_slotsData[me][slot].m_item.m_addInfo) < 0)
                            needCnt[m_slotsData[me][slot].m_item.m_field1]++;
                    }
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
                        if (m_slotsData[opp][slot].m_item.m_addInfo != 0) {
                            CItem* item = G_CDataManager()->find_item(
                                m_slotsData[opp][slot].m_item.m_addInfo);
                            if (item != 0 && !item->IsCreatureItem() &&
                                item->get_rarity() <= 1) {
                                bOppHasLow = true;
                                break;
                            }
                        }
                    }
                }
                if (target != 0) {
                    for (int slot = 3; slot <= 0x1a; ++slot) {
                        if (m_slotsData[me][slot].m_item.m_addInfo != 0) {
                            CItem* item = G_CDataManager()->find_item(
                                m_slotsData[me][slot].m_item.m_addInfo);
                            if (item != 0 && !item->IsCreatureItem() &&
                                item->get_rarity() <= 1) {
                                bMeHasLow = true;
                                break;
                            }
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
                // ORIG Ghidra：float 精度比较 + bool 汇聚岛（mov$1/$0;test al）
                bool hack;
                if ((float)goldMe <=
                        (0.7f + 0.01f * (float)r1) * (float)totalMoney ||
                    goldMe < 10000)
                    hack = false;
                else
                    hack = true;
                if (hack) {
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
                bool hack2;
                if ((double)goldOpp <=
                        0.9 * (double)totalMoney ||
                    goldOpp < 10000)
                    hack2 = false;
                else
                    hack2 = true;
                if (hack2) {
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

// 执行交易（ORIG 0x0853087a 完整转写）：防 hack 金币/头像上报 → 双方金币/物品
// 互换 → 零金币日志 → 物品转移（头像/生物/普通三分支，共享 ret<0 失败出口）→
// 发包 → 更新列表 → 收尾日志。失败物品不回补（ORIG 直接 reset 槽位返回 0）。
// 形态（P157/P160，O=1606）：第二循环全程不缓存 m_pTraders/m_slotsData 指针，
//   每用点重读成员（+0x8d6/+0x91d 等）；仅 ORIG 自身缓存处保留具名局部：
//   -0x60/-0x5c src/dst（第一循环）、-0x3c item ptr（均价块）、
//   -0x34 slot ptr（头像成功块）、-0x2c characNo/-0x28 size（生物块）、
//   -0x4c/-0x50 from/to（零金币块）。
//   ret 共享槽 -0xac 初值 0（ORIG +0x0de）；三分支各写，共享 +0x11ce 负值检查。
//   ChangeItemLimitCount 记在**发送方** trader 的 historyLog（ORIG +0x1090）。
//   TradeComplete 参数为 tradeLogMoney[trader]/[1-trader]（ORIG +0x1613/+0x1639
//   直取 user+0x8e980，GetGoldControl 头内联 W@0x084ed008）。
// 残差豁免（O=1606/N=1619，canon 级 ~13，跨版本结构性）：
//   [N18a] 头像成功块 _Save/DB 参数 ts+4/ts+0x10 双读：ORIG 4.1 单段
//     `mov S,%edx; mov disp(%edx)`（+0xb0e/+0xb50），4.4 恒两跳物化 +1×2。
//   [N76] m_fieldF68[1-trader]+=1：ORIG SAVE_EXPR 折叠单索引复用（+0xed0-
//     +0xef0 8 条），4.4 双索引重算 13 条（+= 展开策略版本差，源码不可控）。
//   [N7/寄存器] 尾 TradeComplete money 槽基址 -0xa4 vs -0x88 与 ebx/edx 互换
//     （.o 帧槽序，AE 归一吸收）。
bool CTradeSpace::proceed_trade()
{
    if (!IsLocked())
        return false;
    if (!checkTrade())
        return false;

    int tradeLogMoney[2] = {0, 0};                // -0x88/-0x84（TradeEnd 用，恒 0）
    int avgPrice[2] = {0, 0};                     // -0x90/-0x8c（拍卖均价累计）
    PacketGuard packet[2];                        // -0xa8 起 2 个
    std::map<CUser*, std::vector<int> > updateMap;  // -0xc4

    // ---- 第一段：防 hack 上报（trader 0..1）----
    for (int trader = 0; trader <= 1; ++trader)
    {
        CUser* src = m_pTraders[trader];          // -0x60
        CUser* dst = m_pTraders[1 - trader];      // -0x5c
        char ip[16];                              // -0xd4
        ((XNuclear::CHades*)dst->getHades())->GetIP(ip);
        int srcMoney = src->getCurCharacInvenR()->get_money();
        // ORIG +0x14b-0x1b2：srcMoney→money→accId→receiver 求值序
        src->getAcccountHackInfo()->sendGold(0, dst->get_acc_id(),
                                             m_nMoney[trader], srcMoney, 0, ip);
        for (int slot = 3; slot <= 0x1a; ++slot)
        {
            // ORIG +0x1c0：遍历 (1-trader) 行、发方 src 上报（0x8530a3a）
            int itemIdx = m_slotsData[1 - trader][slot].m_item.m_addInfo;
            if (src->getAcccountHackInfo()->isAvatar((unsigned)itemIdx)) {
                unsigned long acc = dst->get_acc_id();
                src->getAcccountHackInfo()->sendAvatar(0, acc);
            }
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
            if (m_slotsData[trader][slot].m_item.m_addInfo != 0)
                ++count;
        }
        if (m_nMoney[trader] > 0)
        {
            ++count;
            packet[trader].put_short(count);
            packet[trader].put_short(0);
            packet[trader].put_short(0);
            if (m_pTraders[1 - trader])
                m_pTraders[1 - trader]->getCurCharacInvenW()->gain_money(
                    m_nMoney[trader], (eMoneyAddReason)1, 1, 0);
            if (m_nMoney[trader] % 10000 != 0)
            {
                unsigned int etc = m_pTraders[1 - trader]->SetETC(
                    0x2c4, m_pTraders[trader]->get_acc_id());
                m_pTraders[1 - trader]->getHackAnalyzer()
                    ->addServerHackCnt(m_pTraders[1 - trader],
                                       (WongWork::ENUM_HACKTYPE)0x2c4, 1, etc,
                                       0);
            }
            if ((m_pTraders[trader]->isHackUser() ||
                 m_pTraders[trader]->isHackUserTimer()) &&
                m_nMoney[trader] != 0)
                m_pTraders[1 - trader]->gainMoneyFromHackUser(
                    m_pTraders[trader]->get_acc_id(),
                    (unsigned long long)(long long)m_nMoney[trader], 1, 0);
            if (m_pTraders[trader] && m_pTraders[1 - trader] &&
                m_pTraders[trader]->getCurCharacMoney() == 0)
            {
                CUser* to = m_pTraders[1 - trader];   // -0x50
                CUser* from = m_pTraders[trader];     // -0x4c
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("item_trade.cpp",
                                                       0xc51),
                    true);
                guard.operator*()->operator<<(0xa1);
                guard.operator*()->operator<<(-1);
                SIG_SAVE_GOLD_ZERO_TRADE_LOG* log =
                    guard->GetInBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>();
                log->m_to_m_id = (unsigned)to->get_acc_id();
                std::strncpy(log->m_to_nat_ip, to->m_network.get_str_ip(),
                             0x10);
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
            // ORIG 0x853087a：WriteTradeUp/Down 第三参在清零后读成员（恒 0）
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
            if (m_pTraders[1 - trader] &&
                m_slotsData[trader][slot].m_item.m_addInfo != 0)
            {
                int ret = 0;   // ORIG -0xac 初值 0（+0xde），三分支共享
                // 拍卖均价累计（ORIG +0x91d；-0x3c item ptr 缓存形）
                CAuctionAveragePrice* avgMgr =
                    G_CGameManager()->GetAuctionAveragePriceManager();
                if (avgMgr)
                {
                    Inven_Item* item = &m_slotsData[trader][slot].m_item;
                    unsigned char upgrade = item->GetUpgrade();
                    int avg = avgMgr->GetAveragePrice(item->m_addInfo,
                                                      upgrade);
                    if (item->m_field1 == 2 || item->m_field1 == 3)
                        avg *= item->m_addInfo2;
                    avgPrice[trader] += avg;
                }
                if (m_slotsData[trader][slot].m_item.m_field1
                        == Inven_Item::ITEM_TYPE_AVATAR)  // 头像
                {
                    ret = m_pTraders[1 - trader]->getCurCharacInvenW()
                              ->insertAvatarIntoInventory(
                                  m_slotsData[trader][slot].m_item,
                                  (eAvatarItemAddReason)1, false);
                    if (ret >= 0)
                    {
                        TradeSlot* itemTs = &m_slotsData[trader][slot];  // -0x34
                        _SaveAvatarItemOwnerChange(
                            itemTs->m_item.m_addInfo2, itemTs->m_field04,
                            m_pTraders[1 - trader]->getCurCharacNo(),
                            (unsigned char)ret);
                        DB_InsertCeraItemHistory::makeRequest(
                            itemTs->m_item.m_addInfo2, itemTs->m_field04,
                            m_pTraders[1 - trader]->getCurCharacNo(), 2);
                        // Erase 于发方 mgr、Add 于收方 mgr（ORIG
                        // 0x8531400/0x8531443）；uid 两次重读（+0x0b74/+0x0ba5）
                        WongWork::Avatar_Item* av =
                            m_pTraders[trader]->getCurCharacInvenW()
                                ->GetAvatarItemMgrW()
                                ->EraseAvatarItem(
                                    itemTs->m_item.m_addInfo2);
                        m_pTraders[1 - trader]->getCurCharacInvenW()
                            ->GetAvatarItemMgrW()
                            ->AddAvatarItem(itemTs->m_item.m_addInfo2, av);
                        // 两日志各调一次 GetIPGAgencyNo（P48 无 CSE）
                        m_pTraders[trader]->m_historyLog.AvatarItemDel(
                            itemTs->m_item.m_addInfo,
                            itemTs->m_item.m_addInfo2,
                            av->GetIPGAgencyNo(), (eAvatarItemDelReason)0);
                        m_pTraders[1 - trader]->m_historyLog.AvatarItemAdd(
                            itemTs->m_item.m_addInfo,
                            itemTs->m_item.m_addInfo2,
                            av->GetIPGAgencyNo(), (eAvatarItemAddReason)1);
                    }
                }
                else if (m_slotsData[trader][slot].m_field08 == 7)  // 生物
                {
                    ret = m_pTraders[1 - trader]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  m_slotsData[trader][slot].m_item,
                                  (eItemAddReason)0x1a, false, true);
                    if (ret >= 0)
                    {
                        if (m_slotsData[trader][slot].m_item.m_field1
                                == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                        {
                            if (m_pTraders[trader]->getCurCharacInvenW()
                                     ->GetCreatureMgrW()
                                     ->FindCreatureItem(m_slotsData[trader]
                                                        [slot].m_item
                                                            .m_addInfo2) == 0)
                                return false;
                        }
                        int characNo =
                            m_pTraders[trader]->getCurCharacR()->m_characNo;
                        int size =
                            m_pTraders[trader]->getCurCharacInvenW()
                                ->GetCreatureMgrR()->GetCreatureItemSize() +
                            m_fieldF68[trader];
                        m_pTraders[1 - trader]->getCurCharacInvenW()
                            ->GetCreatureMgrW()
                            ->ChangeOwner(&m_slotsData[trader][slot].m_item,
                                          ret, 0x1a, characNo, size);
                        m_fieldF68[1 - trader] += 1;
                        m_pTraders[trader]->getCurCharacInvenW()
                            ->GetCreatureMgrW()
                            ->DeleteCreatureItem(
                                m_slotsData[trader][slot].m_item
                                    .get_add_info(),
                                0x14);
                        m_pTraders[trader]->m_historyLog.CreatureItemDel(
                            (INVEN_TYPE)3,
                            m_slotsData[trader][slot].m_item.m_addInfo,
                            m_slotsData[trader][slot].m_item.get_add_info(),
                            0, m_slotsData[trader][slot].m_item.m_field1,
                            (eItemDelReason)0x14);
                    }
                }
                else  // 普通物品
                {
                    if (IsTradeLimitAttachTypeItem(
                            m_slotsData[trader][slot].m_item))
                    {
                        DecreaseTradeLimitCount(
                            m_slotsData[trader][slot].m_item);
                        // ORIG +0x1090：记发送方 trader 的 historyLog
                        m_pTraders[trader]->m_historyLog.ChangeItemLimitCount(
                            1, slot,
                            m_slotsData[trader][slot].m_item.m_addInfo, -1,
                            2);
                    }
                    ret = m_pTraders[1 - trader]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  m_slotsData[trader][slot].m_item,
                                  (eItemAddReason)1, true, true);
                    updateMap[m_pTraders[1 - trader]].push_back(ret);
                }

                // 共享负值检查（ORIG +0x11ce）
                if (ret < 0)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xce5, 5)(
                        "insert_item(INVEN_TYPE_INVENTORY) fail - fatal! %d",
                        m_pTraders[1 - trader]->getCurCharacNo());
                    m_slotsData[trader][slot].m_item.reset();
                    return false;
                }
                HistoryLog::WriteTradeUp(
                    (FILE*)m_pTraders[trader]->m_field796f8,
                    m_slotsData[trader][slot].m_item.m_addInfo,
                    m_slotsData[trader][slot].m_item.get_add_info());
                HistoryLog::WriteTradeDown(
                    (FILE*)m_pTraders[1 - trader]->m_field796f8,
                    m_slotsData[trader][slot].m_item.m_addInfo,
                    m_slotsData[trader][slot].m_item.get_add_info());
                m_slotsData[trader][slot].m_item.reset();
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
            return false;
        std::vector<int>& vec = it->second;
        for (std::vector<int>::iterator vit = vec.begin(); vit != vec.end();
             ++vit)
        {
            user->SendUpdateItemList((CUser::eSendTarget)1,
                                     (ENUM_ITEMSPACE)0, *vit);
        }
    }

    for (int trader = 0; trader <= 1; ++trader)
    {
        m_pTraders[trader]->m_historyLog.TradeEnd(
            (eTradeEndReason)2, avgPrice[trader], tradeLogMoney[trader],
            avgPrice[1 - trader], tradeLogMoney[1 - trader]);
        m_pTraders[trader]->GetGoldControl()->TradeComplete(
            tradeLogMoney[trader], tradeLogMoney[1 - trader]);
    }
    return true;
}

// ---- regist_item @ 0x08529e7e ----
// 残差豁免（O=2021/N=2021 等长；canon 级残差为跨版本结构）：
//   [tail-merge] slotIdx==-1 四分支：ORIG je 共享尾块(+0x14bf)，4.4 各分支
//     内联 mov$-1/jmp 岛（×4）；[setcc-fold] +0x640 m_addInfo2<b 的
//     setl/test 物化岛 vs 4.4 直判；[jump-threading] type0 回滚 ==/> 双
//     reset 落块序与 4.4 相反（等长）。
// 注册物品/金钱到交易槽（ORIG 0x08529e7e，2021 条完整转写）：
//   type==0 && a==0 → 金钱（use_money 扣钱 → m_nMoney[trader] += b）；
//   type==0 其余    → 背包物品；type==1 → 头像；type==3 → 装备栏；
//   type==7         → 宠物。取物校验 → delete_item 扣除 → add_item 入槽 →
//   checkTrade/checkCancelTrade 成功发包、失败回滚归还。
// §6.2 巨件核销：P167 227/227 [0缺0多]；gap 0→0（O2021/N2021 等长）；
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：
//   [tail-merge]/[setcc-fold]/[jump-threading] + [N82 全帧槽号错位]
//   （ndiff 796 与语义完成度反向；槽集/调用点 multiset 全等，语义逐分支对照
//   完整），N 封顶。
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

    // ORIG +0x144：switch 二分树（cmp1→type1 远跳、>1: 3/7、==0 fall），
    // 物理块序 case0 → case3(+0xcd7) → case1(+0x14c7) → case7(+0x1bcc) → default
    switch (type) {
    case 0: {
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
                packet.put_packet(g_emptySlot);  // ORIG +0x37a 直引全局
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
            cMyTrace(__PRETTY_FUNCTION__, 0x1a8, 5)(
                "CTradeSpace::regist_item, item find failed(1), ch=%d %d %d %d",
                user->getCurCharacNo(), slot.m_item.m_addInfo,
                slot.m_item.get_add_info(), a);
        }
        if (!_IsTradable(slot.m_item))
            return -0x66;
        // ORIG +0x62f：field1!=1 时数量走 get_add_info() 调用比较
        if (slot.m_item.m_field1 != 1) {
            if (slot.m_item.get_add_info() < b) {
                cMyTrace(__PRETTY_FUNCTION__, 0x1b5, 5)(
                    "CTradeSpace::regist_item, item count failed(1), ch=%d %d %d %d",
                    user->getCurCharacNo(), slot.m_item.m_addInfo,
                    slot.m_item.get_add_info(), b);
                return -8;
            }
        }
        if (b <= 0) {
            cMyTrace(__PRETTY_FUNCTION__, 0x1be, 5)(
                "CTradeSpace::regist_item, item count failed(2), ch=%d %d %d %d",
                user->getCurCharacNo(), slot.m_item.m_addInfo,
                slot.m_item.get_add_info(), b);
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
                TradeSlotAt(m_slotsData, trader, slotIdx)
                    ->m_item.get_add_info());
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
        // ORIG +0xa8f-0xb93：equipable→reset(+0xa9b)；非 equipable：
        // ==b→reset(+0xb95 尾块)、>b→重读链+写回链(+0xaf5/+0xb29/+0xb4b)、
        // <b→reset(+0xb6a)。4.1 无 CSE，条件/读/写各独立全链；两 reset
        // 代码点不合并。
        if (TradeSlotAt(m_slotsData, trader, slotIdx)
                ->m_item.isEquipableItemType()) {
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        } else if (TradeSlotAt(m_slotsData, trader, slotIdx)
                       ->m_item.m_addInfo2 == b) {
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.reset();
        } else if (TradeSlotAt(m_slotsData, trader, slotIdx)
                       ->m_item.m_addInfo2 > b) {
            int cnt = TradeSlotAt(m_slotsData, trader, slotIdx)
                          ->m_item.m_addInfo2;
            TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_addInfo2 =
                cnt - b;
        } else {
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
    case 3: {
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
            // ORIG +0x1098-0x1120：tick 先求值 → 槽链 get_add_info()（非直读）
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                int tick = OS_API::GetDateTimeTick();
                int ai = TradeSlotAt(m_slotsData, trader, slotIdx)
                             ->m_item.get_add_info();
                packet.put_int(
                    user->getCurCharacInvenR()->GetAvatarItemMgrR()
                        ->GetRemainDate(ai, tick));
            } else {
                packet.put_int(
                    TradeSlotAt(m_slotsData, trader, slotIdx)
                        ->m_item.get_add_info());
            }
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(slot.m_item.m_amp.getAbilityType());
            packet.put_short(slot.m_item.m_amp.getAbilityValue());
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                packet.put_int(0x1e);
                packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                                      TradeSlotAt(m_slotsData, trader, slotIdx)
                                          ->m_item.m_addInfo2),
                                  0x1e);
                packet.put_int(4);
                stAvatarExpansionInfo_t info;
                // +0x12a2：addInfo2 重读链（4.1 无 CSE）
                void* pExp = user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    TradeSlotAt(m_slotsData, trader, slotIdx)
                        ->m_item.m_addInfo2);
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
    case 1: {
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
            {
                // ORIG +0x15xx：tick → 槽链 get_add_info() → mgrR（求值序）
                int tick = OS_API::GetDateTimeTick();
                int ai = TradeSlotAt(m_slotsData, trader, slotIdx)
                             ->m_item.get_add_info();
                packet.put_int(
                    user->getCurCharacInvenR()->GetAvatarItemMgrR()
                        ->GetRemainDate(ai, tick));
            }
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(0);
            packet.put_short(0);
            if (TradeSlotAt(m_slotsData, trader, slotIdx)->m_item.m_field1 == 8) {
                packet.put_int(0x1e);
                packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                                      TradeSlotAt(m_slotsData, trader, slotIdx)
                                          ->m_item.m_addInfo2),
                                  0x1e);
                packet.put_int(4);
                stAvatarExpansionInfo_t info;
                void* pExp = user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    TradeSlotAt(m_slotsData, trader, slotIdx)
                        ->m_item.m_addInfo2);
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
    case 7: {
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
                TradeSlotAt(m_slotsData, trader, slotIdx)
                    ->m_item.get_add_info());
            packet.put_byte(slot.m_item.GetItemAttr());
            packet.put_short(slot.m_item.m_fieldb);
            packet.put_int(slot.m_item.m_fieldd);
            packet.put_byte(0);
            packet.put_short(0);
            if (slot.m_item.m_field1 == 5) {
                // ORIG：tick 先行局部、add_info 走槽链 call
                int tick = OS_API::GetDateTimeTick();
                int ai = TradeSlotAt(m_slotsData, trader, slotIdx)
                             ->m_item.get_add_info();
                packet.put_int(
                    user->getCurCharacInvenR()->GetCreatureMgrR()
                        ->GetRemainDate(ai, tick));
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
    default:
        return -0x14;
    }
}
// ============================================================================
// ============================================================================
// remove_item @ 0x0852bfcc —— ORIG 完整转写（O=1767）
// 形态（P157/P160）：ORIG 全程不缓存槽指针，每个字段访问都重做
//   trader*0x798 + trade_slot_no*0x48 + 0x30 两级寻址；本函数用
//   m_slotsData[trader][trade_slot_no] 表达式逐点展开，包视图
//   ((InterfacePacketBuf*)&packet) 内联 lea（ORIG 每 call 前重 lea -0x80）。
// 语义：按 type 分派移除交易槽（0=金币/物品、1=头像、3=装备、7=生物），
// 物品放回 user 背包（插入失败返回负值，不回滚槽位）。
// 参数：trade_slot_no / type / target_slot(pos) / count(num)。
//   - 金币包为 if/else 两 put_int 调用点（ORIG +0x286/0x296/0x2ab）
//   - 广播判定 get_state() 调用两次（ORIG +0x14f1/+0x1501，P48 无 CSE）
//   - case3 取背包实体走成员 Inven_Item GetInvenSlot(int,int) const（sret，
//     ORIG +0x109c _ZNK10CInventory12GetInvenSlotEii）
//   - case0 部分扣减：条件与 newAddInfo 直读槽 m_item.m_addInfo2（ORIG
//     +0xc44/+0xc9c 单 mov），非 get_add_info() 调用
//   - case 1/7 移除后 TradeSlot::Reset()；case 0/3 手动逐字段清槽
// 残差豁免（O=1767/N≈1771，canon 级剩余，跨版本结构性）：
//   [setcc-fold] 3 处 char/bool 返回值 if 判定：4.4 生 test;setne;test;je
//     （ORIG +0x17 IsLocked、+0x481/+0x837 check_empty_count 直判 test;je），
//     无法以合法 C++ 抑制（P22/N 类同族）。
//   [N18b] slotItemType 与 copy.m_item.m_field1 比较：ORIG +0x410-0x41f 两段
//     物化（mov mem,%edx; movzbl; cmp reg,reg），4.4 恒折 cmp MEM,reg。
//   [N76] newAddInfo 减法：ORIG +0xc49-0xc4f store-then-sub mem，4.4 折
//     reg-sub。语义一致。
// §6.2 巨件核销：P167 160/160 [0缺0多]；gap 0→0（O1767/N1767 等长）；
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：
//   [setcc-fold]/[N18b]/[N76] + [N82 全帧槽号错位]（ndiff 650 与语义完成度
//   反向；槽集/调用点 multiset 全等，语义逐分支对照完整），N 封顶。
// ============================================================================
int CTradeSpace::remove_item(CUser* user, int trade_slot_no, int type,
                             int target_slot, int count)
{
    // ORIG +0x17 `test %al,%al; je`：直判形（4.4 对裸 if(char) 生 setne 物化，
    // 显式 !=0 抑制归一尝试）
    if (IsLocked() != 0)
        return -1;
    if (count <= 0)
        return -0x62;

    PacketGuard packet;
    int trader;
    int other;
    CUser* otherUser = 0;   // ORIG +0x40 `movl $0x0,-0x44`：先置零
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
        // P37 单语句链：实参在构造调用前求值（ORIG +0xc8/+0xd8 两 call 先于
        // traceC@0x132），arg1 先 arg2 后
        cMyTrace("int CTradeSpace::remove_item(CUser*, int, int, int, int)",
                 0x45a, 5)
            ("CTradeSpace::remove_item()\t0 == "
             "m_pTraders[owner_index]->getCurCharacR() %x || 0 == "
             "owner->getCurCharacR() %x",
             m_pTraders[trader]->getCurCharacR(), user->getCurCharacR());
        return -1;
    }

    if (m_slotsData[trader][trade_slot_no].m_field08 != type)
        return -1;
    switch (type) {
    case 0:
        if (trade_slot_no == 0) {
            // ---- 金币 ----
            // ORIG +0x1da `js`→负值跳尾块、正值直落（反向条件控布局）
            if (m_nMoney[trader] - count >= 0) {
                user->getCurCharacInvenW()->gain_money(
                    count, (eMoneyAddReason)1, 1, 0);
                m_nMoney[trader] -= count;
                ((InterfacePacketBuf*)&packet)->clear();
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
                ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
                if (m_nMoney[trader] > 0)
                    ((InterfacePacketBuf*)&packet)->put_int(0);
                else
                    ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(m_nMoney[trader]);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                otherUser->Send(packet);
                m_nState[trader] = 0;
                m_nState[other] = 0;
                return 0;
            }
            return -1;
        }
        {
            // ---- 物品（trade_slot_no != 0）----
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
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
            // ORIG +0x99c `cmpl $-0x1; je 尾块`：!=−1 直落发包，−1 跳尾 return
            if (ret != -1) {
            ((InterfacePacketBuf*)&packet)->clear();
            ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
            ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
            // ORIG +0x9ff：EQUIP→全清；非EQUIP仅槽内数量>count 时扣减，否则全清。
            if (m_slotsData[trader][trade_slot_no].m_item.m_field1
                    == Inven_Item::ITEM_TYPE_EQUIP) {
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                m_slotsData[trader][trade_slot_no].m_field00 = 0;
                m_slotsData[trader][trade_slot_no].m_field04 = 0;
                m_slotsData[trader][trade_slot_no].m_field08 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_field1 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_addInfo = 0;
                m_slotsData[trader][trade_slot_no].m_item.set_add_info(0);
                m_slotsData[trader][trade_slot_no].m_item.ResetItemAttr();
                m_slotsData[trader][trade_slot_no].m_item.m_fieldb = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_fieldd = 0;
            } else if (m_slotsData[trader][trade_slot_no].m_item
                           .get_add_info() > count) {
                // 部分移除（ORIG +0xbc3/+0xc0f 两次 get_add_info 调用；
                // newAddInfo 直读槽 m_addInfo2 @+0xc44）
                ((InterfacePacketBuf*)&packet)->put_int(copy.m_item.m_addInfo);
                ((InterfacePacketBuf*)&packet)->put_int(
                    m_slotsData[trader][trade_slot_no].m_item.get_add_info()
                    - count);
                int newAddInfo =
                    m_slotsData[trader][trade_slot_no].m_item.m_addInfo2
                    - count;
                m_slotsData[trader][trade_slot_no].m_item.set_add_info(
                    newAddInfo);
                if (newAddInfo > 0x3e7) {
                    // P37 链式：槽 m_addInfo 实参先于构造求值（ORIG
                    // +0xcb0 mov 0xb(%eax),%ebx 先于 traceC@0xcd1）
                    cMyTrace(
                        "int CTradeSpace::remove_item(CUser*, int, int, int, "
                        "int)",
                        0x503, 5)
                        ("CTradeSpace::remove_item, "
                         "m_ItemSlot[owner_index][trade_slot_no].item."
                         "set_add_info 2 , ITEM #%d, %d",
                         m_slotsData[trader][trade_slot_no].m_item.m_addInfo,
                         newAddInfo);
                }
            } else {
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                m_slotsData[trader][trade_slot_no].m_field00 = 0;
                m_slotsData[trader][trade_slot_no].m_field04 = 0;
                m_slotsData[trader][trade_slot_no].m_field08 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_field1 = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_addInfo = 0;
                m_slotsData[trader][trade_slot_no].m_item.set_add_info(0);
                m_slotsData[trader][trade_slot_no].m_item.ResetItemAttr();
                m_slotsData[trader][trade_slot_no].m_item.m_fieldb = 0;
                m_slotsData[trader][trade_slot_no].m_item.m_fieldd = 0;
            }
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_int(0);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            otherUser->Send(packet);
            }
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
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
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
            // ORIG +0x1047 `movzbl 0xa(%eax)`：读**槽**（非 copy）完整寻址
            if (m_slotsData[trader][trade_slot_no].m_item.m_field1
                    == Inven_Item::ITEM_TYPE_AVATAR) {
                if (target_slot > 9)
                    return -1;
            } else {
                if (target_slot <= 9 || target_slot > 0x15)
                    return -1;
            }
            // ORIG +0x109c：成员 sret 形 _ZNK10CInventory12GetInvenSlotEii
            Inven_Item slotItem =
                user->getCurCharacInvenR()->GetInvenSlot(0, target_slot);
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
            // ORIG +0x1142/+0x114f：实参位直调（level 先于 job 求值，R2L），
            // 结果留 callee-saved 寄存器，不命名局部（P36/P23）
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
            ((InterfacePacketBuf*)&packet)->clear();
            ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
            ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
            ((InterfacePacketBuf*)&packet)->put_int(-1);
            ((InterfacePacketBuf*)&packet)->put_int(0);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_int(0);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(0);
            ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            otherUser->Send(packet);
            m_slotsData[trader][trade_slot_no].m_field00 = 0;
            m_slotsData[trader][trade_slot_no].m_field04 = 0;
            m_slotsData[trader][trade_slot_no].m_field08 = 0;
            m_slotsData[trader][trade_slot_no].m_item.m_field1 = 0;
            m_slotsData[trader][trade_slot_no].m_item.m_addInfo = 0;
            m_slotsData[trader][trade_slot_no].m_item.set_add_info(0);
            m_slotsData[trader][trade_slot_no].m_item.ResetItemAttr();
            m_slotsData[trader][trade_slot_no].m_item.m_fieldb = 0;
            m_slotsData[trader][trade_slot_no].m_item.m_fieldd = 0;
            m_nState[trader] = 0;
            m_nState[other] = 0;
            if (trade_slot_no <= 0xb) {
                // ORIG +0x14f1/+0x1501：get_state 两次独立调用（P48）
                if (user->get_state() != 5 && user->get_state() != 8) {
                    ((InterfacePacketBuf*)&packet)->clear();
                    ((InterfacePacketBuf*)&packet)->put_header(0, 2);
                    ((InterfacePacketBuf*)&packet)->put_byte(0);
                    ((InterfacePacketBuf*)&packet)->put_short(1);
                    user->make_basic_info((char*)&packet, 0);
                    ((InterfacePacketBuf*)&packet)->finalize(true);
                    G_GameWorld()->send_all(packet, user);
                }
            }
            return target_slot;
        }
    case 1:  // ---- 头像 ----
        // ORIG 物理块序：case0(+0x1c4) → case3(+0xf27) → case1(+0x15bb) →
        // case7(+0x186e)（switch 二分树 je 落点决定），源码书写序须一致。
        {
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
            if (copy.m_field08 != 1)
                return -1;
            int ret = m_pTraders[trader]->getCurCharacInvenW()
                          ->insertAvatarIntoSpecificSlot(
                              copy.m_item, target_slot,
                              (eAvatarItemAddReason)4, 1);
            // ORIG +0x16ed `js 尾块`：负值跳 return ret，成功直落
            if (ret >= 0) {
                m_slotsData[trader][trade_slot_no].Reset();
                if (copy.m_field00 != ret) {
                    m_pTraders[trader]->getCurCharacInvenW()
                        ->SendAvatarEvent(0x32, copy.m_field00 + 0xa,
                                          ret + 0xa, copy.m_item.m_addInfo2,
                                          0);
                }
                ((InterfacePacketBuf*)&packet)->clear();
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
                ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                otherUser->Send(packet);
            }
            return ret;
        }
    case 7:  // ---- 生物 ----
        {
            TradeSlot copy = m_slotsData[trader][trade_slot_no];
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
            // ORIG +0x19e7 `js 尾块`：负值跳 return ret，成功直落
            if (ret >= 0) {
                m_slotsData[trader][trade_slot_no].Reset();
                // ORIG +0x1a1c：je→WriteLog 落块尾(+0x1a60)，ChangeSlotNo 直落
                if (copy.m_field00 != ret) {
                    m_pTraders[trader]->getCurCharacInvenW()
                        ->GetCreatureMgrW()
                        ->ChangeSlotNo(&copy.m_item, ret, 0x1d);
                }
                else {
                    m_pTraders[trader]->getCurCharacInvenW()
                        ->WriteCreatureLog(
                            (INVEN_TYPE)3, copy.m_item.m_addInfo,
                            copy.m_item.m_addInfo2, 1, copy.m_item.m_field1,
                            (eItemAddReason)0x1d);
                }
                ((InterfacePacketBuf*)&packet)->clear();
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xf);
                ((InterfacePacketBuf*)&packet)->put_short(trade_slot_no);
                ((InterfacePacketBuf*)&packet)->put_int(-1);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_int(0);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(0);
                ((InterfacePacketBuf*)&packet)->put_packet(g_emptySlot);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                otherUser->Send(packet);
            }
            return ret;
        }
    default:
        return -1;
    }
}
