// ============================================================================
// df_game_r 还原 —— secretshop 域（G4 商城/交易批次）
// 参照 docs/class_func_reports/secretshop__*.md + ORIG 反汇编。
// 覆盖：SALE_INFO/BUY_INFO、RETAILER/SHOPPER、IBuyRule、
// CBuyItembyGold/CBuyItembyRecipe、CSecretShopStatistic、CSecretShop。
// SECRET_SHOP_DATA/SECRET_SHOP_INFO 由 SECRET_SHOP_DATA.cpp 提供。
// Dispatcher_SecretShop* / Timer_SecretShopStatistic 属分发域（G7），
// 本批不实现（GameStubs 桩保留）。
// ============================================================================

#include <map>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "Inven_Item.h"
#include "CDataManager.h"
#include "DNFLexWrapper.h"  // importSecretShopScript 的 Scan*/loadRDARScriptFile
#include "CInventory.h"
#include "CUser.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"
#include "CCubeStatistic.h"
#include "GlobalData.h"
#include "SECRET_SHOP_DATA.h"
#include "CSecretShop.h"   // eBuyRule/CSecretShopStatistic/IBuyRule/CSecretShop 唯一声明点
#include "CParty.h"
#include "CDungeon.h"  // CheckLottery 遍历 m_dungeonMap
#include "PvPTypes.h"  // 权威全局 CMTRand（0x9c8，MT19937；实现见 CMTRand.cpp）
#include "Packet_Secret_Shop_Statistic.h"  // 权威包头（shared/packet/include）

#include <cstring>
// eBuyRule / CSecretShopStatistic / IBuyRule / CSecretShop 类声明已提取至
// CSecretShop.h（跨 TU 唯一声明点，供 CParty::dungeon_start 等调用方）。
// CEventManager 完整定义在 CGuildServerProxy.h（与 GMAccounts.h 重定义冲突，
// 本 TU 不可同时包含）。按 DB_Login.cpp 先例：仅声明 GetRepeatEvent 非虚
// 成员（ORIG T 0x08115998），IsEventing 经 vtable+0x34 槽直调。
class CEventBase;
class CEventManager
{
public:
    CEventBase* GetRepeatEvent(int type);
};

namespace secretshop
{

// ---- SALE_INFO / BUY_INFO（ORIG W 0x08a832aa / 0x08ab1606 逐字段清零；
//      operator== ORIG W 0x08a832ee / 0x08ab161e，sete 直返 bool，
//      供 std::find 实例（0x08a839dd / 0x08ab16ec）比较）----
SALE_INFO::SALE_INFO()
{
    m_itemIdx = 0;
    m_rule = 0;
    m_price = 0;
    m_limit = 0;
    m_material = 0;
    m_materialCount = 0;
    m_field18 = 0;
}

bool SALE_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

BUY_INFO::BUY_INFO()
{
    m_itemIdx = 0;
    m_count = 0;
}

bool BUY_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

RETAILER::RETAILER()
{
}

RETAILER::~RETAILER()
{
}

void RETAILER::clear()
{
    m_sales.clear();
}

SALE_INFO* RETAILER::GetSaleInfo(int itemIdx)
{
    // ORIG T 0x08ab12a0：std::find + ne 比较；迭代器先声明后赋值（P28/P50）。
    std::vector<SALE_INFO>::iterator it;
    it = std::find(m_sales.begin(), m_sales.end(), itemIdx);
    if (it != m_sales.end())
        return &*it;
    return 0;
}

SHOPPER::SHOPPER()
{
}

SHOPPER::~SHOPPER()
{
}

void SHOPPER::clear()
{
    m_buys.clear();
}

BUY_INFO* SHOPPER::GetBuyInfo(int itemIdx)
{
    // ORIG T 0x08ab1344：std::find + ne 比较。
    std::vector<BUY_INFO>::iterator it;
    it = std::find(m_buys.begin(), m_buys.end(), itemIdx);
    if (it != m_buys.end())
        return &*it;
    return 0;
}

void SHOPPER::BuyItem(int itemIdx, int count)
{
    // ORIG T 0x08ab13e8：内联 std::find；命中累加，未命中 ctor+push_back。
    std::vector<BUY_INFO>::iterator it;
    it = std::find(m_buys.begin(), m_buys.end(), itemIdx);
    if (it != m_buys.end()) {
        (*it).m_count += count;   // ORIG 用 operator*（de）
    } else {
        BUY_INFO b;
        b.m_itemIdx = itemIdx;
        b.m_count = count;
        m_buys.push_back(b);
    }
}


CSecretShopStatistic::CSecretShopStatistic()
{
}

CSecretShopStatistic::~CSecretShopStatistic()
{
    for (int i = 0; i < 3; ++i)
        m_pos[i].clear();
}

void CSecretShopStatistic::Clear()
{
    for (int i = 0; i < 3; ++i)
        m_pos[i].clear();
}

int CSecretShopStatistic::GetNpcPos(int npcIdx)
{
    if (npcIdx == 0x3ea)
        return 0;
    if (npcIdx == 0x3eb)
        return 1;
    if (npcIdx == 0x3ec)
        return 2;
    return 0;
}

int CSecretShopStatistic::GetNpcIndex(int pos)
{
    if (pos == 0)
        return 0x3ea;
    if (pos == 1)
        return 0x3eb;
    if (pos == 2)
        return 0x3ec;
    return 0x3ea;
}

int CSecretShopStatistic::GetDungeonData(int npcIdx, int dungeonIdx)
{
    int pos = GetNpcPos(npcIdx);
    std::map<int, SECRET_SHOP_STATISTIC_DATA>& m = m_pos[pos];
    std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it = m.find(dungeonIdx);
    if (it != m.end())
        return (int)&it->second;
    SECRET_SHOP_STATISTIC_DATA data;
    data.m_data[0] = dungeonIdx;
    data.m_data[1] = 0;
    data.m_data[2] = 0;
    data.m_data[3] = 0;
    data.m_data[4] = 0;
    it = m.insert(std::make_pair(dungeonIdx, data)).first;
    return (int)&it->second;
}

void CSecretShopStatistic::RecordShow(int npcIdx, int dungeonIdx, int price)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[1] += 1;
    data->m_data[2] += price;
}

