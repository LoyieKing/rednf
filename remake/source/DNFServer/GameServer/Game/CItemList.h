#ifndef GAME_CITEMLIST_H_
#define GAME_CITEMLIST_H_

// ============================================================================
// df_game_r 还原 —— CItemList（0x74 布局）
// 布局依据 ORIG 构造（0x8510d12）/ 析构（0x8510df6）逐成员推导。
// CItem/CCeraShopGoods/Inven_Item 等实现在各自 TU，此处仅声明。
// ============================================================================

#include <ext/hash_map>
#include <map>
#include <string>
#include <vector>

#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "STEquipmentScript.h"
#include "STStackableScript.h"

// ============================================================================
// STCeraShopItem（商城 DB 条目，G4 商城/交易域批次，ORIG 尺寸 0x88）
// 布局依据 STCeraShopItem::C1 (0x88fd974) / clear (0x88fd9c4) /
// CCeraShopGoods::SetItem (0x8510c1a) 逐字段推导（COW string 尺寸 4）。
// ============================================================================
struct STCeraShopItem
{
    STCeraShopItem();
    STCeraShopItem(const STCeraShopItem& other);
    ~STCeraShopItem();
    STCeraShopItem& operator=(const STCeraShopItem& other);
    bool operator==(const STCeraShopItem& other);

    void clear();
    int getCeraPrice();
    int getGoldPrice();
    bool isLimitSaleItem();

    char m_field0;                    // +0x00
    char m_pad01[3];                  // +0x01
    int m_field4;                     // +0x04
    int m_field8;                     // +0x08（operator== 比较位）
    int m_fieldc;                     // +0x0c（groupIdx）
    int m_field10;                    // +0x10（subGroupIdx）
    int m_field14;                    // +0x14（itemIdx）
    char m_field18;                   // +0x18
    char m_pad19[3];                  // +0x19
    int m_field1c;                    // +0x1c（stackCount）
    char m_field20;                   // +0x20（sticker）
    char m_pad21;                     // +0x21
    unsigned short m_field22;         // +0x22（expiryDate）
    int m_field24;                    // +0x24
    int m_field28;                    // +0x28
    int m_field2c;                    // +0x2c（goldPrice）
    int m_field30;                    // +0x30（winPoint）
    int m_field34;                    // +0x34（mileagePrice）
    int m_field38;                    // +0x38（ceraPrice）
    char m_field3c;                   // +0x3c
    char m_pad3d[3];                  // +0x3d
    std::string m_name;               // +0x40
    int m_field44;                    // +0x44（bonusWeight）
    int m_field48;                    // +0x48
    int m_field4c;                    // +0x4c
    char m_field50;                   // +0x50（giftDisallow）
    char m_field51;                   // +0x51（ceraPointDisallow）
    char m_field52;                   // +0x52
    char m_field53;                   // +0x53（stackBuyable）
    int m_field54;                    // +0x54（limit sale flag，-2 = 非限售）
    int m_field58;                    // +0x58
    int m_field5c;                    // +0x5c
    char m_field60;                   // +0x60
    char m_pad61[3];                  // +0x61
    unsigned short m_field62;         // +0x62
    unsigned short m_field64;         // +0x64
    int m_field68;                    // +0x68
    int m_field6c;                    // +0x6c
    int m_field70;                    // +0x70
    int m_field74;                    // +0x74
    char m_field78;                   // +0x78
    char m_pad79[3];                  // +0x79
    int m_field7c;                    // +0x7c
    int m_field80;                    // +0x80
    char m_field84;                   // +0x84
};

// ============================================================================
// CCeraShopGoods（商城商品，G4 商城/交易域批次，ORIG 尺寸 0x3c）
// 布局依据 SetItem (0x8510c1a) + 各 weak 访问器位移推导。
// ============================================================================
class CCeraShopGoods
{
public:
    CCeraShopGoods(const STCeraShopItem& item);

    void SetItem(const STCeraShopItem& item);

    int GetItemIdx() const;
    int GetStackCount() const;
    int GetGoldPrice() const;
    int GetWinPoint() const;
    int GetCeraPrice() const;
    int GetMileagePrice() const;
    int GetSubGroupIndex() const;
    int GetGroupIdx() const;
    int GetFeaturedIdx() const;
    int GetSticker() const;
    int GetExpiryDate() const;
    int GetBonusWeight() const;
    CItem* GetRelatedItem() const;
    bool isGiftDisallowance_() const;
    bool isCeraPointDisallowance_() const;
    bool isStackBuyable() const;

