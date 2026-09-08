#ifndef GAME_CSTACKABLEITEM_H_
#define GAME_CSTACKABLEITEM_H_

// ============================================================================
// df_game_r 还原 —— CStackableItem（0x3f0 布局，单继承 CItem）
// 布局依据 ORIG 构造（0x8515762）/ 报告函数（CStackableItem.md）逐成员推导：
//   +0x148 AradItemInfo（0x2c）  +0x174 RecipeInfo（0x4c）
//   +0x1c0 LotteryInfo（0x24：idx/vec<pair>/cnt/vec<int>）
//   +0x1e0 CMTRand*  +0x1e4 MultiBoxLotteryInfo（0x38）
//   +0x21c/0x228/0x234 包数据 vector  +0x240 stStackableBooster_t（0x24）
//   +0x264 BoosterSelectInfo（0x1c）  +0x280 vector<stGlobalEffect_t>
//   +0x290 stLimitCubeInfo（0x30）  +0x2b0 m_nItemType
//   +0x2c0 vector<int> usableType  +0x2f4 map<STEnchantSystemMapKey,...>
//   +0x348 map<int,int>  +0x365 AvatarOptionChangeType
//   +0x368/0x380 set<int>  +0x3a0/0x3ac/0x3b8 vector  +0x3d8 set<int>
// ============================================================================

#include "AradItemInfo.h"   // +0x148 AradItemInfo 子对象（布局转写；类型见 AradItemInfo.h）
#include "STEquipmentScript.h"   // STEnchantSystemData 基类 + EquipmentSkillDataUp（附魔 map 键/值依赖）

// ---- StatChangeInfo（全局域；ORIG mangled _ZN14StatChangeInfoC1Ev）----
// CStackableItem::GetStatChangeInfo（ORIG 0x08694716 W）按值返回；
// ORIG ctor 0x085141a8：movl 0→+0 / movb 0→+4 / movb 0→+5。
struct StatChangeInfo
{
    StatChangeInfo()
    {
        sec = 0;
        m_flagLo = 0;
        m_flagHi = 0;
    }

    int sec;                  // +0x00
    char m_flagLo;            // +0x04（低字节）
    char m_flagHi;            // +0x05（次字节）
};

#include "CItem.h"

#include "AradRabbitBox.h"  // +0x168 兔盒列表元素类型（W6_ARAD_ItemBonus 收敛）

class CMTRand;
class CUser;

namespace StackableAction
{
// ORIG Type 枚举值取自 getStackableActionType（0x089aeb9b）字符串→类型映射。
enum Type
{
    TYPE_0 = 0,
    TYPE_CANDLE_GOLDEN_WAX = 0x7,
    TYPE_MEGAPHONE = 0x16,
    TYPE_ADD_AVATAR_SOCKET = 0x20,
    TYPE_EQUIPMENT_PURIFY = 0x21,
    TYPE_SERIA_ROOM_DECO = 0x2a,
    TYPE_UPGRADE_CARGO = 0x2b,
    TYPE_CHATEMOTICON = 0x2c,
    TYPE_OPEN_GUIDE_BOOK = 0x2d,
    TYPE_CLASS_CHANGE = 0x2e,
    TYPE_CHARACTER_NAME_CHANGE = 0x33,
    TYPE_BINGO = 0x34,
    TYPE_PORTABLE_UPGRADE = 0x35,
    TYPE_LOTTERY_COUNT = 0x36,
    TYPE_ADD_FATIGUE = 0x37
};
// ORIG getStackableActionUsablePlace（0x089aed67）字符串→UsablePlace 映射：
//   "[village]"→1 "[dungeon]"→2 "[pvp]"→3 "[seria room]"→4 "[dont use assult]"→5，默认 1
enum UsablePlace
{
    USABLE_PLACE_VILLAGE = 1,
    USABLE_PLACE_DUNGEON = 2,
    USABLE_PLACE_PVP = 3,
    USABLE_PLACE_SERIA_ROOM = 4,
    USABLE_PLACE_DONT_USE_ASSULT = 5
};
// ORIG getStackableActionUsableServerType（0x089aee0a）字符串→UsableServerType 映射：
//   "[general server]"→1 "[event server]"→2 "[starter server]"→3，默认 1
enum UsableServerType
{
    USABLE_SERVER_TYPE_GENERAL = 1,
    USABLE_SERVER_TYPE_EVENT = 2,
    USABLE_SERVER_TYPE_STARTER = 3
};
}