void CSecretShopStatistic::RecordBuy(int npcIdx, int dungeonIdx)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[3] += 1;
}

void CSecretShopStatistic::RecordPrice(int npcIdx, int dungeonIdx, int price)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[4] += price;
}


void CSecretShopStatistic::SendSecretShopStatistic()
{
    // ORIG 0x85fd278：包在循环外构造一次（PacketHeader(0x1b69,0xfb2)，
    // m_count=0）；对每个 pos，若 map 非空：memset 数据区（0xfa0），
    // 逐项拷贝 SECRET_SHOP_STATISTIC_DATA 的全部 5 个 int（m_data[0]=dungeonIdx
    // 由 GetDungeonData 写入），经 s_statistic_proxy_mgr->GetServerProxy(0)
    // ->SendPacket(&packet, 0xfb2) 发送。ORIG 不写 m_count/m_shopIdx。
    Packet_Secret_Shop_Statistic packet;
    for (int pos = 0; pos < 3; ++pos) {
        if (m_pos[pos].empty())
            continue;
        memset(packet.m_items, 0, sizeof(packet.m_items));
        int count = 0;
        for (std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
                 m_pos[pos].begin();
             it != m_pos[pos].end(); ++it, ++count) {
            memcpy(packet.m_items + count * sizeof(SECRET_SHOP_STATISTIC_DATA),
                   &it->second, sizeof(SECRET_SHOP_STATISTIC_DATA));
        }
        CStatisticServerProxy* proxy =
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                (ENUM_SERVER_GROUP)0);
        proxy->SendPacket((char*)&packet, 0xfb2);
    }
}




int IBuyRule::CheckLimit(SECRET_SHOP_INFO& info, int itemIdx, int count, int& remain)
{
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    BUY_INFO* buy = info.m_shopper.GetBuyInfo(itemIdx);
    if (sale == 0)
        return 0;
    if (buy != 0) {
        if (sale->m_limit < buy->m_count + count)
            return 0;
        remain = sale->m_limit - (buy->m_count + count);
    } else {
        if (sale->m_limit < count)
            return 0;
        remain = sale->m_limit - count;
    }
    return 1;
}

int IBuyRule::InsertItemIntoInventory(CUser* user, int itemIdx, int count,
                                      Inven_Item& out)
{
    CDataManager* dm = G_CDataManager();
    CItem* item = dm->find_item(itemIdx);
    if (item == 0)
        return -1;
    item->make_item(out);
    out.m_addInfo = itemIdx;
    if (item->is_stackable())
        out.m_addInfo2 = count;
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    int ret = inven->insertItemIntoInventory(out, (eItemAddReason)0x2c, 1, 0);
    if (ret < 0)
        return -1;
    return ret;
}