    int m_itemIdx;                    // +0x00
    int m_stackCount;                 // +0x04
    int m_goldPrice;                  // +0x08
    int m_winPoint;                   // +0x0c
    int m_ceraPrice;                  // +0x10
    int m_mileagePrice;               // +0x14
    int m_subGroupIdx;                // +0x18
    int m_groupIdx;                   // +0x1c
    int m_featuredIdx;                // +0x20
    int m_sticker;                    // +0x24
    int m_expiryDate;                 // +0x28
    int m_bonusWeight;                // +0x2c
    int m_field30;                    // +0x30
    char m_giftDisallow;              // +0x34
    char m_ceraPointDisallow;         // +0x35
    char m_stackBuyable;              // +0x36
    char m_pad37;                     // +0x37
    CItem* m_pItem;                   // +0x38
};

struct stCeraShopBonusItemInfo_t
{
    int m_field0;                    // +0x00
    unsigned short m_field4;         // +0x04
    char m_pad6[2];                  // +0x06
    int m_field8;                    // +0x08
};

struct stCeraShopBonusInfo_t
{
    int m_field0;                                     // +0x00
    int m_field4;                                     // +0x04
    std::vector<stCeraShopBonusItemInfo_t> m_items;   // +0x08
};

struct BonusItem
{
    unsigned int m_min;   // +0x00
    unsigned int m_max;   // +0x04
    int m_itemIdx;        // +0x08
    char m_pad_c[8];      // +0x0c（元素尺寸 0x14，ORIG operator[] 步长）
};

enum ENUM_ITEM_CREATE_TYPE
{
    ENUM_ITEM_CREATE_TYPE_0 = 0,
    ENUM_ITEM_CREATE_TYPE_1 = 1
};

class CItemList
{
public:
    CItemList();
    ~CItemList();

    bool init();
    void destroy();
    CItem* find_item(int idx);
    CItem* find_item(const char* name);
    CCeraShopGoods* FindGoods(int goodsIdx);
    BonusItem* GetProperBonusItem(unsigned int bonusIdx);
    void getCeraShopBonusItem(int idx, unsigned long& outItem,
                              unsigned int& outCount) const;
    void get_lotto_item(int idx, char* out);
    void create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item, int param);
    bool insert_item(int group, int idx, CItem* item);
    bool load_item_files();
    bool Load_Goods_File(const char* path);

    __gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>,
                        std::equal_to<int>,
                        std::allocator<CItem*> >* m_gradeItemList;  // +0x00
    __gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>,
                        std::equal_to<int>,
                        std::allocator<CItem*> > m_itemMap;         // +0x04
    __gnu_cxx::hash_map<int, CCeraShopGoods*, __gnu_cxx::hash<int>,
                        std::equal_to<int>,
                        std::allocator<CCeraShopGoods*> > m_goodsMap; // +0x18
    __gnu_cxx::hash_map<unsigned int, int, __gnu_cxx::hash<unsigned int>,
                        std::equal_to<unsigned int>,
                        std::allocator<int> > m_lottoMap;           // +0x2c
    std::vector<stCeraShopBonusInfo_t> m_bonusList;                 // +0x40
    int m_field4c;                                                  // +0x4c
    std::vector<BonusItem> m_bonusItem;                             // +0x50
    std::map<unsigned long, int> m_lottoCount;                      // +0x5c
};

// ---- 外部依赖（其它 TU 提供） ----
class CDataManager;
extern CDataManager* G_CDataManager();

class BaseItemKey
{
public:
    BaseItemKey();
    BaseItemKey(unsigned char rarity, unsigned char usableLevel,
                unsigned short groupName);
};

class BaseItemValue
{
public:
    BaseItemValue();
    BaseItemValue(int value);
    int m_field0;   // +0x00
};

bool importCashShopItemList(char* path,
                            std::vector<STCeraShopItem>* shopList,
                            std::vector<stCeraShopBonusInfo_t>& bonusList,
                            void* a, int* b,
                            std::map<int, std::vector<std::vector<int> > >* c,
                            std::vector<int>* d);

class Arad_GiftItem_Set
{
public:
    int open(STStackableScript* script);

    // ORIG Arad_GiftItem_Set::Data（open 插入/ find 返回的对象）：
    //   +0x00 item_id；+0x04/+0x08 两个 std::string（脚本 +0x184/+0x188）。
    struct Data
    {
        Data() : m_item(0) {}
        int m_item;          // +0x00
        std::string m_str4;  // +0x04
        std::string m_str8;  // +0x08
    };

    // ORIG this+0x04：map<long long, Data>。
    // key = ((long long)kind << 32) | ((uint)field5c + ((uint)slot << 16))。
    std::map<long long, Data> m_map;
};

namespace ExtreamDungeon
{
class CCompound_ExtreamDun_Item
{
public:
    void check_N_addItem(CEquipItem& item);
    void after_addItem();
    void ForceResultItemRule_Setting(int idx, bool flag, int value);
};
}

int get_rand_int(int range);

#endif  // GAME_CITEMLIST_H_
