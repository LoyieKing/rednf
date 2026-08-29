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
#include "ServerParameterScript.h"  // ENUM_UNUSABLE_EQUIPMENT_TYPE（CCompound 私有转换函数参数）

#include "STStackableScript.h"
#include "Arad_ScriptLoader.h"   // Arad_GiftItem_Set 虚基类（vptr@+0x00）

// ============================================================================
// STCeraShopItem（商城 DB 条目，G4 商城/交易域批次，ORIG 尺寸 0x88）
// 布局依据 STCeraShopItem::C1 (0x88fd974) / clear (0x88fd9c4) /
// CCeraShopGoods::SetItem (0x8510c1a) 逐字段推导（COW string 尺寸 4）。
// 真名线索（Load_Goods_File / insertCeraShopItemIntoDB 的 SQL+日志实证）：
//   m_field8=ipg_no（"fail IPG" 日志、dnf_item_price(ipg_no,...) 首列）、
//   m_field14=it_no（itemIdx，dnf_item_price 第二列 / find_item 参数）、
//   m_field1c=it_cnt（dnf_item_price 第三列）、m_field38=cera_price 来源、
//   m_field22=sale type（insertCeraShopItemIntoDB 的 c 分支选择 0/1），
//   m_field10=m_lottoMap 键（GetRestrictCode 查询源）。
// ============================================================================
struct STCeraShopItem
{
    STCeraShopItem();
    bool operator==(const STCeraShopItem& other);

    void clear();
    int getCeraPrice();
    int getGoldPrice();
    bool isLimitSaleItem();

    char m_field0;                    // +0x00
    int m_field4;                     // +0x04
    int m_field8;                     // +0x08（operator== 比较位）
    int m_fieldc;                     // +0x0c（groupIdx）
    int m_field10;                    // +0x10（subGroupIdx）
    int m_field14;                    // +0x14（itemIdx）
    char m_field18;                   // +0x18
    int m_field1c;                    // +0x1c（stackCount）
    char m_field20;                   // +0x20（sticker）
    unsigned short m_field22;         // +0x22（expiryDate）
    int m_field24;                    // +0x24
    int m_field28;                    // +0x28
    int m_field2c;                    // +0x2c（goldPrice）
    int m_field30;                    // +0x30（winPoint）
    int m_field34;                    // +0x34（mileagePrice）
    int m_field38;                    // +0x38（ceraPrice）
    char m_field3c;                   // +0x3c
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
    unsigned short m_field62;         // +0x62
    unsigned short m_field64;         // +0x64
    int m_field68;                    // +0x68
    int m_field6c;                    // +0x6c
    int m_field70;                    // +0x70
    int m_field74;                    // +0x74
    char m_field78;                   // +0x78
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
    float GetBonusWeight() const;
    CItem* GetRelatedItem() const;
    unsigned char isGiftDisallowance_() const;
    unsigned char isCeraPointDisallowance_() const;
    unsigned char isStackBuyable() const;

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
    CItem* m_pItem;                   // +0x38
};

struct stCeraShopBonusItemInfo_t
{
    ~stCeraShopBonusItemInfo_t();    // ORIG W 0x089363da：空体
    int m_field0;                    // +0x00
    unsigned short m_field4;         // +0x04
    int m_field8;                    // +0x08
};

// ---- stCeraShopBonusInfo_t（ORIG C1 W 0x0891be5c：仅 vector ctor；
//      C1ERKS W 0x08924c00 / D1 W 0x08525f3c / op= W 0x08924cba，全 W → 头内联）----
struct stCeraShopBonusInfo_t
{
    stCeraShopBonusInfo_t() : m_items() {}
    stCeraShopBonusInfo_t(const stCeraShopBonusInfo_t& o)
        : m_field0(o.m_field0), m_field4(o.m_field4), m_items(o.m_items) {}
    ~stCeraShopBonusInfo_t() {}
    stCeraShopBonusInfo_t& operator=(const stCeraShopBonusInfo_t& o)
    {
        m_field0 = o.m_field0;
        m_field4 = o.m_field4;
        m_items = o.m_items;
        return *this;
    }