void IBuyRule::SendSecretShopBuyItem(CUser* user, int slot, Inven_Item& item,
                                     int a, int b, int c)
{
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(1, 0x129);
    buf->put_byte(1);
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    buf->put_int(inven->get_money());
    buf->put_short(slot);
    buf->put_int(item.m_addInfo);
    buf->put_int(item.m_addInfo2);
    buf->put_byte(item.m_amp.getAbilityType() & 0xff);
    buf->put_short(item.m_amp.getAbilityValue() & 0xffff);
    buf->put_int(a);
    buf->put_int(b);
    buf->put_int(c);
    buf->finalize(true);
    user->Send(packet);
}

void IBuyRule::LogCubeStatistic(CUser* user, std::pair<int, int>& item)
{
    // ORIG 0x85fb8b2：
    // GetInstanceCubeStatistic()->collectCubeStatistics(
    //     item.first, item.second, user, (CUBE_STATISTIC_FIELD)0x6a)
    GetInstanceCubeStatistic()->collectCubeStatistics(
        item.first, item.second, user, (CUBE_STATISTIC_FIELD)0x6a);
}

void IBuyRule::LogValueStatistic(CUser* user, unsigned int value)
{
    // ORIG 0x85fb886：
    // GetInstanceValueStatistic()->AddValueStatistic(
    //     (VALUE_STATISTIC_FIELD)0xd, user, value)
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd,
                                                   user, value);
}

// ---- CBuyItembyGold（金币购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&) 存 +0x04，vptr 置
// &_ZTVN10secretshop14CBuyItembyGoldE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

CBuyItembyGold::CBuyItembyGold(CSecretShopStatistic& statistic)
    : IBuyRule(statistic)
{
}

bool CBuyItembyGold::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                             int count)
{
    // [b21 N] 语义已闭合：err-outer/sale-inner 双守卫、CheckLimit、money、InsertItem、
    // use_money/SendSecretShopBuyItem/LogValueStatistic/BuyItem/RecordPrice 调用链与实参
    // （total/remain/slot/&info.m_shopper/*GetSecretShopData/getDungeonIdxAfterClear）逐条
    // 对齐 ORIG。残余 P197-④ 类：CheckLimit 返 `xor $1` 直测 vs 4.4 `sete` 物化；money
    // 比较 `cmp mem,reg` vs `mov;cmp reg`；RecordPrice 实参求值寄存器缓存序。无子母，封 N。
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x42);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, static_cast<char>(err));
        return 0;
    }
    if (sale == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x11);
        return 0;
    }
    int remain = 0;
    if (!CheckLimit(info, itemIdx, count, remain)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x5f);
        return 0;
    }
    unsigned int total = sale->m_price * count;
    if (user->getCurCharacMoney() < (int)total) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 10);
        return 0;
    }
    Inven_Item item;
    int slot = InsertItemIntoInventory(user, itemIdx, count, item);
    if (slot < 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 4);
        return 0;
    }
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    inven->use_money(total, (eMoneySubReason)0x27, 1);
    SendSecretShopBuyItem(user, slot, item, -1, 0, remain);
    LogValueStatistic(user, total);
    info.m_shopper.BuyItem(itemIdx, count);
    int dungeonIdx = user->getDungeonIdxAfterClear();
    CSecretShopStatistic* stat = m_pStatistic;
    stat->RecordPrice(*(int*)user->GetSecretShopData(), dungeonIdx, total);
    return 1;
}

// ---- CBuyItembyRecipe（配方购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&)，vptr 置
// &_ZTVN10secretshop16CBuyItembyRecipeE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

CBuyItembyRecipe::CBuyItembyRecipe(CSecretShopStatistic& statistic)
    : IBuyRule(statistic)
{
}

