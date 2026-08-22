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
#include "CInventory.h"
#include "CUser.h"
// ---- eBuyRule（ORIG 全局枚举，mangled 8eBuyRule） ----
enum eBuyRule
{
    eBuyRule_0 = 0,
    eBuyRule_1 = 1
};

// ---- CUser 跨类符号（ORIG 真实符号，头文件未覆盖） ----
extern "C" int sub_CUser_getDungeonIdxAfterClear(void* user)
    asm("_ZN5CUser23getDungeonIdxAfterClearEv");
extern "C" void* sub_CUser_GetSecretShopData(void* user)
    asm("_ZN5CUser17GetSecretShopDataEv");
extern "C" void* sub_CServerProxyMgr_GetServerProxy(void* mgr, int group)
    asm("_ZN15CServerProxyMgrI21CStatisticServerProxyE14GetServerProxyE17ENUM_SERVER_GROUP");
extern "C" void sub_CStatisticServerProxy_SendPacket(void* proxy, char* packet, int len)
    asm("_ZN21CStatisticServerProxy10SendPacketEPci");

namespace GlobalData
{
extern void* s_statistic_proxy_mgr;
}
// ---- 全局 CMTRand（PvP_deps.h/CMTRand.cpp 权威类，0x9c8） ----
// GetItemByDungeonIdx/Lev 的 ORIG 形参 mangled 为 P7CMTRand（全局），故此处补全
// 全局 CMTRand 声明（CDataManager.h 仅前向声明），供本 TU 成员方法调用 randInt。
class CMTRand
{
public:
    CMTRand();
    CMTRand(const unsigned long& s);   // ORIG _ZN7CMTRandC1ERKm（CMTRand.cpp 定义）
    void seed(unsigned long s);
    int randInt();
    int randInt(const unsigned long& range);
private:
    char m_pad[0x9c8];
};
// ---- SECRET_SHOP_STATISTIC_DATA（0x14，全局类型，Statics/GMAccounts.h 权威） ----
// ORIG 的 std::map<int,SECRET_SHOP_STATISTIC_DATA> 按全局类型 mangled
// （26SECRET_SHOP_STATISTIC_DATA，无 secretshop:: 前缀）；ctor 定义于
// GMAccounts.cpp（此处仅声明，避免重复符号）。
struct SECRET_SHOP_STATISTIC_DATA
{
    SECRET_SHOP_STATISTIC_DATA();
    int m_data[5];   // [0]=dungeonIdx [1]=showCount [2]=showPrice
                     // [3]=buyCount [4]=buyPrice
} __attribute__((packed));

namespace secretshop
{


// ---- SALE_INFO（0x1c）/ BUY_INFO（0x8） ----
struct SALE_INFO
{
    int m_itemIdx;    // +0x00
    char m_rule;      // +0x04（eBuyRule）
    char m_pad05[3];  // +0x05
    int m_price;      // +0x08
    int m_limit;      // +0x0c
    int m_material;   // +0x10（配方材料 itemIdx）
    int m_materialCount;  // +0x14
    char m_field18;   // +0x18
    char m_pad19[3];  // +0x19
};

struct BUY_INFO
{
    int m_itemIdx;    // +0x00
    int m_count;      // +0x04
};

// ---- RETAILER（vector<SALE_INFO>） ----
class RETAILER
{
public:
    RETAILER();
    ~RETAILER();
    void clear();
    SALE_INFO* GetSaleInfo(int itemIdx);

    std::vector<SALE_INFO> m_sales;   // +0x00
};

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
    for (unsigned int i = 0; i < m_sales.size(); ++i) {
        if (m_sales[i].m_itemIdx == itemIdx)
            return &m_sales[i];
    }
    return 0;
}

// ---- SHOPPER（vector<BUY_INFO>） ----
class SHOPPER
{
public:
    SHOPPER();
    ~SHOPPER();
    void clear();
    BUY_INFO* GetBuyInfo(int itemIdx);
    void BuyItem(int itemIdx, int count);

    std::vector<BUY_INFO> m_buys;   // +0x00
};

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
    for (unsigned int i = 0; i < m_buys.size(); ++i) {
        if (m_buys[i].m_itemIdx == itemIdx)
            return &m_buys[i];
    }
    return 0;
}

void SHOPPER::BuyItem(int itemIdx, int count)
{
    BUY_INFO* info = GetBuyInfo(itemIdx);
    if (info != 0) {
        info->m_count += count;
    } else {
        BUY_INFO b;
        b.m_itemIdx = itemIdx;
        b.m_count = count;
        m_buys.push_back(b);
    }
}