    int m_field0;                                     // +0x00
    int m_field4;                                     // +0x04
    std::vector<stCeraShopBonusItemInfo_t> m_items;   // +0x08
};   // sizeof 0x14（_M_insert_aux 步进实证）

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
    // ORIG _ZNK9CItemList19GetVendingBonusRateEv W 0x0854c596：读 +0x4c（弱内联，
    // GetFinalProbability 消费；W8LT_VendingHero 批次补声明）。
    int GetVendingBonusRate() const { return m_field4c; }
    // ORIG W 0x0869476e _ZNK9CItemList21GetQuestItemDropBounsERKm：
    // m_lottoCount(+0x5c, std::map<unsigned long,int>).find(key)；
    // 命中返 second（mov 0x4 偏移），未命中返 0。
    int GetQuestItemDropBouns(const unsigned long& key) const;
    int GetRestrictCode(int itemIdx);                 // ORIG 0x0817a10c (W)
    CCeraShopGoods* FindGoods(int goodsIdx);
    BonusItem* GetProperBonusItem(unsigned int bonusIdx);
    void getCeraShopBonusItem(int idx, unsigned long& outItem,
                              unsigned int& outCount) const;
    void get_lotto_item(int idx, char* out);
    void create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item, int param);
    bool insert_item(int group, int idx, CItem* item);
    bool load_item_files();
    bool Load_Goods_File(const char* path);
    // ORIG W 0x8539e38：返回 +0x00 的等级分桶 hash_map 数组指针（元素步长
    // sizeof(hash_map)=20；generateTable 以等级直接下标遍历）。
    __gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>,
                        std::equal_to<int>,
                        std::allocator<CItem*> >* getItemGradeList() const;

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
    // ORIG C1 0x08513f82（W）：写 +0/+1 字节与 +2 short；op< 无独立符号（内联），
    // 按字典序 [推断]（RandomRentalA2 批次按 choose_item 报告补全字段名）。
    BaseItemKey(unsigned char rarity, unsigned char usableLevel,
                unsigned short groupName);

    unsigned char m_rarity;     // +0x00
    unsigned char m_level;      // +0x01
    unsigned short m_partType;  // +0x02

    bool operator<(const BaseItemKey& o) const
    {
        return m_rarity < o.m_rarity ||
               (m_rarity == o.m_rarity && m_level < o.m_level) ||
               (m_rarity == o.m_rarity && m_level == o.m_level &&
                m_partType < o.m_partType);
    }
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

class Arad_GiftItem_Set : public Arad_ScriptLoader
{
public:
    struct Data;   // 前置声明：find 返回 Data*，完整定义在下方

    Arad_GiftItem_Set();   // ORIG 0x081a2a68：填 +0x1c 的 ST_EVENT_GIFT_ITEM*→kind 表
    ~Arad_GiftItem_Set();  // ORIG T 0x081a2dea：析构 +0x04/+0x1c 两个 map（无虚析构）
    int open(STStackableScript* script);
    // ORIG 0x081a311e：按 64 位 key=(kind<<32)|(job<<16)+usableLevel 查 m_map。
    // 命中返回 &it->second（Data*，send_mail 依此读取 item_id/两个 string），否则 0。
    Data* find(int kind, int job, int usableLevel);
    int str_to_enum(const char* name);                      // ORIG 0x081a2e3e
    int level_check(CUser* user, const CStackableItem* item); // ORIG 0x081a31c4
    // ORIG T _ZN17Arad_GiftItem_Set9send_mailEP5CUseriiii 0x081a329e（LinkClosureD 补）
    int send_mail(CUser* user, int p2, int p3, int p4, int giftIdx);

    // ORIG Arad_GiftItem_Set::Data（open 插入/ find 返回的对象）：
    //   +0x00 item_id；+0x04/+0x08 两个 std::string（脚本 +0x184/+0x188）。
    struct Data
    {
        // ORIG C1 0x081a40e8：两个 string 均以 "material_exception_item" 构造
        // （rodata 0x8bac660），非空串。
        Data() : m_item(0),
                 m_str4("material_exception_item"),
                 m_str8("material_exception_item") {}
        int m_item;          // +0x00
        std::string m_str4;  // +0x04
        std::string m_str8;  // +0x08
    };