bool CBuyItembyRecipe::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                               int count)
{
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x43);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, static_cast<char>(err));
        return 0;
    }
    if (sale == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x11);
        return 0;
    }
    int remain = 0;
    if (!CheckLimit(info, itemIdx, count, remain)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x5f);
        return 0;
    }
    int materialIdx = sale->m_material;
    int need = sale->m_materialCount * count;
    std::pair<int, int> cubeStat;
    cubeStat.first = materialIdx;
    cubeStat.second = need;
    CInventory* invenR = (CInventory*)user->getCurCharacInvenR();
    Inven_Item material;
    int materialSlot = invenR->GetInvenData(materialIdx, material);
    if (materialSlot == -1 || material.m_addInfo2 < need) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 10);
        return 0;
    }
    Inven_Item item;
    int slot = InsertItemIntoInventory(user, itemIdx, count, item);
    if (slot < 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 4);
        return 0;
    }
    CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
    invenW->delete_item((INVEN_TYPE)1, materialSlot, need, (eItemDelReason)0x1a, 1);
    const CInventory* invenR2 = user->getCurCharacInvenR();
    const Inven_Item* ref = invenR2->GetInvenRef(1, materialSlot);
    int remain2 = (ref != 0) ? ref->m_addInfo2 : 0;
    SendSecretShopBuyItem(user, slot, item, materialIdx, need, remain2);
    LogCubeStatistic(user, cubeStat);
    info.m_shopper.BuyItem(itemIdx, count);
    return 1;
}


// ---- CSecretShop（ORIG 布局：map<eBuyRule,IBuyRule*> +0x00、
//      CMTRand* +0x18、CSecretShopStatistic +0x1c） ----

// ---- SecretShopScript 跨类方法（CDataManager +0xa700，ORIG 真实符号；
//      声明见 CDataManager.h，本 TU 直接真实调用） ----

CSecretShop::CSecretShop()
{
    // ORIG：map.insert(pair)（非 operator[]）；m_pRand = new CMTRand(time(0))
    // （PvPTypes.h 权威 CMTRand 的 const unsigned long& 种子 ctor，
    //  符号 _ZN7CMTRandC1ERKm @0x080cba2a；ORIG 无 CMTRand(time_t) 重载，
    //  time(0) 经临时 unsigned long 绑定 const 引用）。
    m_rules.insert(std::make_pair((eBuyRule)0, new CBuyItembyGold(m_statistic)));
    m_rules.insert(std::make_pair((eBuyRule)1, new CBuyItembyRecipe(m_statistic)));
    m_pRand = new CMTRand(time(0));
}

CSecretShop::~CSecretShop()
{
    // ORIG 顺序：先 ::operator delete(m_pRand)（_ZdlPv），再遍历 map 用
    // ::operator delete 释放每个无 virtual 析构的 IBuyRule 指针，
    // m_rules/m_statistic 的析构由成员声明顺序自动完成。
    if (m_pRand != 0)
        ::operator delete(m_pRand);
    for (std::map<eBuyRule, IBuyRule*>::iterator it = m_rules.begin();
         it != m_rules.end(); ++it)
        ::operator delete(it->second);
    m_rules.clear();
}

IBuyRule* CSecretShop::GetRule(eBuyRule rule)
{
    // ORIG：`if (it != end) return it->second; return 0;`（ne 比较，非 eq+取反）。
    std::map<eBuyRule, IBuyRule*>::iterator it = m_rules.find(rule);
    if (it != m_rules.end())
        return it->second;
    return 0;
}

int CSecretShop::LotteryNpc(int dungeonIdx, int level, int price)
{
    unsigned long randomSeed = 10000;
    int randVal = m_pRand->randInt(randomSeed);
    int npcIdx = 1000;
    register unsigned char fv asm("eax") =
        (unsigned char)G_CDataManager()->m_secretShop
            .GetNpcByDungeonIdx(npcIdx, randVal, dungeonIdx);
    fv ^= 1;
    if (fv)
        G_CDataManager()->m_secretShop.GetNpcByDungeonLev(npcIdx, randVal, level);
    if (npcIdx != 0 && npcIdx != 1000)
        m_statistic.RecordShow(npcIdx, dungeonIdx, price);
    return npcIdx;
}

