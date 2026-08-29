#ifndef GAME_CSECRET_SHOP_H_
#define GAME_CSECRET_SHOP_H_

// df_game_r secretshop 管理类声明（从 CSecretShop.cpp 本地定义块提取为跨 TU
// 唯一声明点；方法实现仍在 CSecretShop.cpp）。提取动机：CParty::dungeon_start
// 需调用 LotteryNpc/SendSecretShopNpc（ORIG T 0x085fabac / 0x085fb2e0）。
// ORIG 符号：CSecretShop C1 0x085fa930 / LotteryNpc 0x085fabac /
//   CheckLottery 0x085fad20 / SendSecretShopNpc 0x085fb2e0 /
//   GetRule 0x085fb27c / BuyItem 0x085fb16e；
//   CSecretShopStatistic 方法 0x085fd1xx 区（SendSecretShopStatistic 0x085fd278）。
// 布局：CSecretShop map<eBuyRule,IBuyRule*> +0x00、CMTRand* +0x18、
//   CSecretShopStatistic +0x1c。

#include <map>
#include <utility>

#include "SECRET_SHOP_DATA.h"   // SALE_INFO / SECRET_SHOP_INFO（namespace secretshop）

struct SECRET_SHOP_STATISTIC_DATA;   // 全局类型（GMAccounts.h 权威，26 前缀）
// ---- eBuyRule（ORIG 全局枚举，mangled 8eBuyRule，非 namespace 内） ----
// ---- eBuyRule（ORIG 全局枚举，mangled 8eBuyRule，非 namespace 内） ----
enum eBuyRule
{
    eBuyRule_0 = 0,
    eBuyRule_1 = 1
};

namespace secretshop
{

// ---- CSecretShopStatistic（3×map<int,SECRET_SHOP_STATISTIC_DATA>，0x48） ----
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

    std::map<int, ::SECRET_SHOP_STATISTIC_DATA> m_pos[3];   // +0x00/+0x18/+0x30
};

// ---- IBuyRule（购买规则基类；唯一 virtual=BuyItem@vtable[0]，无虚析构） ----
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

// ---- CBuyItembyGold / CBuyItembyRecipe（无虚析构，vptr 同基类槽位） ----
class CBuyItembyGold : public IBuyRule
{
public:
    CBuyItembyGold(CSecretShopStatistic& statistic);
    virtual bool BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                         int count);
};

class CBuyItembyRecipe : public IBuyRule
{
public:
    CBuyItembyRecipe(CSecretShopStatistic& statistic);
    virtual bool BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                         int count);
};

// ---- CSecretShop ----
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
    CMTRand* m_pRand;                        // +0x18（全局 CMTRand，PvPTypes.h 权威）
    CSecretShopStatistic m_statistic;        // +0x1c
};

}  // namespace secretshop

#endif  // GAME_CSECRET_SHOP_H_
