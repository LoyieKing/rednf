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
// RETAILER/SHOPPER 属独立批次类，其 ctor/dtor/clear/GetSaleInfo 以
// extern asm 标签引用（ORIG 真实符号）。

namespace secretshop
{

class RETAILER
{
public:
    char m_pad[0x0c];
};

class SHOPPER
{
public:
    char m_pad[0x0c];
};

// ---- RETAILER / SHOPPER 跨类方法（ORIG 真实符号）----
extern "C" void sub_RETAILER_ctor(void* self) asm("_ZN10secretshop8RETAILERC1Ev");
extern "C" void sub_RETAILER_dtor(void* self) asm("_ZN10secretshop8RETAILERD1Ev");
extern "C" void sub_RETAILER_clear(void* self) asm("_ZN10secretshop8RETAILER5clearEv");
extern "C" void* sub_RETAILER_GetSaleInfo(void* self, int idx)
    asm("_ZN10secretshop8RETAILER11GetSaleInfoEi");
extern "C" void sub_SHOPPER_ctor(void* self) asm("_ZN10secretshop7SHOPPERC1Ev");
extern "C" void sub_SHOPPER_dtor(void* self) asm("_ZN10secretshop7SHOPPERD1Ev");
extern "C" void sub_SHOPPER_clear(void* self) asm("_ZN10secretshop7SHOPPER5clearEv");

class SECRET_SHOP_INFO
{
public:
    SECRET_SHOP_INFO();
    ~SECRET_SHOP_INFO();
    void clear();
    void* GetSaleInfo(int idx);

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

SECRET_SHOP_INFO::SECRET_SHOP_INFO()
{
    sub_RETAILER_ctor(&m_retailer);
    sub_SHOPPER_ctor(&m_shopper);
}

SECRET_SHOP_INFO::~SECRET_SHOP_INFO()
{
    sub_SHOPPER_dtor(&m_shopper);
    sub_RETAILER_dtor(&m_retailer);
}

void SECRET_SHOP_INFO::clear()
{
    m_bClear = 0;
    sub_RETAILER_clear(&m_retailer);
    sub_SHOPPER_clear(&m_shopper);
}

void* SECRET_SHOP_INFO::GetSaleInfo(int idx)
{
    return sub_RETAILER_GetSaleInfo(&m_retailer, idx);
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