void CSecretShop::LotteryItems(std::vector<SALE_INFO>& out, int dungeonIdx,
                               int level, int price)
{
    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（GetItemByDungeonIdx /
    // GetItemByDungeonLev 各一处，共 2 次）。
    // m_pRand 即 PvPTypes.h 权威全局 CMTRand；GetItemByDungeonIdx/Lev 形参
    // ORIG 0x085faca2：event = s_event_manager->GetRepeatEvent(0x3a)
    // ->IsEventing((CUser*)0)（vtable slot 13），作 GetItemByDungeonIdx/Lev
    // 第 5 参（命中时额外 GetItemByEvent 追加事件商品）。CEventManager 完整
    // 定义在 CGuildServerProxy.h（与 GMAccounts.h 重定义冲突不可同时包含），
    // 按 DB_Login.cpp 先例：GetRepeatEvent 用前置 CEventManager 的成员声明，
    // IsEventing 经 vtable+0x34 槽直调（ORIG 指令形态）。
    typedef bool (*IsEventingFn)(CEventBase*, int);
    register CEventBase* ev asm("eax") =
        GlobalData::s_event_manager->GetRepeatEvent(0x3a);
    bool event = (*reinterpret_cast<IsEventingFn*>(
                      *reinterpret_cast<void**>(ev) + 0x34))(ev, 0);
    if (G_CDataManager()->m_secretShop.GetItemByDungeonIdx(m_pRand, out, dungeonIdx, level, event))
    {
    }
    else
        G_CDataManager()->m_secretShop.GetItemByDungeonLev(m_pRand, out, dungeonIdx, price, event);
}

void CSecretShop::CheckLottery()
{
    // ORIG 0x85fad20：统计工具 —— 对 CDataManager 地下城表中每个地下城，
    // 以 dungeonIdx=0x3ea(1002)、level=get_index()、price=get_standard_level()
    // 抽取 10000 次 LotteryItems，统计各商品出现次数，并把百分比写入
    // secretshop_lottery.txt。
    std::vector<SALE_INFO> list;
    std::map<int, int> counts;
    FILE* fp = fopen("secretshop_lottery.txt", "wt");
    if (fp == 0)
    {
        return;
    }
    __gnu_cxx::hash_map<int, CDungeon*>& dungeons =
        G_CDataManager()->m_dungeonList->m_dungeonMap;
    for (__gnu_cxx::hash_map<int, CDungeon*>::iterator it = dungeons.begin();
         it != dungeons.end(); ++it)
    {
        CDungeon* dungeon = it->second;
        counts.clear();
        for (int i = 0; i < 10000; ++i)
        {
            LotteryItems(list, 0x3ea, dungeon->get_index(),
                         dungeon->get_standard_level());
            for (std::size_t j = 0; j < list.size(); ++j)
            {
                std::map<int, int>::iterator c =
                    counts.find(list[j].m_itemIdx);
                if (c == counts.end())
                {
                    counts.insert(std::make_pair(list[j].m_itemIdx, 1));
                }
                else
                {
                    ++c->second;
                }
            }
            list.clear();
        }
        fprintf(fp, "Dungeon : %s(idx:%d, lev:%d)\n",
                dungeon->GetDungeonName(), dungeon->get_index(),
                dungeon->get_standard_level());
        int total = 0;
        for (std::map<int, int>::iterator c = counts.begin();
             c != counts.end(); ++c)
        {
            total += c->second;
        }
        for (std::map<int, int>::iterator c = counts.begin();
             c != counts.end(); ++c)
        {
            fprintf(fp, "- %d(%d, %.2f%%)\n", c->first, c->second,
                    100.0 * ((double)c->second / (double)total));
        }
        fputc('\n', fp);
    }
    fclose(fp);
}

void CSecretShop::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx, int count)
{
    SALE_INFO* sale = info.GetSaleInfo(itemIdx);
    if (sale == 0)
        return;
    IBuyRule* rule = GetRule((eBuyRule)sale->m_rule);
    if (rule == 0)
        return;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return;
    if (!item->is_stackable())
        count = 1;
    if (rule->BuyItem(user, info, itemIdx, count) &&
        info.m_bClear != 1) {
        int dungeonIdx = user->getDungeonIdxAfterClear();
        int npcIdx = *(int*)user->GetSecretShopData();
        m_statistic.RecordBuy(npcIdx, dungeonIdx);
        info.m_bClear = 1;
    }
}

void CSecretShop::SendSecretShopNpc(CUser* user, int npcIdx)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x114);
    ((InterfacePacketBuf*)&packet)->put_int(npcIdx);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}

void CSecretShop::SendSecretShopItemList(CUser* user, std::vector<SALE_INFO>& list)
{
    int count = (int)list.size();
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x115);
    ((InterfacePacketBuf*)&packet)->put_int(count);
    for (int i = 0; i < count; ++i) {
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_itemIdx);
        ((InterfacePacketBuf*)&packet)->put_byte(list[i].m_rule);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_price);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_limit);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_material);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_materialCount);
        ((InterfacePacketBuf*)&packet)->put_byte(list[i].m_field18);
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}

