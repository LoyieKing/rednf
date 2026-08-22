// df_game_r secretshop::SECRET_SHOP_DATA 还原（G2-5 批次）。
// CParty +0x2a4，总尺寸 0x88。符号（nm -C）：
//   C1 0x082a5422 / D1 0x082a549e / clear 0x08ab14b8 /
//   IsCompleteBuy 0x08ab155a / IsOpen 0x08ab150c / SetBuying 0x08ab1532；
//   附带 SECRET_SHOP_INFO：C1 0x082a5396 / D1 0x082a53dc /
//   clear 0x08ab1642 / GetSaleInfo 0x085fbe8a。
// 布局（由 ctor/clear/IsOpen/SetBuying 反汇编推导）：
//   SECRET_SHOP_DATA：+0x00 int m_openCode、+0x04 SECRET_SHOP_INFO[4]
//   （0x1c 每项）、+0x74 char m_buying[4]、+0x78..+0x87 保留区。
//   SECRET_SHOP_INFO：+0x00 RETAILER（vector<SALE_INFO>）、+0x0c SHOPPER
//   （vector<BUY_INFO>）、+0x18 char m_bClear。
// RETAILER/SHOPPER 类声明与实现见 SECRET_SHOP_DATA.h / CSecretShop.cpp
// （本 TU 不再定义，直接以真实 C++ 成员调用）。

#include "SECRET_SHOP_DATA.h"

namespace secretshop
{

SECRET_SHOP_INFO::SECRET_SHOP_INFO()
    : m_retailer(), m_shopper()
{
}

SECRET_SHOP_INFO::~SECRET_SHOP_INFO()
{
}

void SECRET_SHOP_INFO::clear()
{
    m_bClear = 0;
    m_retailer.clear();
    m_shopper.clear();
}

SALE_INFO* SECRET_SHOP_INFO::GetSaleInfo(int idx)
{
    return m_retailer.GetSaleInfo(idx);
}

SECRET_SHOP_DATA::SECRET_SHOP_DATA()
{
}

SECRET_SHOP_DATA::~SECRET_SHOP_DATA()
{
}

void SECRET_SHOP_DATA::clear()
{
    m_openCode = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_info[i].clear();
        m_buying[i] = 0;
    }
}

int SECRET_SHOP_DATA::IsCompleteBuy()
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_buying[i] != 0)
        {
            return 0;
        }
    }
    return 1;
}

int SECRET_SHOP_DATA::IsOpen()
{
    if (m_openCode == 0 || m_openCode == 1000)
    {
        return 0;
    }
    return 1;
}

void SECRET_SHOP_DATA::SetBuying(int idx, bool buy)
{
    if (idx >= 0 && idx < 4)
    {
        m_buying[idx] = buy ? 1 : 0;
    }
}

}  // namespace secretshop
