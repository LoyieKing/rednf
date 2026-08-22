// df_game_r secretshop 数据域 —— 唯一声明点。
// SECRET_SHOP_DATA：CParty 子对象 +0x2a4，总尺寸 0x88；布局：
//   +0x00 int m_openCode、+0x04 SECRET_SHOP_INFO[4]（0x1c 每项）、
//   +0x74 char m_buying[4]、+0x78..+0x87 保留区。
// SECRET_SHOP_INFO：+0x00 RETAILER（vector<SALE_INFO>，0x0c）、+0x0c SHOPPER
//   （vector<BUY_INFO>，0x0c）、+0x18 char m_bClear。
// ORIG 符号：SECRET_SHOP_DATA C1 0x082a5422 / D1 0x082a549e / clear 0x08ab14b8 /
//   IsCompleteBuy 0x08ab155a / IsOpen 0x08ab150c / SetBuying 0x08ab1532；
//   SECRET_SHOP_INFO C1 0x082a5396 / D1 0x082a53dc / clear 0x08ab1642 /
//   GetSaleInfo 0x085fbe8a（透传 RETAILER::GetSaleInfo 0x08ab12a0）；
//   SHOPPER GetBuyInfo 0x08ab1b2c / BuyItem 0x08ab1cc0。
// 实现分布：RETAILER/SHOPPER 方法在 CSecretShop.cpp；SECRET_SHOP_INFO/
// SECRET_SHOP_DATA 方法在 SECRET_SHOP_DATA.cpp。
#ifndef GAME_SECRET_SHOP_DATA_H_
#define GAME_SECRET_SHOP_DATA_H_

#include <vector>

namespace secretshop
{

// ---- SALE_INFO（0x1c）----
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

// ---- BUY_INFO（0x8）----
struct BUY_INFO
{
    int m_itemIdx;    // +0x00
    int m_count;      // +0x04
};

class RETAILER
{
public:
    RETAILER();
    ~RETAILER();
    void clear();
    SALE_INFO* GetSaleInfo(int itemIdx);

    std::vector<SALE_INFO> m_sales;   // +0x00
};

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

class SECRET_SHOP_INFO
{
public:
    SECRET_SHOP_INFO();
    ~SECRET_SHOP_INFO();
    void clear();
    SALE_INFO* GetSaleInfo(int idx);

    RETAILER m_retailer;  // +0x00
    SHOPPER m_shopper;    // +0x0c
    char m_bClear;        // +0x18
    char m_pad[3];        // +0x19
};

class SECRET_SHOP_DATA
{
public:
    SECRET_SHOP_DATA();
    ~SECRET_SHOP_DATA();
    void clear();
    int IsCompleteBuy();
    int IsOpen();
    void SetBuying(int idx, bool buy);

    int m_openCode;              // +0x00
    SECRET_SHOP_INFO m_info[4];  // +0x04
    char m_buying[4];            // +0x74
    char m_pad[0x10];            // +0x78..+0x87（保留区）
};

}  // namespace secretshop

#endif  // GAME_SECRET_SHOP_DATA_H_
