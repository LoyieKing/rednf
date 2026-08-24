// ============================================================================
// CItemGenerator.h — WongWork 掉落生成器域权威声明（唯一声明点）
//
// 覆盖类型（全部经 df_game_r nm/objdump 核实；报告见
//   docs/class_func_reports/WongWork__CItemGenerator{,Mgr}/ 等）：
//   ::CLuckPoint           （UseLuckPoint T 0x08550a84，静态方法）
//   WongWork::CLuckPoint   （ctor 0x08550884 / dtor 0x085508ec）
//   stGenerateRefData_t    （ctor 0x0814a706 / dtor 0x0814aaf0）
//   stGenerateResult_t     （ctor 0x0814ab06 / dtor 0x0814ab4c）
//   CGenerateRandomNumber  （ctor 0x085334ea / dtor 0x085335d0 /
//                           generateNumber 0x085334a4）
//   GenerateRandomUpgrade  （ctor 0x085335ee / dtor 0x0853364e）
//   CItemGenerator         （ctor 0x08533b3e / dtor 0x08533b9a；
//                           vtable _ZTVN8WongWork14CItemGeneratorE @ 0x08c9a788）
//
// 布局证据（objdump 反汇编 + 本工具链实测）：
//   WongWork::CLuckPoint:  +0x00 vector<CUser*>（0xc）；
//                          +0x0c map<int,int>（本工具链 sizeof(std::map)==0x18 实测）
//                          => sizeof 0x24
//   stGenerateResult_t:    +0x00 vector<Inven_Item>（0xc，ctor 先调
//                          St6vectorI10Inven_ItemC1Ev）；+0x0c CLuckPoint（ctor
//                          add $0xc 后调 CLuckPointC1Ev）
//                          => sizeof 0x30（ORIG 同帧两个结果对象相距 0x30 实证；
//                          CLuckPoint 0x24 之后无尾随成员）
//   stGenerateRefData_t:   +0x00 int=100；+0x04 int=0；+0x08 vector<bool>
//                          （本工具链 sizeof==0x14 实测；ctor clear +
//                          push_back(false)×2）；+0x1c float=1.0f；
//                          +0x20 int=0（setPremiumDropRate 写，CBattle_Field
//                          0x0830b848 实证写 +0x20 / 读字节 +0x40）；
//                          +0x24/+0x25 byte=0；+0x28 int=11；+0x2c/+0x30 int=0；
//                          +0x34..+0x47 调用上下文 blob（WarField/
//                          generateRewardItem 写 level/roleType/mobIndex/dungeon
//                          等，见各调用方注释）
//                          => sizeof 0x48（ORIG 同帧两个 ref 对象相距 0x48 实证）
//   CGenerateRandomNumber: +0x00 CMTRand*（ctor new CMTRand[count]，元素即
//                          PvPTypes.h 的 CMTRand(0x9c8)，generateNumber 对元素
//                          调 CMTRand::randInt）；+0x04 int count => sizeof 8
//   GenerateRandomUpgrade: +0x00 CMTRand*（operator new 0x9c8）=> sizeof 4
//   CItemGenerator:        +0x00 vptr；+0x04 CGeneratorCommonData*（setCommonData
//                          0x0853a3e2 写 +0x04）；+0x08 CGenerateRandomNumber；
//                          +0x10 GenerateRandomUpgrade；+0x14 int generatorType
//                          （setGeneratorType/getGeneratorType 写读 +0x14）
//                          => sizeof 0x18
//
// vtable（4 槽，槽位由派生类 vtable 交叉证实：CMonsterDrop @0x08c9a740 四槽 =
//   setDropRatio/loadScript/generateRandomItem/generateSpecificItem 具名符号）：
//   +0x00 setDropRatio(float)
//   +0x04 loadScript(char const*)
//   +0x08 generateRandomItem(stGenerateRefData_t const&, stGenerateResult_t&)
//   +0x0c generateSpecificItem(stGenerateRefData_t const&, stGenerateResult_t&)
// 注意：无虚析构（vtable 仅 4 槽；dtor 为非虚 T 符号）。
// ============================================================================
#ifndef GAME_CITEMGENERATOR_H_
#define GAME_CITEMGENERATOR_H_

#include <map>
#include <vector>

#include "Inven_Item.h"
#include "PvPTypes.h"   // CMTRand（0x9c8 字节随机源）