void CSecretShop::SendSecretShopStatistic()
{
    m_statistic.SendSecretShopStatistic();
    m_statistic.Clear();
}

}  // namespace secretshop
// ============================================================================
// SecretShopScript::GetItemByDungeonIdx / GetItemByDungeonLev（ORIG 0x08a82a0e /
// 0x08a82b34）。依赖链：GetItems -> CopyItem/IsExistItem/GetDomainRate，GetLevelIdx，
// GetItemByEvent -> GetItems。全部按 class_func_reports/SecretShopScript 忠实转写。
// ============================================================================
bool SecretShopScript::GetItemByDungeonIdx(CMTRand* rand,
                                           std::vector<secretshop::SALE_INFO>& out,
                                           int dungeonIdx, int level, bool event)
{
    std::map<int, stNpcSales>::iterator npcIt = m_dungeonNpcMap.find(dungeonIdx);
    if (npcIt == m_dungeonNpcMap.end())
        return 0;
    std::map<int, stDungeonSales>::iterator dunIt =
        npcIt->second.m_dungeonSales.find(level);
    if (dunIt == npcIt->second.m_dungeonSales.end())
        return 0;
    register unsigned char fv asm("eax") =
        (unsigned char)GetItems(rand, out, dunIt->second);
    fv ^= 1;
    if (fv == 0) {
        if (event)
            GetItemByEvent(rand, out);
        return 1;
    }
    return 0;
}

bool SecretShopScript::GetItemByDungeonLev(CMTRand* rand,
                                           std::vector<secretshop::SALE_INFO>& out,
                                           int dungeonIdx, int level, bool event)
{
    std::map<int, stNpcSales>::iterator npcIt = m_dungeonNpcMap.find(dungeonIdx);
    if (npcIt == m_dungeonNpcMap.end())
        return 0;
    int levelIdx = GetLevelIdx(level);
    std::map<int, stDungeonSales>::iterator dunIt =
        npcIt->second.m_eventSales.find(levelIdx);
    if (dunIt == npcIt->second.m_eventSales.end())
        return 0;
    register unsigned char fv asm("eax") =
        (unsigned char)GetItems(rand, out, dunIt->second);
    fv ^= 1;
    if (fv == 0) {
        if (event)
            GetItemByEvent(rand, out);
        return 1;
    }
    return 0;
}

char SecretShopScript::GetItemByEvent(CMTRand* rand,
                                      std::vector<secretshop::SALE_INFO>& out)
{
    std::vector<secretshop::SALE_INFO> list;
    register unsigned char fv asm("eax") =
        (unsigned char)GetItems(rand, list, m_eventDungeonSales);
    fv ^= 1;
    if (fv == 0) {
        for (int i = 0; i < static_cast<int>(list.size()); ++i) {
            list[i].m_field18 = 1;
            out.push_back(list[i]);
        }
        return 1;
    }
    return 0;
}

char SecretShopScript::GetItems(CMTRand* rand,
                                std::vector<secretshop::SALE_INFO>& out,
                                stDungeonSales& sales)
{
    if ((int)sales.m_sales.size() < sales.m_maxCount) {
        // 商品数少于上限：直接全部列出
        for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
             it != sales.m_sales.end(); ++it) {
            secretshop::SALE_INFO sale;
            CopyItem(sale, it->second);
            out.push_back(sale);
        }
    }
    else {
        // 按权重随机抽取，直到填满 m_maxCount 或商品重复
        unsigned long total = 0;    // 累计 rate
        unsigned long target = 0;
        int guard = 0;
        do {
            if ((int)out.size() >= sales.m_maxCount)
                return 1;
            total = (unsigned long)GetDomainRate(sales);
            target = (unsigned long)rand->randInt(total);
            unsigned long acc = 0;
            for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
                 it != sales.m_sales.end(); ++it) {
                acc += (unsigned long)it->second.m_rate;
                if (target < acc) {
                    if (!IsExistItem(out, it->second.m_itemIdx)) {
                        secretshop::SALE_INFO sale;
                        CopyItem(sale, it->second);
                        out.push_back(sale);
                    }
                    break;
                }
            }
            ++guard;
        } while (guard <= 0x2710);
    }
    return 1;
}