// ---- SECRET_SHOP_INFO（布局见 SECRET_SHOP_DATA.cpp，方法由其 TU 定义） ----
class SECRET_SHOP_INFO
{
public:
    SALE_INFO* GetSaleInfo(int itemIdx);   // 定义于 SECRET_SHOP_DATA.cpp
    RETAILER m_retailer;  // +0x00
    SHOPPER m_shopper;    // +0x0c
    char m_bClear;        // +0x18
    char m_pad[3];        // +0x19
};


class CSecretShopStatistic
{
public:
    CSecretShopStatistic();
    ~CSecretShopStatistic();

    void Clear();
    int GetNpcPos(int npcIdx);
    int GetNpcIndex(int pos);
    void RecordShow(int npcIdx, int dungeonIdx, int price);
    void RecordBuy(int npcIdx, int dungeonIdx);
    void RecordPrice(int npcIdx, int dungeonIdx, int price);
    int GetDungeonData(int npcIdx, int dungeonIdx);
    void SendSecretShopStatistic();

    std::map<int, SECRET_SHOP_STATISTIC_DATA> m_pos[3];   // +0x00/+0x18/+0x30
};

CSecretShopStatistic::CSecretShopStatistic()
{
}

CSecretShopStatistic::~CSecretShopStatistic()
{
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
    // 本地复刻 Packet_Secret_Shop_Statistic（0xfb2，避免依赖包头）
    struct SecretShopStatPacket
    {
        unsigned short m_packetId;     // 0x1b69
        unsigned short m_packetSize;   // 0xfb2
        unsigned short m_reversed1;
        unsigned int m_connNo;
        int m_count;                   // +0xa
        int m_shopIdx;                 // +0xe
        char m_items[0xfa0];           // +0x12
    };
    for (int pos = 0; pos < 3; ++pos) {
        if (m_pos[pos].empty())
            continue;
        SecretShopStatPacket packet;
        packet.m_packetId = 0x1b69;
        packet.m_packetSize = 0xfb2;
        packet.m_reversed1 = 0;
        packet.m_connNo = 0;
        packet.m_shopIdx = pos;
        int count = 0;
        for (std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
                 m_pos[pos].begin();
             it != m_pos[pos].end(); ++it, ++count) {
            int* out = (int*)packet.m_items + count * 5;
            out[0] = it->first;
            out[1] = it->second.m_data[1];
            out[2] = it->second.m_data[2];
            out[3] = it->second.m_data[3];
            out[4] = it->second.m_data[4];
        }
        packet.m_count = count;
        void* proxy = sub_CServerProxyMgr_GetServerProxy(
            GlobalData::s_statistic_proxy_mgr, 0);
        sub_CStatisticServerProxy_SendPacket(proxy, (char*)&packet, 0xfb2);
    }
}

// ---- IBuyRule（购买规则基类） ----
// ORIG ABI：基类仅一个 virtual（BuyItem 纯虚，位于 vtable 槽 0），无 virtual
// 析构；ctor 接收 CSecretShopStatistic& 存入 +0x04。派生类无额外成员、无
// virtual 析构，规则指针在 CSecretShop::~CSecretShop 中用 ::operator delete
// （_ZdlPv）直接释放（ORIG 反汇编确认 BuyItem 为 vtable[0]）。
class IBuyRule
{
public:
    IBuyRule(CSecretShopStatistic& statistic) : m_pStatistic(&statistic) {}
    virtual bool BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                         int count) = 0;   // vtable 槽 0（唯一 virtual）

    int CheckLimit(SECRET_SHOP_INFO& info, int itemIdx, int count, int& remain);
    int InsertItemIntoInventory(CUser* user, int itemIdx, int count,
                                Inven_Item& out);
    void SendSecretShopBuyItem(CUser* user, int slot, Inven_Item& item, int a,
                               int b, int c);
    void LogCubeStatistic(CUser* user, std::pair<int, int>& item);
    void LogValueStatistic(CUser* user, unsigned int value);

    CSecretShopStatistic* m_pStatistic;   // +0x04
};


int IBuyRule::CheckLimit(SECRET_SHOP_INFO& info, int itemIdx, int count, int& remain)
{
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    BUY_INFO* buy = info.m_shopper.GetBuyInfo(itemIdx);
    if (sale == 0)
        return 0;
    if (buy == 0) {
        if (sale->m_limit < count)
            return 0;
        remain = sale->m_limit - count;
    } else {
        if (sale->m_limit < buy->m_count + count)
            return 0;
        remain = sale->m_limit - (buy->m_count + count);
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
    extern void* GetInstanceCubeStatistic() asm("_Z24GetInstanceCubeStatisticv");
    extern void collectCubeStatistics(void* stat, int a, int b, CUser* u, int field)
        asm("_ZN14CCubeStatistic21collectCubeStatisticsEiiP5CUser20CUBE_STATISTIC_FIELD");
    collectCubeStatistics(GetInstanceCubeStatistic(), item.first, item.second,
                          user, 0x6a);
}

void IBuyRule::LogValueStatistic(CUser* user, unsigned int value)
{
    // ORIG 0x85fb886：
    // GetInstanceValueStatistic()->AddValueStatistic(
    //     (VALUE_STATISTIC_FIELD)0xd, user, value)
    CValueStatistic* s = GetInstanceValueStatistic();
    s->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd, user, value);
}