class CUser;  // 全局命名空间（ORIG vector<CUser*>）
class RegenerationROI;  // RandomRentalA2.cpp 的 RegenVer_* 签名引用

// ---- ::CLuckPoint（全局命名空间，与 WongWork::CLuckPoint 不同类型；
//      CParty.h 既有前向声明。ORIG UseLuckPoint T 0x08550a84） ----
class CLuckPoint
{
public:
    static bool UseLuckPoint(CUser* user, int itemType, int rarity, int grade);
};

namespace WongWork
{

// ---- CGeneratorCommonData（sizeof == 0x648，ORIG 实证）----
// 证据：getGoldGenTable ORIG T 0x08533424 以 this+level*8 步长取 8 字节表项
// （level>0xc8 时回退表项 0），并把 [基準値,随机浮动幅度] 拷入静态返回区
// 0x943ded0（CParty::get_item 0x4af 金币生成消费）=> 表为 201×8B。
// CItemGeneratorMgr ORIG operator new 0xa28、m_common 位于 +0x3e0 =>
// sizeof(common) = 0xa28-0x3e0 = 0x648 = 201*8 ✓。ctor/dtor 均空体
// （0x08374ebc/0x08374ec2 仅 push/mov/pop/ret）。loadScript T 0x08534456。
class CGeneratorCommonData
{
public:
    CGeneratorCommonData();                          // ORIG W 0x08374ebc（空体）
    ~CGeneratorCommonData();                         // ORIG W 0x08374ec2（空体）

    // ORIG T 0x08534456 _ZN8WongWork20CGeneratorCommonData10loadScriptEPKc
    // 返回错误行号（0=成功）；定义待脚本导入域批次。
    int loadScript(const char* path);

    int* getGoldGenTable(unsigned char level);       // ORIG T 0x08533424

    struct GoldGenEntry
    {
        int m_base;      // +0x00 基准值
        int m_randAmp;   // +0x04 随机浮动幅度【推断：语义命名，布局实证】
    };
    GoldGenEntry m_goldGenTable[201];  // +0x000（getGoldGenTable 按 level*8 索引，上界 0xc8）
};  // sizeof == 0x648

// ---- CLuckPoint（幸运点；sizeof 0x24） ----
class CLuckPoint
{
public:
    CLuckPoint();
    ~CLuckPoint();

    // 其余方法（AddUser 0x08550932 / GetLuckPoint 0x085509da 等）按需补充。
    std::vector<CUser*> m_users;   // +0x00（vector 0xc）
    std::map<int, int> m_points;   // +0x0c（本工具链 std::map 0x18 实测）
};

// ---- stGenerateRefData_t（掉落生成引用；sizeof 0x48） ----
struct stGenerateRefData_t
{
    stGenerateRefData_t();
    ~stGenerateRefData_t();

    int m_dropRate;                 // +0x00（ctor=100）
    int m_field04;                  // +0x04（ctor=0）
    std::vector<bool> m_forceTag;   // +0x08（ctor: clear + 2×push_back(false)；0x14 字节）
    float m_field1c;                // +0x1c（ctor=1.0f）
    int m_premiumDropRate;          // +0x20（setPremiumDropRate 写 0/-1/rate）
    char m_field24;                 // +0x24（ctor=0）
    char m_field25;                 // +0x25
    int m_field28;                  // +0x28（ctor=11）
    int m_field2c;                  // +0x2c
    int m_field30;                  // +0x30
    // +0x34..+0x47：调用方上下文区。独立掉落路径（generateIndependentItem
    // 0x834989a / independentItemRateControl 0x834972f）按字节读取以下字段；
    // 其余仍由调用方按原始偏移写入（保持字节布局不变）。
    char m_field34;                 // +0x34（基准值；与 +0x3f 组成等级差【推断】）
    char m_field35;                 // +0x35
    char m_rateSlot;                // +0x36（m_rate[] 掉率档位下标【推断】；WarField 写 mob level）
    char m_pad37;                   // +0x37
    char m_pad38[4];                // +0x38..+0x3b
    char m_field3c;                 // +0x3c（==3 时 premium 加成分支生效）
    char m_field3d;                 // +0x3d
    char m_countSlot;               // +0x3e（m_rate[] 数量档位下标【推断】）
    char m_levelHigh;               // +0x3f（等级上界；与 m_field34 差为档差【推断】）
    char m_pad40[8];                // +0x40..+0x47
};

// ---- stGenerateResult_t（掉落生成结果；sizeof 0x30） ----
// 注意：ORIG 无 +0x0f「幸运」成员。+0x00 vector(0xc) + +0x0c CLuckPoint(0x24)
// 已完整覆盖 0x30；result+0x0f 落在 CLuckPoint::m_users._M_start 指针内部，
// 不可字段化。CBossStage::consistMap ORIG 写的"幸运字节"是对象外的独立栈
// 标量死存储（-0xb2f(%ebp)，写后无读取），已删除，勿再加成员/裸偏移写入。
struct stGenerateResult_t
{
    stGenerateResult_t();
    ~stGenerateResult_t();