// 全局命名空间自由函数（ORIG T：0x089aeb9b / 0x089aed67 / 0x089aee0a，
// 字符串→枚举映射，调用方 ImportStackableScript 0x89aeec8）。
int getStackableActionType(std::string& type);
int getStackableActionUsablePlace(std::string& place);
int getStackableActionUsableServerType(std::string& type);
class STStackableScript;

// ---- STStackablePackageData（ORIG C1/C2 W 0x08919346：2×movl 0 逐字段清零）----
// 由 STStackableScript.h 迁入（CStackableItem 包数据成员 +0x21c/+0x228/+0x234
// 元素类型；ORIG C1 实证 vector<STStackablePackageData>）。
struct STStackablePackageData
{
    STStackablePackageData()
    {
        m_field0 = 0;
        m_field4 = 0;
    }

    int m_field0;   // +0x00
    int m_field4;   // +0x04
};

// ---- CountinuousItemOption（ORIG T 0x0898c770 C1 / 0x0898c784 clear；sizeof 3）----
// 3 个独立字节；clear 逐字节清零。trivial 析构。本类方法属 STStackableScript TU。
// 由 STStackableScript.h 迁入（CStackableItem +0x2d8/+0x2e4 子对象类型）。
struct CountinuousItemOption
{
public:
    CountinuousItemOption();
    void clear();

    union {
        struct {
            unsigned char m_field0;  // +0x00
            unsigned char m_field1;  // +0x01
            unsigned char m_field2;  // +0x02
        };
        struct __attribute__((packed)) {   // pack 保类对齐 1（m_option1b9 奇偏移不漂移）
            unsigned short m_sec16;  // +0x00 16位视图（Lane7 GetCoolTimeOption
                                     // ORIG 0x86946db movzwl 0x2d8(%edx) 实证）
        };
    };
};   // sizeof 3

// ---- AvatarOptionChangeType（ORIG W 0x08513b6e C1 / 0x08513b48 clear；sizeof 3）----
// clear 链：m_b2 = 0xff → m_b1 = m_b2 → m_b0 = m_b1（movb/movzbl 逐级传播）。trivial 析构。
// 由 STStackableScript.h 迁入（CStackableItem +0x365 子对象类型）。
struct AvatarOptionChangeType
{
public:
    AvatarOptionChangeType();
    void clear();

    unsigned char m_field0;  // +0x00
    unsigned char m_field1;  // +0x01
    unsigned char m_field2;  // +0x02
};   // sizeof 3

// ---- 附魔键/值（STEquipmentScript.h 已含 STEnchantSystemData 权威基类）----
// ORIG 布局实证：
//   _ZN19STEnchantSystemData5clearEv W 0x854b1a8 → 基类 {int@0, byte@4(清0), int@8}
//   _ZN21STEnchantSystemMapKeyC1Ev  W 0x854b268 → 调基类 ctor 后仅置 this+0xc=0
//     （即 MapKey 继承 STEnchantSystemData 并新增 +0x0c 单字节，sizeof 16）
//   _ZNK21STEnchantSystemMapKeyltERKS_ W 0x854b1c8（57 条）：逐字段
//     <+0x00 int、+0x04 u8(movzbl)、+0x08 int、+0x0c u8(unsigned jae)>
//     标准字典序；同字段相等才继续下一字段。
// 由 STStackableScript.h 迁入（CStackableItem m_enchantMap(+0x2f4) 键类型）。
struct STEnchantSystemMapKey : public STEnchantSystemData
{
    unsigned char m_fieldC;   // +0x0c

    STEnchantSystemMapKey()
    {
        m_fieldC = 0;
    }

    // ORIG D1/D2 W 0x08513b2e 为 5 字节空体（inline 弱副本）：基类析构同样
    // 5 字节空，派生不生成对基类 D2 的调用。保留用户空析构以对齐符号集。
    ~STEnchantSystemMapKey() {}