bool SecretShopScript::IsExistItem(std::vector<secretshop::SALE_INFO>& out,
                                   int itemIdx)
{
    for (unsigned int i = 0; i < out.size(); ++i)
        if (out[i].m_itemIdx == itemIdx)
            return true;
    return false;
}

int SecretShopScript::GetDomainRate(stDungeonSales& sales)
{
    int total = 0;
    for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
         it != sales.m_sales.end(); ++it)
        total += it->second.m_rate;
    return total;
}

int SecretShopScript::GetLevelIdx(int level)
{
    for (std::set<stLevelSection>::iterator it = m_levelSections.begin();
         it != m_levelSections.end(); ++it) {
        if ((*it).m_minLev <= level && level <= (*it).m_maxLev)
            return (*it).m_levelIdx;
    }
    return 0;
}

int SecretShopScript::GetRandItemPrice(int price)
{
    int m = rand() % 3;
    if (m == 0)
        return price + (int)((double)price * ((double)m_priceVarPercent / 100.0));
    if (m == 1)
        return price - (int)((double)price * ((double)m_priceVarPercent / 100.0));
    return price;
}

void SecretShopScript::CopyItem(secretshop::SALE_INFO& sale, stSaleInfo& info)
{
    sale.m_itemIdx = info.m_itemIdx;
    sale.m_rule = info.m_rule;
    sale.m_limit = info.m_limit;
    if (sale.m_rule == (char)0) {
        sale.m_price = GetRandItemPrice(info.m_price);
    } else if (sale.m_rule == (char)1) {
        sale.m_material = info.m_price;
        sale.m_materialCount = info.m_materialCount;
    }
}

// ---- SecretShopScript::clear（ORIG 0x8372020 W）----
void SecretShopScript::clear()
{
    m_priceVarPercent = 0;   // +0x80
    m_int84 = 0;             // +0x84
    m_npcMap.clear();        // +0x00
    m_levelSections.clear(); // +0x30
    m_levelNpcMap.clear();   // +0x18
    m_dungeonNpcMap.clear(); // +0x48
}

// ---- SecretShopScript::verifyNpc（ORIG 0x8a82804）----
void SecretShopScript::verifyNpc(std::multimap<int, stDungeonNpc>& npcMap)
{
    int total = 0;
    for (std::multimap<int, stDungeonNpc>::iterator it = npcMap.begin();
         it != npcMap.end(); ++it)
    {
        std::pair<std::multimap<int, stDungeonNpc>::iterator,
                  std::multimap<int, stDungeonNpc>::iterator> range =
            npcMap.equal_range(it->first);
        total = 0;
        for (std::multimap<int, stDungeonNpc>::iterator r = range.first;
             r != range.second; ++r)
        {
            total += r->second.m_rate;
        }
        if (total != 10000)
        {
            printf("[SecretShop Error] NPC ratio is wrong!(index:%d)\n",
                   range.first->second.m_field0);
        }
    }
}

// ---- importSecretShopScript（ORIG 0x8a81cf0，_Z22importSecretShopScriptP16SecretShopScriptPKc）----