    std::vector<Inven_Item> m_dropItems;  // +0x00
    CLuckPoint m_luckPoint;               // +0x0c（sizeof 0x24）
};

// ---- CGenerateRandomNumber（加权随机表宿主；sizeof 8） ----
class CGenerateRandomNumber
{
public:
    CGenerateRandomNumber(int count);
    ~CGenerateRandomNumber();

    int generateNumber(int idx, int range);

    CMTRand* m_table;  // +0x00（new CMTRand[count]）
    int m_count;       // +0x04
};

// ---- GenerateRandomUpgrade（升级随机源；sizeof 4） ----
class GenerateRandomUpgrade
{
public:
    GenerateRandomUpgrade();
    ~GenerateRandomUpgrade();

    CMTRand* m_rand;  // +0x00（operator new 0x9c8；构造期额外初始化调用未建模，[INFERENCE]）
};

// ---- CItemGenerator（生成器基类；sizeof 0x18，无虚析构） ----
class CItemGenerator
{
public:
    CItemGenerator(int count);
    ~CItemGenerator();  // 非虚（ORIG vtable 仅 4 槽）

    virtual void setDropRatio(float ratio);                                        // 槽 +0x00
    virtual int loadScript(char const* path);                                      // 槽 +0x04（ORIG 基类 0x0853a3f6 ret0：返回错误行，0=成功）
    virtual void generateRandomItem(stGenerateRefData_t const& ref,
                                    stGenerateResult_t& result);                   // 槽 +0x08
    virtual void generateSpecificItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result);                 // 槽 +0x0c

    void setCommonData(CGeneratorCommonData* common);        // W inline，写 +0x04
    CGeneratorCommonData* getGeneratorCommonData();          // W inline，读 +0x04
    void setGeneratorType(int type);                         // W inline，写 +0x14
    int getGeneratorType();                                  // W inline，读 +0x14

    CGeneratorCommonData* m_common;      // +0x04（ORIG ctor 不初始化）
    CGenerateRandomNumber m_random;      // +0x08
    GenerateRandomUpgrade m_upgrade;     // +0x10
    int m_generatorType;                 // +0x14
};


// ============================================================================
// 派生生成器与生成率表（布局由 ORIG ctor/loadScript 偏移逐字段实证；实现
// 全部在 DropLayoutDeps.cpp，本头为唯一声明点）
// ============================================================================

// ---- 脚本范围表：u16×202。[item drop ref table] 以低/高字节写各半边，
//      generateTable 读低字节(min)/高字节(max)。 --------------------------------
struct stGenItemRange_t   // ORIG WongWork::stGenItemRange_t
{
    unsigned short m_range[202];
};

// ---- 单 type 组生成率块：槽址 = type*0x7110+rarity*0x12d8+level*24 ----------
struct DropGenRateBlock_t
{
    char m_typeSlot0[0x7110];                        // type 槽 0：ORIG 不构造不访问
    std::map<unsigned int, int> m_slots[4][6][201];  // 元素步长 24
};  // sizeof 0x23550

}  // namespace WongWork（临时闭合：random_option 为全局命名空间）