    bool operator<(const STEnchantSystemMapKey& o) const
    {
        if (m_a < o.m_a)
            return true;
        if (m_a != o.m_a)
            return false;
        // ORIG 0x854b201: m_b(+0x4) 第二条比较 `cmp %al,%dl; jge`=有符号字节序；
        // 原 (unsigned char) 版产 jae，m_b≥0x80 时与 ORIG 不一致。改 (signed char)。
        if (static_cast<signed char>(m_b) < static_cast<signed char>(o.m_b))
            return true;
        if (static_cast<unsigned char>(m_b) != static_cast<unsigned char>(o.m_b))
            return false;
        if (m_c < o.m_c)
            return true;
        if (m_c != o.m_c)
            return false;
        if (m_fieldC < o.m_fieldC)
            return true;
        return false;
    }
};

// ---- STEnchantSystemMapData（ORIG C1 0x089c3384 / C1ERKS 0x08527d54 /
//      D1 0x085218cc，全 W → 头内联；D1==D2、C1==C2 同址）----
// C1：vector<EquipmentSkillDataUp>@+0x00 构造 → string@+0x0c 构造 → 双 clear；
// C1ERKS：vector 拷贝 → string 拷贝；D1：string 析构 → vector 析构（unwind 清理）。
// sizeof 0x10（vector 0xc + string 0x4，EquipmentSkillDataUp 见 EquipmentParameterInfo.h）。
// 由 STStackableScript.h 迁入（CStackableItem m_enchantMap(+0x2f4) 值类型）。
struct STEnchantSystemMapData
{
    std::vector<EquipmentSkillDataUp> m_vec0;   // +0x00
    std::string m_strC;                         // +0x0c

    STEnchantSystemMapData()
        : m_vec0(), m_strC()
    {
        m_vec0.clear();
        m_strC.clear();
    }

    STEnchantSystemMapData(const STEnchantSystemMapData& other)
        : m_vec0(other.m_vec0), m_strC(other.m_strC)
    {
    }

    ~STEnchantSystemMapData()
    {
    }
};

struct stStackableBoosterItemInfo_t
{
    int m_itemIdx;              // +0x00（item index）
    int m_prob;                 // +0x04（概率/范围）
    unsigned short m_addInfo;   // +0x08（set_add_info / 附加数量；ORIG movzwl 0x8）
    unsigned char m_byteA;      // +0x0a（endType 附加字段）
    unsigned char m_byteB;      // +0x0b
    unsigned char m_byteC;      // +0x0c
    unsigned char m_avatarType; // +0x0d（[/avatar]/[/special avatar] 写 0x0d）
    unsigned char m_avatarLevel; // +0x0e（同上写 0x0e）
    unsigned char m_pad;        // +0x0f
};
static_assert(sizeof(stStackableBoosterItemInfo_t) == 0x10,
              "stStackableBoosterItemInfo_t ABI drift (ORIG size 0x10)");
static_assert(offsetof(stStackableBoosterItemInfo_t, m_itemIdx) == 0x00,
              "stStackableBoosterItemInfo_t m_itemIdx offset drift");
static_assert(offsetof(stStackableBoosterItemInfo_t, m_prob) == 0x04,
              "stStackableBoosterItemInfo_t m_prob offset drift");
static_assert(offsetof(stStackableBoosterItemInfo_t, m_addInfo) == 0x08,
              "stStackableBoosterItemInfo_t m_addInfo offset drift (ORIG movzwl 0x8)");

struct stStackableBoosterElement_t
{
    // ORIG C1/C2 0x089aee9c（t 局部）：仅构造 m_items（vector 成员）。
    stStackableBoosterElement_t() : m_items() {}
    int m_field0;    // +0x00
    int m_count;     // +0x04
    int m_prob;      // +0x08
    std::vector<stStackableBoosterItemInfo_t> m_items;  // +0x0c
};