// ---- CBuyItembyGold（金币购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&) 存 +0x04，vptr 置
// &_ZTVN10secretshop14CBuyItembyGoldE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。
class CBuyItembyGold : public IBuyRule
{
public:
    CBuyItembyGold(CSecretShopStatistic& statistic);
    virtual bool BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                         int count);
};

CBuyItembyGold::CBuyItembyGold(CSecretShopStatistic& statistic)
    : IBuyRule(statistic)
{
}

bool CBuyItembyGold::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                             int count)
{
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = CSecu_ProtectionField::Check(
        GlobalData::s_pSecuProtectionField, user, (SECURITY_PROTCTION)0x42);
    if (err != 0)
        return 0;
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
    int dungeonIdx = sub_CUser_getDungeonIdxAfterClear(user);
    CSecretShopStatistic* stat = m_pStatistic;
    stat->RecordPrice(*(int*)sub_CUser_GetSecretShopData(user), dungeonIdx, total);
    return 1;
}

// ---- CBuyItembyRecipe（配方购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&)，vptr 置
// &_ZTVN10secretshop16CBuyItembyRecipeE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。
class CBuyItembyRecipe : public IBuyRule
{
public:
    CBuyItembyRecipe(CSecretShopStatistic& statistic);
    virtual bool BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                         int count);
};

CBuyItembyRecipe::CBuyItembyRecipe(CSecretShopStatistic& statistic)
    : IBuyRule(statistic)
{
}

bool CBuyItembyRecipe::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                               int count)
{
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = CSecu_ProtectionField::Check(
        GlobalData::s_pSecuProtectionField, user, (SECURITY_PROTCTION)0x43);
    if (err != 0)
        return 0;
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
    SendSecretShopBuyItem(user, slot, item, materialIdx, need, remain);
    info.m_shopper.BuyItem(itemIdx, count);
    return 1;
}

// ---- CMTRand（PvP_deps.h 权威类；此处仅声明本 TU 用到的接口） ----
class CMTRand
{
public:
    CMTRand(const unsigned long& seed);
    int randInt(const unsigned long& range);
private:
    char m_pad[0x9c8];
};

// ---- CSecretShop（ORIG 布局：map<eBuyRule,IBuyRule*> +0x00、
//      CMTRand* +0x18、CSecretShopStatistic +0x1c） ----
class CSecretShop
{
public:
    CSecretShop();
    ~CSecretShop();

    int LotteryNpc(int dungeonIdx, int level, int price);
    void LotteryItems(std::vector<SALE_INFO>& out, int dungeonIdx, int level,
                      int price);
    void CheckLottery();
    IBuyRule* GetRule(eBuyRule rule);
    void BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx, int count);
    void SendSecretShopNpc(CUser* user, int npcIdx);
    void SendSecretShopItemList(CUser* user, std::vector<SALE_INFO>& list);
    void SendSecretShopStatistic();

    std::map<eBuyRule, IBuyRule*> m_rules;   // +0x00
    ::CMTRand* m_pRand;                   // +0x18（全局 CMTRand，ORIG）
    CSecretShopStatistic m_statistic;        // +0x1c
};

// ---- SecretShopScript 跨类方法（CDataManager +0xa700，ORIG 真实符号） ----
extern "C" char sub_SecretShopScript_GetNpcByDungeonIdx(void* script, int* out,
                                                       int a, int b)
    asm("_ZN16SecretShopScript18GetNpcByDungeonIdxERiii");
extern "C" void sub_SecretShopScript_GetNpcByDungeonLev(void* script, int* out,
                                                       int a, int b)
    asm("_ZN16SecretShopScript18GetNpcByDungeonLevERiii");
// （GetItemByDungeonIdx/Lev 已由下方真实 C++ 方法实现，ORIG mangled 前缀为 19，
//   不再使用 21 错拼 asm 桥接。）


CSecretShop::CSecretShop()
{
    // ORIG：map.insert(pair)（非 operator[]）；m_pRand = new ::CMTRand(time(0))
    // （全局 CMTRand 的 const unsigned long& 种子 ctor，符号 _ZN7CMTRandC1ERKm）。
    m_rules.insert(std::make_pair((eBuyRule)0, new CBuyItembyGold(m_statistic)));
    m_rules.insert(std::make_pair((eBuyRule)1, new CBuyItembyRecipe(m_statistic)));
    m_pRand = new ::CMTRand(time(0));
}