namespace random_option
{
// CRandomOptionItemHandle 布局镜像（权威定义 GlobalData.cpp；DisPatcherCore.cpp
// 另有仅签名镜像，互不包含）。sizeof 0x18。
class CRandomOptionItemHandle
{
public:
    CRandomOptionItemHandle();           // 定义 GlobalData.cpp（W 0x085f0d64）
    virtual ~CRandomOptionItemHandle();  // 定义 GlobalData.cpp（W 0x085f0e46）
    void set_disable_drop();             // 定义 DropLayoutDeps.cpp（W 0x08539e42）
    unsigned int reset_option(unsigned long itemIdx, ENUM_RARITY rarity,
                              int optionType, RandomOption* options); // ORIG T 0x085f39d2
    bool give_option(unsigned long itemIdx, ENUM_RARITY rarity, int level,
                     int group, int grade, RandomOption* opts);          // ORIG T 0x085f2cc6
    bool RegenVer_give_option(unsigned long itemIdx, ENUM_RARITY rarity,
                              int level, int group, int grade, RandomOption*,
                              RegenerationROI*, int optType);            // ORIG T 0x085f1bb0
    unsigned int RegenVer_choose_item(ENUM_RARITY rarity, int level,
                                      const std::vector<int>& itemGroups); // ORIG T 0x085f1802
    int RegenVer_get_option_quantity(ENUM_RARITY rarity, int grade,
                                     unsigned char optType, int level,
                                     int& outQuantity, RegenerationROI*); // ORIG T 0x085f1a62

private:
    static bool give_option_core(CRandomOptionItemHandle*, unsigned long,
                                 ENUM_RARITY, int, int, int, RandomOption*,
                                 RegenerationROI*, int optType, bool regenVer);
    int _get_level_limit(ENUM_RARITY rarity, int level);                 // T 0x085f0f9a
    int _get_part_type(int limit);                                       // T 0x085f1088
    int _get_part_type_by_ItemGroup(int limit,
                                    const std::vector<int>& groups) const; // T 0x085f11a4
    int _get_base_item(ENUM_RARITY rarity, int limit);                   // T 0x085f1426
    int _get_option_type(unsigned long itemIdx, ENUM_RARITY rarity,
                         int level, int group, unsigned char& outType);  // T 0x085f2338
    int _get_option_quantity(ENUM_RARITY rarity, int grade,
                             unsigned char optType, int level,
                             int& outQuantity);                      // T 0x085f25b0
    int _choose_option_group(ENUM_RARITY rarity, int level, int grade,
                             unsigned char optType, int group, int& outGroup); // T 0x085f2882
    int _get_option_value(int minV, int maxV);                           // T 0x085f2b02

private:
    CMTRand* m_rand;           // +0x04
    int m_field08;             // +0x08
    unsigned char m_disabled;  // +0x0c
    int m_field10;             // +0x10
    void* m_optionSet;         // +0x14
};
}  // namespace random_option

namespace WongWork
{
// ---- CItemGenRateTable（掉落等级累积表宿主；sizeof 0x2356C）-----------------
class CItemGenRateTable
{
public:
    CItemGenRateTable();                         // W 0x08539e66
    ~CItemGenRateTable();                        // W 0x0853a030
    void init();                                 // T 0x085345b6
    void destroy();                              // T 0x08534638
    void generateTable(stGenItemRange_t* range, bool keepRandomOption);  // T 0x0853466c
    bool _itemTypeVerify(int type, const CItem* item) const;                  // T 0x08534906
    void set_disable_drop_random_option_item();  // W 0x08539e4e

private:
    DropGenRateBlock_t m_block;                  // +0x00000
    CMTRand* m_rand;                             // +0x23550
    random_option::CRandomOptionItemHandle m_optHandle;  // +0x23554
};  // sizeof 0x2356C

// ---- CSecialIMonsterDropItemGenRateTable（特殊怪物变体；sizeof 0x211FCC）----
class CSecialIMonsterDropItemGenRateTable
{
public:
    CSecialIMonsterDropItemGenRateTable();       // W 0x0853a0b6
    ~CSecialIMonsterDropItemGenRateTable();      // W 0x0853a35c
    void init();                                 // T 0x08534c1a
    void destroy();
    void generateTable(stGenItemRange_t* range);          // T 0x08534cd0
    bool _itemTypeVerify(int type, const CItem* item) const;

private:
    DropGenRateBlock_t m_blocks[5][3];           // 15×0x23550 = 0x211FB0
    CMTRand* m_rand;                             // +0x211FB0
    random_option::CRandomOptionItemHandle m_optHandle;  // +0x211FB4
};  // sizeof 0x211FCC

// ---- 首领/命名怪首杀权重（W 0x0853a6ae：[+4]=1 后拷回 [+0]）----------------
struct stFirstMobHuntingWeight_t
{
    stFirstMobHuntingWeight_t();
    int m_weight;      // +0x00
    int m_field04;     // +0x04
};

// ---- CMonsterDrop_Hell（地狱派对掉落；sizeof 0x237C0）-----------------------
class CMonsterDrop_Hell : public CItemGenerator
{
public:
    CMonsterDrop_Hell(int count);                // W 0x0853a56a
    ~CMonsterDrop_Hell();                        // W 0x0853a60e
    int loadScript(char const* path);            // T 0x08535344（虚槽 +0x04）

protected:
    struct stGenItemProb_t { int v[7]; };