struct stStackableBooster_t
{
    std::vector<stStackableBoosterElement_t> m_elements;              // +0x00
    std::map<std::pair<char, char>,
             std::vector<stStackableBoosterItemInfo_t> > m_map;       // +0x0c（ORIG clear 写 +0x28c 处）
    stStackableBooster_t& operator=(const stStackableBooster_t& other);
    void clear();   // ORIG W 0x089c3076：vector::clear(@0) + map::clear(@0xc)；实现 STStackableScript.cpp
    // ORIG T 0x089c2832 / 0x089c295a（非 static 成员；调用方 ImportStackableScript
    // 压 &script->m_booster280 作 this）：脚本 "[xxx]"/"[/xxx]" 标签 → Booster 类型
    // 枚举（0..8），未知标签返回 9（ORIG 默认 `mov $0x9,%eax`）。
    int tag2Type(const char* tag);
    int tag2EndType(const char* tag);
};
static_assert(sizeof(stStackableBoosterElement_t) == 0x18,
              "stStackableBoosterElement_t ABI drift (ORIG construct size 0x18)");


struct stLimitCubeResult
{
    int m_itemIdx;   // +0x00
    int m_count;     // +0x04
    int m_prob;      // +0x08
    // ORIG C1 0x089c3362 / D1 0x089d4262（W 弱符号，共 C1/C2/D1/D2 4 个）：
    //   C1 三次独立重读 this 逐句清零三 int（函数体赋值，非初始化列表，P29 反面形态）；
    //   D1 空体。用户 ctor+空析构 → vector 元素非 trivial（P71 gcc4.1 场景，
    //   gcc4.4 下 _Destroy_aux 实例差异属跨编译器结构性豁免）。
    stLimitCubeResult() { m_itemIdx = 0; m_count = 0; m_prob = 0; }
    ~stLimitCubeResult() {}
};
// ORIG 4 个用户定义符号：_ZN17stLimitCubeResultC1/C2/D1/D2Ev（0x089c3362/0x089d4262 W）；
// vector 元素步进实证 sizeof==0xc（allocate: n*3 shl 2），三 int 无 pad。
static_assert(sizeof(stLimitCubeResult) == 0x0c,
              "stLimitCubeResult ABI drift (ORIG 4 syms, 3 int, sizeof 0xc)");

struct stLimitCubeInfo
{
    std::vector<std::pair<int, int> > m_materials;   // +0x00
    // ORIG：get_2nd_material_index() 读 +0x29c（=m_limitCube+0x0c）
    int m_2ndMaterialIdx;                            // +0x0c
    // ORIG：get_2nd_material_count() 读 +0x2a0（=m_limitCube+0x10）
    int m_2ndMaterialCount;                          // +0x10
    std::vector<stLimitCubeResult> m_results;        // +0x14
};

// ORIG 全局效果结构（0xc 字节；createGlobalEffectInfo 0x89bae94 按
// int + int + float/10000.0f 填充，mangled 名 stGlobalEffect_t）
struct stGlobalEffect_t
{
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    float m_field8;  // +0x08
};

#include "LotteryInfo.h"   // LotteryInfo 唯一权威定义（原本地副本删除；布局一致）
#include "MultiBoxLotteryInfo.h"   // MultiBoxLotteryInfo 唯一权威定义（namespace 旧建模删除，§4.4 合并）

struct RecipeInfo
{
    RecipeInfo();
    ~RecipeInfo();
    void additionalInfo(STStackableScript& script);
    void clear();
    void changeOutput(std::vector<std::pair<int, int> >& out);  // ORIG W 0x08693a78 _ZN10RecipeInfo12changeOutputERSt6vectorISt4pairIiiESaIS4_E

    // ORIG C1 0x8513c6a / clear 0x8513bf2 / D1 0x85144f4，sizeof 0x4c：
    // +0x00/+0x0c/+0x18/+0x28/+0x40 vector<pair<int,int>>
    // +0x24 char +0x25 char +0x34/+0x38/+0x3c int
    std::vector<std::pair<int, int> > m_vec0;    // +0x00
    std::vector<std::pair<int, int> > m_vecC;    // +0x0c
    std::vector<std::pair<int, int> > m_vec18;   // +0x18
    char m_field24;                              // +0x24
    char m_field25;                              // +0x25
    char m_pad26[2];                             // +0x26
    std::vector<std::pair<int, int> > m_vec28;   // +0x28
    int m_field34;                               // +0x34
    int m_field38;                               // +0x38
    int m_field3c;                               // +0x3c
    std::vector<std::pair<int, int> > m_vec40;   // +0x40
};