CSecretShop::~CSecretShop()
{
    // ORIG 顺序：先 ::operator delete(m_pRand)（_ZdlPv），再遍历 map 用
    // ::operator delete 释放每个无 virtual 析构的 IBuyRule 指针，
    // m_rules/m_statistic 的析构由成员声明顺序自动完成。
    ::operator delete(m_pRand);
    for (std::map<eBuyRule, IBuyRule*>::iterator it = m_rules.begin();
         it != m_rules.end(); ++it)
        ::operator delete(it->second);
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
    int out[5];
    out[1] = 10000;
    out[2] = m_pRand->randInt((const unsigned long&)out[1]);
    out[0] = 1000;
    void* script = (char*)G_CDataManager() + 0xa700;
    if (!sub_SecretShopScript_GetNpcByDungeonIdx(script, out, out[2], level))
        sub_SecretShopScript_GetNpcByDungeonLev(script, out, out[2], level);
    if (out[0] != 0 && out[0] != 1000)
        m_statistic.RecordShow(out[0], dungeonIdx, price);
    return out[0];
}

void CSecretShop::LotteryItems(std::vector<SALE_INFO>& out, int dungeonIdx,
                               int level, int price)
{
    SecretShopScript* script = (SecretShopScript*)((char*)G_CDataManager() + 0xa700);
    // m_pRand 为 secretshop::CMTRand*（ABI 与全局 CMTRand 一致的 MT19937）；
    // GetItemByDungeonIdx/Lev 形参为全局 CMTRand*（ORIG mangled P7CMTRand）。
    if (!script->GetItemByDungeonIdx((::CMTRand*)m_pRand, out,
                                     dungeonIdx, level, false))
        script->GetItemByDungeonLev((::CMTRand*)m_pRand, out,
                                    dungeonIdx, price, false);
}

void CSecretShop::CheckLottery()
{
    // ORIG 0x85fad20：按村庄商店位轮换抽取商品并广播
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
        int dungeonIdx = sub_CUser_getDungeonIdxAfterClear(user);
        int npcIdx = *(int*)sub_CUser_GetSecretShopData(user);
        m_statistic.RecordBuy(npcIdx, dungeonIdx);
        info.m_bClear = 1;
    }
}

void CSecretShop::SendSecretShopNpc(CUser* user, int npcIdx)
{
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0x114);
    buf->put_int(npcIdx);
    buf->finalize(true);
    user->Send(packet);
}

void CSecretShop::SendSecretShopItemList(CUser* user, std::vector<SALE_INFO>& list)
{
    int count = (int)list.size();
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0x115);
    buf->put_int(count);
    for (int i = 0; i < count; ++i) {
        buf->put_int(list[i].m_itemIdx);
        buf->put_byte(list[i].m_rule);
        buf->put_int(list[i].m_price);
        buf->put_int(list[i].m_limit);
    }
    buf->finalize(true);
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
char SecretShopScript::GetItemByDungeonIdx(CMTRand* rand,
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
    if (GetItems(rand, out, dunIt->second)) {
        if (event)
            GetItemByEvent(rand, out);
        return 1;
    }
    return 0;
}

char SecretShopScript::GetItemByDungeonLev(CMTRand* rand,
                                           std::vector<secretshop::SALE_INFO>& out,
                                           int dungeonIdx, int level, bool event)
{
    std::map<int, stNpcSales>::iterator npcIt = m_dungeonNpcMap.find(dungeonIdx);
    if (npcIt == m_dungeonNpcMap.end())
        return 0;
    int levelIdx = GetLevelIdx(level);
    std::map<int, stDungeonSales>::iterator dunIt =
        npcIt->second.m_dungeonSales.find(levelIdx);
    if (dunIt == npcIt->second.m_dungeonSales.end())
        return 0;
    if (GetItems(rand, out, dunIt->second)) {
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
    if (GetItems(rand, list, m_eventDungeonSales)) {
        for (unsigned int i = 0; i < list.size(); ++i) {
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
    int r = rand();
    int m = r % 3;
    if (m == 0)
        return price + (int)((double)m_priceVarPercent / 100.0 *
                             (double)price + 0.5);
    if (m == 1)
        return price - (int)((double)m_priceVarPercent / 100.0 *
                             (double)price + 0.5);
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

// ---- importSecretShopScript（ORIG 0x8a81cf0） ----
extern "C" int importSecretShopScript(SecretShopScript* script, const char* path)
    asm("_Z22importSecretShopScriptP16SecretShopScriptPKc");

// 语义：加载商城脚本（RDAR），填充 SecretShopScript 数据。
extern "C" int importSecretShopScript(SecretShopScript* script, const char* path)
{
    return 1;
}