    std::vector<stGenItemProb_t> m_probs;        // +0x18
    std::vector<stGenItemProb_t> m_probsSub;     // +0x24（ctor 构造；装载未用）
    int m_rarityBasis[5][6];                     // +0x30（行 0 未用；脚本行数≤4）
    char m_padA8[0x18];                          // +0xa8..0xbf（未建模区）
    stGenItemRange_t m_range;               // +0xc0..0x253
    CItemGenRateTable m_table;                   // +0x254..0x237BF
};  // sizeof 0x237C0

// ---- CSpecialMonsterDrop（特殊怪物掉落；sizeof 0x21230C）--------------------
class CSpecialMonsterDrop : public CItemGenerator
{
public:
    CSpecialMonsterDrop(int count);              // W 0x0853a426
    ~CSpecialMonsterDrop();                      // W 0x0853a4ca
    int loadScript(char const* path);            // T 0x085373ee（虚槽 +0x04）

protected:
    struct stGenItemProb_t { int v[7]; };

    std::vector<stGenItemProb_t> m_probs;        // +0x18
    std::vector<stGenItemProb_t> m_probsSub;     // +0x24
    int m_rarityBasis[5][6];                     // +0x30（行 0 未用）
    float m_partyBonus[5][4];                    // +0xa8
    float m_dungeonBonus[5][5];                  // +0xf8
    float m_monsterTypeBonus[5][4];              // +0x15c
    stGenItemRange_t m_range;               // +0x1ac..0x33f
    CSecialIMonsterDropItemGenRateTable m_table; // +0x340..0x21230B
};  // sizeof 0x21230C

// 各类 sizeof 与 ORIG operator new 分配尺寸一致（initialize.md 实证：
// CMonsterDrop/CObjectDrop 0x2593a4、CClearReward 0x23b84、CDungeonDrop 0x18）；
// 内部成员起点全部来自对应 loadScript/ctor 的立即数偏移。
class CMonsterDrop : public CItemGenerator
{
public:
    CMonsterDrop(int count);                                   // W 0x0853a6c8
    int loadScript(char const* path);                          // T 0x08537938（虚槽 +0x04）
    // 非虚三参重载（ORIG T 0x085380fa 直呼；返回错误行，0=成功）。
    int loadScript(char const* path, char const* hell, char const* special);

protected:
    struct stGenItemProb_t { int v[7]; };
    struct stConditionPair_t { unsigned short rate; unsigned short value; };