struct BoosterSelectInfo
{
    BoosterSelectInfo();                          // ORIG C1Ev W 0x08513b82
    BoosterSelectInfo& operator=(const BoosterSelectInfo& other);   // ORIG aSERKS_ W 0x08515128
    int m_field0;                       // +0x00
    int m_field4;                       // +0x04
    int m_field8;                       // +0x08
    std::vector<std::string> m_nameVec; // +0x0c
    char m_field18;                     // +0x18
};

class CStackableItem : public CItem
{
public:
    CStackableItem();
    virtual ~CStackableItem();
    virtual void make_item(Inven_Item& item) const;
    virtual int GetItemType() const;
    int GetMonsterCardId();   // ORIG W 0x0849f530 _ZN14CStackableItem16GetMonsterCardIdEv（非 const；读 +0x2b8）
    virtual bool IsAvatarItem() const;
    virtual bool IsCreatureItem() const;
    virtual bool IsClearAvatar() const;
    virtual bool IsEnchantalbe() const;
    virtual bool getUsableItemType(int type) const;
    virtual bool isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const;
    virtual int GetGuildPowerWarPoint() const;
    virtual int GetUserPowerWarPoint() const;
    virtual bool IsImpossibleGift() const;
    virtual bool IsRandomOptionUnseal() const;
    virtual void set_parent_booster_item_id(unsigned long id);
    virtual unsigned long get_parent_booster_item_id() const;

