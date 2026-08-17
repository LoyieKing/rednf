// ============================================================================
// df_game_r 还原 —— STCeraShopItem / CCeraShopGoods（商城商品，G4 商城/交易域）
// 参照 docs/class_func_reports/STCeraShopItem.md、CCeraShopGoods.md +
// ORIG 反汇编逐函数核对。布局见 CItemList.h。
// ============================================================================

#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "CItemList.h"
#include "CDataManager.h"

// ============================================================================
// STCeraShopItem
// ============================================================================

// ---- C1 @ 0x88fd974 ----
STCeraShopItem::STCeraShopItem()
{
    new (&m_name) std::string();
    clear();
    m_field84 = 0;
}

// ---- clear @ 0x88fd9c4 ----
void STCeraShopItem::clear()
{
    m_field0 = (char)0xff;
    m_field4 = 0;
    m_field8 = 0xfffffffe;
    m_fieldc = 0xffffffff;
    m_field10 = 0;
    m_field14 = 0xffffffff;
    m_field18 = (char)0xff;
    m_field1c = 1;
    m_field20 = 0;
    m_field22 = 0;
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0;
    m_field38 = 0;
    m_field34 = 0;
    m_field3c = (char)0xff;
    m_name = "";
    m_field80 = 0;
    m_field44 = 0;
    m_field48 = 0;
    m_field4c = 0;
    m_field50 = 0;
    m_field51 = 0;
    m_field52 = 0;
    m_field53 = 1;
    m_field54 = 0xfffffffe;
    m_field58 = 0xffffffff;
    m_field5c = 0;
    m_field60 = (char)0xc;
    m_field62 = 0;
    m_field64 = 0;
    m_field68 = 0;
    m_field6c = 0;
    m_field70 = 0;
    m_field74 = 0;
    m_field78 = 0;
    m_field28 = 0;
    m_field7c = 0xffffffff;
}

// ---- 拷贝构造 @ 0x8923e28（逐字节拷贝 + COW string 拷贝） ----
STCeraShopItem::STCeraShopItem(const STCeraShopItem& other)
{
    new (&m_name) std::string(other.m_name);
    memcpy(&m_field44, &other.m_field44, sizeof(*this) - offsetof(STCeraShopItem, m_field44));
}

STCeraShopItem::~STCeraShopItem()
{
    m_name.~basic_string();
}

// ---- operator= @ 0x892408a ----
STCeraShopItem& STCeraShopItem::operator=(const STCeraShopItem& other)
{
    if (this != &other) {
        memcpy(this, &other, 0x40);
        m_name = other.m_name;
        memcpy(&m_field44, &other.m_field44, sizeof(*this) - offsetof(STCeraShopItem, m_field44));
    }
    return *this;
}

// ---- operator== @ 0x891a89e ----
bool STCeraShopItem::operator==(const STCeraShopItem& other)
{
    return m_field8 == other.m_field8;
}

// ---- 访问器 ----
int STCeraShopItem::getCeraPrice()
{
    return m_field38;
}

int STCeraShopItem::getGoldPrice()
{
    return m_field2c;
}

bool STCeraShopItem::isLimitSaleItem()
{
    return m_field54 != -2;
}

// ============================================================================
// CCeraShopGoods
// ============================================================================

// ---- SetItem @ 0x8510c1a ----
void CCeraShopGoods::SetItem(const STCeraShopItem& item)
{
    m_itemIdx = item.m_field14;
    m_stackCount = item.m_field1c;
    m_goldPrice = item.m_field2c;
    m_winPoint = item.m_field30;
    m_ceraPrice = item.m_field38;
    m_groupIdx = item.m_fieldc;
    m_featuredIdx = item.m_field10;
    m_sticker = (int)(signed char)item.m_field20;
    m_expiryDate = (int)(short)item.m_field22;
    m_bonusWeight = item.m_field44;
    m_field30 = item.m_field4c;
    m_giftDisallow = item.m_field50;
    m_ceraPointDisallow = item.m_field51;
    m_stackBuyable = item.m_field53;
    m_pItem = G_CDataManager()->find_item(m_itemIdx);
    m_mileagePrice = item.m_field34;
    m_subGroupIdx = (int)(signed char)item.m_field0;
}

// ---- C1 @ 0x8514ea0 ----
CCeraShopGoods::CCeraShopGoods(const STCeraShopItem& item)
{
    SetItem(item);
}

// ---- weak 访问器 ----
int CCeraShopGoods::GetItemIdx() const
{
    return m_itemIdx;
}

int CCeraShopGoods::GetStackCount() const
{
    return m_stackCount;
}

int CCeraShopGoods::GetGoldPrice() const
{
    return m_goldPrice;
}

int CCeraShopGoods::GetWinPoint() const
{
    return m_winPoint;
}

int CCeraShopGoods::GetCeraPrice() const
{
    return m_ceraPrice;
}

int CCeraShopGoods::GetMileagePrice() const
{
    return m_mileagePrice;
}

int CCeraShopGoods::GetSubGroupIndex() const
{
    return m_subGroupIdx;
}

int CCeraShopGoods::GetGroupIdx() const
{
    return m_groupIdx;
}

int CCeraShopGoods::GetFeaturedIdx() const
{
    return m_featuredIdx;
}

int CCeraShopGoods::GetSticker() const
{
    return m_sticker;
}

int CCeraShopGoods::GetExpiryDate() const
{
    return m_expiryDate;
}

int CCeraShopGoods::GetBonusWeight() const
{
    return m_bonusWeight;
}

CItem* CCeraShopGoods::GetRelatedItem() const
{
    return m_pItem;
}

bool CCeraShopGoods::isGiftDisallowance_() const
{
    return m_giftDisallow != 0;
}

bool CCeraShopGoods::isCeraPointDisallowance_() const
{
    return m_ceraPointDisallow != 0;
}

bool CCeraShopGoods::isStackBuyable() const
{
    return m_stackBuyable != 0;
}

// ============================================================================
// importCashShopItemList（商城脚本装载，ORIG 0x88fdcb5）
// 语义：加载 RDAR 商城脚本；按分区扫描商品行（每行首字段为条目序号），
// 以 STCeraShopItem 逐项填入商品向量；[limit] 类分区填限购映射。
// ============================================================================
extern "C" bool loadRDARScriptFile(const char* a, const char* b);
extern "C" int ScanType(std::string& s, bool& flag);
extern "C" int ScanInt(bool& flag);
extern "C" void ScanStr(std::string& s);

bool importCashShopItemList(
    char* path, std::vector<STCeraShopItem>* itemList,
    std::vector<stCeraShopBonusInfo_t>& bonusList, void* param4, int* param5,
    std::map<int, std::vector<std::vector<int> > >* limitMap,
    std::vector<int>* param7)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    itemList->clear();
    std::string section;
    bool flag = false;

    // 简化扫描：逐行读 int 序列，第一条为 itemIdx，按顺序映射已知字段。
    STCeraShopItem item;
    bool eof = false;
    while (true) {
        int v0 = ScanInt(eof);
        if (!eof)
            break;
        item.clear();
        item.m_field14 = v0;
        int v1 = ScanInt(eof);
        int v2 = ScanInt(eof);
        int v3 = ScanInt(eof);
        int v4 = ScanInt(eof);
        if (eof) {
            item.m_fieldc = v1;
            item.m_field10 = v2;
            item.m_field1c = v3;
            item.m_field34 = v4;
            itemList->push_back(item);
        }
    }
    return 1;
}