    // ORIG this+0x04：map<long long, Data>（vptr@+0x00 后）。
    // key = ((long long)kind << 32) | ((uint)field5c + ((uint)slot << 16))。
    std::map<long long, Data> m_map;

    // ORIG this+0x1c：map<string,int>（"ST_EVENT_GIFT_ITEM"[2..6] → 1..6）
    std::map<std::string, int> m_kindMap;
};

namespace ExtreamDungeon
{

// ── ORIG 合成链枚举（按值传参，ABI=int；成员名不可考，按数值命名）──
enum ENUM_INPUTITEMS_RARITY
{
    INPUT_RARITY_0 = 0,
    INPUT_RARITY_1 = 1,
    INPUT_RARITY_2 = 2
};

enum ENUM_OUTPUTITEMS_RARITY
{
    OUTPUT_RARITY_0 = 0,
    OUTPUT_RARITY_1 = 1,
    OUTPUT_RARITY_2 = 2,
    OUTPUT_RARITY_3 = 3
};

enum eBindSphereType
{
    BIND_SPHERE_OFF = 0,
    BIND_SPHERE_ON = 1
};
// ── ORIG 强制规则条目（ctor 0x832e00a：flag→+0x00 字节、value→+0x04）──
struct forceRule_ResultItem
{
    forceRule_ResultItem() : m_flag(false), m_value(0) {}
    forceRule_ResultItem(bool flag, int value);
    char m_flag;   // +0x00（1=禁止合成，0=强制允许）
    int m_value;   // +0x04
};

// ── ORIG 排序表项（ctor 0x832dfbe / dtor 0x8330cec；_AddItem 构造）──
struct OUTPUT_ITEM_STR
{
    OUTPUT_ITEM_STR(int usableLevel, int index);
    bool operator<(const OUTPUT_ITEM_STR& other) const;   // 升序【推断】

    int m_usableLevel;   // +0x00
    int m_index;         // +0x04
};

// ORIG _ZN14ExtreamDungeon19BindTable_PredicateERKNS_15OUTPUT_ITEM_STRES2_
// T 0x832c3cc：ExtreamDungeon 命名空间级排序谓词（lower_bound/upper_bound 比较器，
// 非 CCompound 成员——mangled 无类前缀）。
bool BindTable_Predicate(const OUTPUT_ITEM_STR& a, const OUTPUT_ITEM_STR& b);

// ── ORIG BindCube_ScriptData（ctor 0x832e058 / dtor 0x832e220，尺寸 0xb88，
//    作为 CCompound 的 +0x04 子对象；偏移均为相对本对象）──
class BindCube_ScriptData
{
public:
    BindCube_ScriptData();
    ~BindCube_ScriptData();

    int m_rarityDicision[3][4];                          // +0x000（LoadScript [rarity dicision] 写入区）
    std::vector<OUTPUT_ITEM_STR> m_bindItems[0x3b][4];   // +0x030（行步 0x30；列=装备类别）
    std::vector<OUTPUT_ITEM_STR> m_resultItems[4];       // +0xb40（_AddItem 按类别入表）
    std::map<int, forceRule_ResultItem> m_forceRules;    // +0xb70（绝对 +0xb74）
};                                                        // 尺寸 0xb88

// ── ORIG BindCube_LvBonusScriptData（ctor 0x832e318 / dtor 0x832e3ac，尺寸 0x30）──
class BindCube_LvBonusScriptData
{
public:
    BindCube_LvBonusScriptData();
    ~BindCube_LvBonusScriptData();