    bool IsAvatarOptionChangeItem() const;
    // ORIG 0x0822ca48 (W)：movzbl 0x321(%eax),%eax —— 返回 +0x321 字节非零（m_char321）
    bool IsDeletableItem() const;
    // ORIG _ZN14CStackableItem18IsSharedEffectItemEv W 0x0822ca18：movzbl 0x2ec(%eax),%eax —— 返回 m_char2ec 字节（共享特效物品标记）
    char IsSharedEffectItem();
    // ORIG _ZN14CStackableItem13GetEmancipateEv W 0x0854b4e8：movzwl 0x2ee(%eax),%eax —— 返回 m_short2ee（解绑值）
    unsigned short GetEmancipate();
    int check_able_To_use(int job, int level);
    bool check_change_job_type(char changeJob, char checkJob) const;
    bool IsExpAffect();
    // ORIG 0x086946d2 (W 非 const，按值返回)：读 +0x2d8(+0x2da) —— cooltime 时长(秒)/标志
    // 反汇编为 sret 写入（ushort@+0,uchar@+2），证明按值返回 3 字节小 struct。
    // packed 实证：ORIG _ZN5CUser22_processContinuousItemEjP14CStackableItem
    // 0x865ea7d/0x865eab5 sret 临时槽基址 -0xf/-0x12（步长 3、非 4B 对齐），
    // 默认对齐版 sizeof=4 恒反产 -0x10/-0x14（probe 实证）。sizeof 4→3。
    struct CoolTimeOption { unsigned short sec; char flag; } __attribute__((packed));
    CoolTimeOption GetCoolTimeOption();
    // ORIG 0x086946f4 (W 非 const)：读 +0x2e4(+0x2e6) —— effect 时长(秒)/标志
    CoolTimeOption GetEffectItemOption();
    // ORIG 0x08694716 (W 非 const)：读 +0x2dc(+0x2e0) —— stat change 时长(秒)/状态
    // ORIG ctor 0x085141a8（mangled _ZN14StatChangeInfoC1Ev → 全局类型）：
    // movl 0→+0 / movb 0→+4 / movb 0→+5（flag 仅低 2 字节）。定义见 CStackableItem.h 头部。
    typedef ::StatChangeInfo StatChangeInfo;   // P105：保留既有限定名，mangling 归全局域
    ::StatChangeInfo GetStatChangeInfo();
    // ORIG 0x08694746 (W 非 const)：flds +0x2e8 —— 返回经验加成比例(float)
    float GetExpAffectRatio();
    void getBoosterItem(std::vector<stStackableBoosterItemInfo_t>& out) const;
    int get_1st_material_count(unsigned int itemIdx);
    // ORIG 0x822c9e0：mov 0x29c(%eax),%eax —— 返回第 2 材料项的索引（m_limitCube.m_2ndMaterialIdx）
    int get_2nd_material_index();
    // ORIG 0x822c9ee：mov 0x2a0(%eax),%eax —— 返回第 2 材料项的数量（m_limitCube.m_2ndMaterialCount）
    int get_2nd_material_count();
    int get_amplify_reinforce_count(int count);
    // ---- 强化/增幅券访问器（定义 ItemUpgradeVtableB2.cpp；均为 0xf 字节
    //      内联级访问器，objdump 忠实转写）----
    // ORIG _ZN14CStackableItem17IsReinforceTicketEv W 0x084b4222：movzbl +0x30f
    bool IsReinforceTicket();
    // ORIG _ZN14CStackableItem16GetReinforceRateEv W 0x084b4232：movzbl +0x312
    unsigned char GetReinforceRate();
    // ORIG _ZN14CStackableItem24IsAmplifyReinforceTicketEv W 0x084b4242：
    // movzbl +0x313
    bool IsAmplifyReinforceTicket();
    // ORIG _ZN14CStackableItem23GetAmplifyReinforceRateEv W 0x084b4252：
    // movzbl +0x316
    unsigned char GetAmplifyReinforceRate();
    // ORIG _ZN14CStackableItem17GetReinforceVauleEv T 0x0854b4f8：movzwl +0x310
    unsigned short GetReinforceVaule();
    // ORIG _ZN14CStackableItem24GetAmplifyReinforceValueEv T 0x0854b508：
    // movzwl +0x314
    unsigned short GetAmplifyReinforceValue();
    // ORIG 0x854b518：返回 +0x344 字节非零（随机增幅券标记）。
    bool is_random_amplify_reinforce_ticket();
    void get_limit_cube_item(std::pair<int, int>& out);
    void get_lotto_item(char* out);
    void get_multibox_default_item(std::vector<std::pair<Inven_Item, bool> >* out);
    void get_multibox_default_item_arad(std::vector<std::pair<Inven_Item, bool> >* out);
    // ORIG _ZNK14CStackableItem27IsUse2ndUpgradeTableRecipeEv W 0x0869475e（实现=Wave9 CUserRest body）
    bool IsUse2ndUpgradeTableRecipe() const;
    void get_multibox_lottery_item(std::vector<std::pair<Inven_Item, bool> >* out);
    bool is_included_material_for_limit_cube(unsigned int itemIdx);
    int quickslot_priority();
    // ORIG _ZN14CStackableItem12IsEmancipateEv W 0x0822ca38：movzbl +0x2f0 直返（P9）
    unsigned char IsEmancipate();
    void set_item(STStackableScript& script);
    bool verify();
    bool verifyUsablePlace(class CUser* user);
    bool verifyUsableServerType();
    // ORIG 0x0822ca58 (W)：return m_int39c == type
    bool verifyStackableAction(StackableAction::Type type) const;
    int getTradeLimitCount() const;
    int getStackableLimit() const;
    // ORIG 0x822ca0a (W)：return &m_vec280（+0x280）
    const std::vector<stGlobalEffect_t>& getGlobalEffectData() const;
    // ORIG 0x822ca28：movzwl 0x2b4(%eax) —— 返回珍宝目标槽位（16 位，单次零扩展）
    unsigned short getJewelTargetSocket() const;
    int get_sub_type() const;             // ORIG W 0x822c9d2：return m_int2bc（定义 CStackableItem.cpp）
    AradItemInfo m_arad148;               // +0x148（0x2c；m_field0/m_field4/3str/2vec）
    RecipeInfo m_recipe174;               // +0x174
    LotteryInfo m_lotto;                  // +0x1c0
    CMTRand* m_pRand;                     // +0x1e0
    MultiBoxLotteryInfo m_multiBox;  // +0x1e4
    std::vector<STStackablePackageData> m_pkg21c;  // +0x21c
    std::vector<STStackablePackageData> m_pkg228;  // +0x228
    std::vector<std::vector<STStackablePackageData> > m_pkg234;  // +0x234
    stStackableBooster_t m_booster;       // +0x240
    BoosterSelectInfo m_boosterSelect;    // +0x264
    std::vector<stGlobalEffect_t> m_vec280;  // +0x280
    int m_int28c;                         // +0x28c
    stLimitCubeInfo m_limitCube;          // +0x290
    int m_nItemType;                      // +0x2b0
    unsigned short m_jewelTargetSocket;   // +0x2b4（珍宝目标槽位）
    int m_int2b8;                         // +0x2b8
    int m_int2bc;                         // +0x2bc
    std::vector<int> m_usableTypes;       // +0x2c0
    int m_int2cc;                         // +0x2cc
    int m_int2d0;                         // +0x2d0
    int m_int2d4;                         // +0x2d4
    CountinuousItemOption m_option2d8;    // +0x2d8（3 字节）
    StatChangeInfo m_stat2dc;             // +0x2dc（int+2char）
    CountinuousItemOption m_option2e4;    // +0x2e4（3 字节）
    bool m_char2e7;                       // +0x2e7（exp 影响标记；ORIG IsExpAffect movzbl 按 bool 读取，非 char→bool 归一）
    union {                               // +0x2e8（b28c-L8：ORIG GetExpAffectRatio
        int m_int2e8;                     // W 0x08694746 体为 `add $0x2e8; flds (%eax)`
        float m_flt2e8;                   // ⇒ 该字存在 float 视图；同尺寸同偏移，布局不变）
    };
    char m_char2ec;                       // +0x2ec
    char m_pad2ed[1];                     // +0x2ed
    unsigned short m_short2ee;            // +0x2ee
    char m_char2f0;                       // +0x2f0
    std::map<STEnchantSystemMapKey, STEnchantSystemMapData> m_enchantMap;  // +0x2f4
    char m_char30c;                       // +0x30c
    char m_chars30d[2];                   // +0x30d
    bool m_char30f;                       // +0x30f
    unsigned short m_short310;            // +0x310
    char m_char312;                       // +0x312
    bool m_char313;                       // +0x313
    unsigned short m_short314;            // +0x314
    char m_char316;                       // +0x316
    int m_int318;                         // +0x318
    int m_int31c;                         // +0x31c
    bool m_char320;                       // +0x320（区间启用标记；ORIG
                                          // _CheckTargetUsableLv 0x08548c9c
                                          // `movzbl; xor $1; test; je` 按 bool 读取）
    bool m_char321;                       // +0x321（可删除标记；ORIG movzbl 按 bool 读取）
    int m_int324;                         // +0x324
    int m_int328;                         // +0x328
    int m_int32c;                         // +0x32c
    int m_int330;                         // +0x330
    int m_int334;                         // +0x334
    int m_int338;                         // +0x338
    int m_int33c;                         // +0x33c
    int m_int340;                         // +0x340
    bool m_char344;                       // +0x344（随机增幅券标记；ORIG movzbl 按 bool 读取）
    std::map<int, int> m_amplifyMap;      // +0x348
    int m_int360;                         // +0x360
    char m_char364;                       // +0x364
    AvatarOptionChangeType m_avatar365;   // +0x365（3 字节）
    std::set<int> m_set368;               // +0x368
    std::set<int> m_set380;               // +0x380
    bool m_avatarOptionChange;            // +0x398
    // [b28d-N254] ORIG 0x0822ca58 `mov 0x39c(%eax),%eax; mov %eax,%edx` 两段装载
    // ⇒ 字段类型为 enum（int 字段 4.4 恒折单段，P254）。StackableAction::Type 4 字节，
    // sizeof/偏移/mangled 零变化。
    StackableAction::Type m_int39c;       // +0x39c
    std::vector<int> m_vec3a0;            // +0x3a0
    std::vector<StackableAction::UsablePlace> m_usablePlaces;  // +0x3ac
    std::vector<StackableAction::UsableServerType> m_usableServers;  // +0x3b8
    bool m_randomOptionUnseal;            // +0x3c4
    char m_char3c5;                       // +0x3c5
    int m_int3c8;                         // +0x3c8
    int m_int3cc;                         // +0x3cc
    int m_int3d0;                         // +0x3d0
    int m_tradeLimitCount;                // +0x3d4
    std::set<int> m_set3d8;               // +0x3d8
};

#endif  // GAME_CSTACKABLEITEM_H_