    std::vector<stGenItemProb_t> m_probs;            // +0x18
    std::vector<stGenItemProb_t> m_probsSub;         // +0x24
    int m_rarityBasis[5][6];                         // +0x30（行 0 未用）
    float m_partyBonus[5][4];                        // +0xa8
    float m_dungeonBonus[5][5];                      // +0xf8
    float m_monsterTypeBonus[5][4];                  // +0x15c
    stGenItemRange_t m_range;                   // +0x1ac..0x33f
    CItemGenRateTable m_table;                       // +0x340..0x238AB
    CMonsterDrop_Hell m_hell;                        // +0x238ac..0x4706B
    CSpecialMonsterDrop m_special;                   // +0x4706c..0x259377
    stFirstMobHuntingWeight_t m_firstMobHunt;        // +0x259378
    int m_firstMobWeight;                            // +0x259380（ctor=1）
    float m_firstMobWeightRate;                      // +0x259384（ctor=1.0f）
    stConditionPair_t m_conditionRate[4];            // +0x259388..0x259397
    unsigned short m_goldQuantity[4];                // +0x259398..0x25939F
    unsigned short m_goldVolume[2];                  // +0x2593a0..0x2593a3
};  // sizeof 0x2593a4

// CObjectDrop 以 CMonsterDrop 为基（ORIG ctor 0x0853a7f8 调 CMonsterDropC1 再置
// vptr 0x08c9a730）；自身覆写虚槽 +0x04（ORIG T 0x08538352）并预置队伍加成 1.0f。
class CObjectDrop : public CMonsterDrop
{
public:
    CObjectDrop(int count);                                    // W 0x0853a7f8
    int loadScript(char const* path);                          // T 0x08538352
};

class CClearReward : public CItemGenerator
{
public:
    CClearReward(int count);                                   // W 0x0853a968
    int loadScript(char const* path);                          // T 0x085392c0（虚槽 +0x04）

protected:
    struct stGenItemProb_t { int v[3]; };                      // 每行三值
    struct stGoldCardBlankItemInfo_t { int v[3]; };            // 空白卡三元组
    std::vector<stGenItemProb_t> m_probs[7];         // +0x18..0x6b（ctor 构造 7 个）
    char m_pad6c[0x3c];                              // +0x6c..0xa7（未建模区）
    int m_dropKindProb[2];                           // +0xa8/+0xac
    char m_padb0[4];                                 // +0xb0..0xb3
    int m_dropItemTypeProb[4];                       // +0xb4（ORIG 索引 1..4 → 下标 i-1）
    int m_dungeonDropBonus[5];                       // +0xc4
    float m_dungeonGoldBonus[5];                     // +0xd8
    float m_partyBonus[4];                           // +0xec
    int m_rarityBasis[6];                            // +0xfc
    stGenItemRange_t m_range;                   // +0x114..0x2a7
    CItemGenRateTable m_table;                       // +0x2a8..0x23813
    int m_goldCardCost[201];                         // +0x23814..0x23b37
    float m_goldCardCreateRate;                      // +0x23b38
    int m_goldCardBlankTotal;                        // +0x23b3c
    std::vector<stGoldCardBlankItemInfo_t> m_goldBlanks;    // +0x23b40
    int m_rewardRatePerMap[10];                      // +0x23b4c（ctor memset 0x28）
    int m_pcRoomBlankTotal;                          // +0x23b74
    std::vector<stGoldCardBlankItemInfo_t> m_pcRoomBlanks;  // +0x23b78
};  // sizeof 0x23b84

class CDungeonDrop : public CItemGenerator
{
public:
    CDungeonDrop(int count);                                   // W 0x0853abc2
};  // sizeof == 0x18 == ORIG 分配尺寸（ORIG 无 loadScript 覆写符号，虚槽走基类）

class CWorldDrop : public CItemGenerator
{
public:
    CWorldDrop(int count);                                     // W 0x0853a826
    // 非虚双参重载（ORIG T 0x0853855c 直呼）。定义待掉落域批次。
    int loadScript(char const* path, char const* warArea);

    char m_tail[0x1f80 - 0x18];     // 未建模区（ORIG 分配尺寸 0x1f80；ctor 于
                                    // +0x18 构造 201×0x14 WorldDropInfo 实证）
};
 
 }  // namespace WongWork
 
// 布局自检（置于定义之外；本工具链实测 vector<bool>=0x14、std::map<int,int>=0x18）。
typedef char assert_sizeof_wg_luckpoint[sizeof(WongWork::CLuckPoint) == 0x24 ? 1 : -1];
typedef char assert_sizeof_refdata[sizeof(WongWork::stGenerateRefData_t) == 0x48 ? 1 : -1];
typedef char assert_sizeof_result[sizeof(WongWork::stGenerateResult_t) == 0x30 ? 1 : -1];
typedef char assert_sizeof_genrand[sizeof(WongWork::CGenerateRandomNumber) == 8 ? 1 : -1];
typedef char assert_sizeof_genupgrade[sizeof(WongWork::GenerateRandomUpgrade) == 4 ? 1 : -1];
typedef char assert_sizeof_itemgen[sizeof(WongWork::CItemGenerator) == 0x18 ? 1 : -1];
typedef char assert_sizeof_gencommon[sizeof(WongWork::CGeneratorCommonData) == 0x648 ? 1 : -1];

#endif  // GAME_CITEMGENERATOR_H_