    std::vector<unsigned int> m_levels;    // +0x00（[level/silver level dicision] 等级表）
    std::vector<unsigned int> m_rates[3];  // +0x0c（3 行比率表）
};                                          // 尺寸 0x30

class CCompound_ExtreamDun_Item
{
public:
    CCompound_ExtreamDun_Item();    // ORIG 0x832c402
    ~CCompound_ExtreamDun_Item();   // ORIG 0x832c524
    // ORIG _ZN14ExtreamDungeon25CCompound_ExtreamDun_Item10LoadScriptEPKcS2_
    // （返回 0=成功，非 0 为错误码；CDataManager::init 调用）
    int LoadScript(const char* dir, const char* path);
    bool check_N_addItem(CEquipItem& item);   // ORIG 返回 _AddItem 结果（0/1）
    void after_addItem();
    void ForceResultItemRule_Setting(int idx, bool flag, int value);

    bool _isPossibleResultItem(CEquipItem& item) const;         // ORIG 0x832db4e（const 方法、非 const 引用参数）
    bool _AddItem(CEquipItem& item);                            // ORIG 0x832dd1c
    int _convertOutputItem(ENUM_UNUSABLE_EQUIPMENT_TYPE eqType) const;   // ORIG 0x832c5ac
    int _convertInputItem(ENUM_UNUSABLE_EQUIPMENT_TYPE eqType) const;    // ORIG 0x832c620

    // ── ORIG 合成主链（nm -S 核实；Main 2026-08-26 授权补声明）──
    int checkCompoundable(Inven_Item* items, const CEquipItem** refs, char kind) const;   // T 0x832d7d6
    bool makeCompoundItem(const CEquipItem** refs, char select, Inven_Item& out,
                          eBindSphereType bind);                                          // T 0x832ccc0

private:
    int _get_base_level(const CEquipItem* const* refs) const;                             // W 0x832e40c
    int _convertInput_To_Output(ENUM_INPUTITEMS_RARITY inRarity) const;                   // T 0x832c5f0
    int _determine_rarity(unsigned int randVal,                                             // ORIG 返回 0..4（4=未命中）
                          ENUM_INPUTITEMS_RARITY inRarity) const;      // T 0x832c976
    std::pair<unsigned int, unsigned int> _determine_level_bonus(
        unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity, unsigned int& idxOut,
        eBindSphereType bind) const;                                                      // T 0x832c9ca（sret）
    std::pair<unsigned int, unsigned int> _determine_level_bonus(
        ENUM_INPUTITEMS_RARITY inRarity, unsigned int baseLevel,
        ENUM_OUTPUTITEMS_RARITY outRarity, eBindSphereType bind) const;                   // T 0x832cadc（sret）
    unsigned int _determine_level_bonus_index(
        unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
        eBindSphereType bind) const;                                                      // T 0x832cc02
    int _exception_Rare(const CEquipItem** refs, char select,
                        ENUM_OUTPUTITEMS_RARITY outRarity,
                        unsigned int baseLevel) const;                                    // T 0x832cffe
    int _FindSelectItem(unsigned int group, ENUM_OUTPUTITEMS_RARITY outRarity,
                        const std::pair<unsigned int, unsigned int>& range) const;        // T 0x832d15e
    int _FindRandomItem(ENUM_OUTPUTITEMS_RARITY outRarity,
                        const std::pair<unsigned int, unsigned int>& range) const;        // T 0x832d2ce
    int _FindItemAll_BindSphere(const CEquipItem** refs, char select,
                                ENUM_OUTPUTITEMS_RARITY outRarity,
                                std::pair<unsigned int, unsigned int> range) const;       // T 0x832d438
    int _FindItemRelateCreationRate(
        std::vector<OUTPUT_ITEM_STR>::const_iterator begin,
        std::vector<OUTPUT_ITEM_STR>::const_iterator end) const;                          // T 0x832d4b4

    CMTRand* m_rand;                          // +0x000（new CMTRand(time)，尺寸 0x9c8）
    BindCube_ScriptData m_data;               // +0x004（0xb88）
    BindCube_LvBonusScriptData m_lvBonus[2];  // +0xb8c（普通/白银两套等级决策）
};
}

int get_rand_int(int range);

#endif  // GAME_CITEMLIST_H_