// 按 docs/class_func_reports/_lt_global_gt_/importSecretShopScript.md 忠实转写：
//   - 所有取值扫描均为 int ScanInt(bool* ok) 形式；占位扫描传 (bool*)0
//     （ORIG movl $0x0 压参），成功标志仅用于条目循环终止判断。
//   - [dungeon npc]/[level npc]：每条 7 次扫描（key/npcIdx/rate 带 flag +
//     4 次 flag=0 占位，ORIG +0x11c 循环 i=0..3），循环至 flag 失败后
//     verifyNpc 对应 multimap。
//   - [npc]：npcKey 存入局部 stNpcSales::m_field0；内层 token 循环处理
//     [dungeon]/[level]（先 dungeonKey/maxCount 两次占位扫描，随后 itemIdx
//     带 flag 的商品循环）、[/npc]（整表插入 m_dungeonNpcMap）；
//     [dungeon cash item] 为无操作继续。
//   - [event item] 在外层：首个占位扫描写 m_eventDungeonSales.m_maxCount
//     （ORIG script+0x64），其后 itemIdx 带 flag 的商品循环插入 m_sales。
//   - stDungeonNpc 各字段与 key/npcIdx/rate 的对应为 [推断]（ORIG 经局部
//     辅助构造函数写入，未逐指令核对）。
int importSecretShopScript(SecretShopScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    script->clear();
    std::string token;
    bool done = false;
    while (!done && ScanType(token, true))
    {
        done = (token == "[secret shop end]");
        if (!done && (token == "[dungeon npc]" || token == "[level npc]"))
        {
            std::multimap<int, stDungeonNpc>& npcMap =
                (token == "[dungeon npc]") ? script->m_npcMap
                                           : script->m_levelNpcMap;
            for (;;)
            {
                bool ok = false;
                int key = ScanInt(&ok);
                if (!ok)
                    break;
                int npcIdx = ScanInt(&ok);
                if (!ok)
                    break;
                int rate = ScanInt(&ok);
                if (!ok)
                    break;
                for (int i = 0; i <= 3; ++i)
                    ScanInt((bool*)0);   // 4 个占位扫描（ORIG +0x11c）
                stDungeonNpc npc;
                npc.m_field0 = key;      // [推断]
                npc.m_npcIdx = npcIdx;
                npc.m_rate = rate;
                npcMap.insert(std::make_pair(key, npc));
            }
            script->verifyNpc(npcMap);
        }
        else if (!done && token == "[level section]")
        {
            for (;;)
            {
                bool ok = false;
                stLevelSection section;
                section.m_levelIdx = ScanInt(&ok);
                if (!ok)
                    break;
                section.m_minLev = ScanInt((bool*)0);
                section.m_maxLev = ScanInt((bool*)0);
                script->m_levelSections.insert(section);
            }
        }
        else if (!done && token == "[npc]")
        {
            stNpcSales sales;
            sales.m_field0 = ScanInt((bool*)0);   // npcKey
            std::string sub;
            while (ScanType(sub, true))
            {
                if (sub == "[/npc]")
                {
                    script->m_dungeonNpcMap.insert(
                        std::make_pair(sales.m_field0, sales));
                    break;
                }
                if (sub == "[dungeon]" || sub == "[level]")
                {
                    stDungeonSales ds;
                    ds.m_field0 = ScanInt((bool*)0);       // dungeonKey
                    ds.m_maxCount = ScanInt((bool*)0);     // +0x04
                    for (;;)
                    {
                        bool ok = false;
                        stSaleInfo sale;
                        sale.m_itemIdx = ScanInt(&ok);
                        if (!ok)
                            break;
                        sale.m_rule = (char)ScanInt((bool*)0);
                        sale.m_price = ScanInt((bool*)0);
                        sale.m_materialCount = ScanInt((bool*)0);
                        sale.m_limit = ScanInt((bool*)0);
                        sale.m_rate = ScanInt((bool*)0);
                        ds.m_sales.insert(std::make_pair(sale.m_itemIdx, sale));
                    }
                    if (sub == "[dungeon]")
                        sales.m_dungeonSales.insert(
                            std::make_pair(ds.m_field0, ds));
                    else
                        sales.m_eventSales.insert(
                            std::make_pair(ds.m_field0, ds));
                }
                // "[dungeon cash item]" 及未知 token：无操作继续内层循环
            }
        }
        else if (!done && token == "[event item]")
        {
            script->m_eventDungeonSales.m_maxCount = ScanInt((bool*)0);  // +0x64
            for (;;)
            {
                bool ok = false;
                stSaleInfo sale;
                sale.m_itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                sale.m_rule = (char)ScanInt((bool*)0);
                sale.m_price = ScanInt((bool*)0);
                sale.m_materialCount = ScanInt((bool*)0);
                sale.m_limit = ScanInt((bool*)0);
                sale.m_rate = ScanInt((bool*)0);
                script->m_eventDungeonSales.m_sales.insert(
                    std::make_pair(sale.m_itemIdx, sale));
            }
        }
        else if (!done && token == "[price random]")
        {
            script->m_priceVarPercent = ScanInt((bool*)0);   // +0x80
        }
        else if (!done && token == "[eplp wait time]")
        {
            script->m_int84 = ScanInt((bool*)0);             // +0x84
        }
        // "[add slot cash user stackable count]"：无操作继续
    }
    return 1;
}

void* CUser::GetSecretShopData()
{
    CParty* party = static_cast<CParty*>(GetParty());
    if (party != 0)
        return party->GetSecretShopData();
    return 0;
}
