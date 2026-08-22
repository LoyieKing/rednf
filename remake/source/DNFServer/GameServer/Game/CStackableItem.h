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

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "CItem.h"

class CMTRand;
class CUser;

namespace StackableAction
{
enum UsablePlace
{
    USABLE_PLACE_0 = 0
};
enum UsableServerType
{
    USABLE_SERVER_TYPE_0 = 0
};
}

class STStackableScript;

#pragma pack(push, 1)
struct StackableTail365
{
    unsigned short m_short365;   // +0x00（0x365）
    char m_char367;              // +0x02（0x367）
};
#pragma pack(pop)

struct stStackableBoosterItemInfo_t
{
    int m_itemIdx;   // +0x00
    int m_prob;      // +0x04
};

struct stStackableBoosterElement_t
{
    int m_field0;    // +0x00
    int m_count;     // +0x04
    int m_prob;      // +0x08
    std::vector<stStackableBoosterItemInfo_t> m_items;  // +0x0c
};

struct stStackableBooster_t
{
    std::vector<stStackableBoosterElement_t> m_elements;              // +0x00
    std::map<std::pair<char, char>,
             std::vector<stStackableBoosterItemInfo_t> > m_map;       // +0x0c
    stStackableBooster_t& operator=(const stStackableBooster_t& other);
};

struct stLimitCubeResult
{
    int m_itemIdx;   // +0x00
    int m_count;     // +0x04
    int m_prob;      // +0x08
};

struct stLimitCubeInfo
{
    std::vector<std::pair<int, int> > m_materials;   // +0x00
    int m_field0c;                                   // +0x0c
    int m_field10;                                   // +0x10
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

struct LotteryInfo
{
    LotteryInfo();
    ~LotteryInfo();
    void clear();
    int m_defaultIdx;                        // +0x00
    std::vector<std::pair<int, int> > m_items;   // +0x04
    int m_defaultCount;                      // +0x10
    std::vector<int> m_counts;               // +0x14
};

namespace MultiBoxLotteryInfo
{
struct stDefaultItem
{
    int m_itemIdx;   // +0x00
    int m_count;     // +0x04
    int m_prob;      // +0x08
    char m_flag;     // +0x0c
};

struct LotteryUnit
{
    LotteryUnit();
    LotteryUnit(const LotteryUnit& other);
    ~LotteryUnit();

    int m_count;                       // +0x00
    int m_defaultIdx;                  // +0x04
    int m_defaultNum;                  // +0x08
    char m_defaultFlag;                // +0x0c
    char m_pad0d[3];                   // +0x0d
    std::vector<std::pair<int, int> > m_items;   // +0x10
    std::vector<std::pair<int, int> > m_bonus;   // +0x1c
};

struct LotteryDefaultArad
{
    LotteryDefaultArad();
    LotteryDefaultArad(unsigned int a, unsigned int b, unsigned int c);
    ~LotteryDefaultArad();

    unsigned int m_field0;   // +0x00
    unsigned int m_field4;   // +0x04
    unsigned int m_field8;   // +0x08
};

struct MultiBoxLotteryInfoBody
{
    char m_pad0[0x10];                 // +0x00
    std::vector<LotteryUnit> m_units;  // +0x10
    int m_defaultCount;                // +0x1c
    int m_defaultIdx;                  // +0x20
    int m_defaultNum;                  // +0x24
    char m_defaultFlag;                // +0x28
    char m_pad29[3];                   // +0x29
    std::vector<stDefaultItem> m_items;  // +0x2c
};
}

struct RecipeInfo
{
    RecipeInfo();
    ~RecipeInfo();
    void additionalInfo(STStackableScript& script);
    void clear();

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
    BoosterSelectInfo& operator=(const BoosterSelectInfo& other);
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
    int check_able_To_use(int job, int level);
    bool check_change_job_type(char changeJob, char checkJob) const;
    bool IsExpAffect();
    void getBoosterItem(std::vector<stStackableBoosterItemInfo_t>& out) const;
    int get_1st_material_count(unsigned int itemIdx);
    int get_amplify_reinforce_count(int count);
    void get_limit_cube_item(std::pair<int, int>& out);
    void get_lotto_item(char* out);
    void get_multibox_default_item(std::vector<std::pair<Inven_Item, bool> >* out);
    void get_multibox_default_item_arad(std::vector<std::pair<Inven_Item, bool> >* out);
    void get_multibox_lottery_item(std::vector<std::pair<Inven_Item, bool> >* out);
    bool is_included_material_for_limit_cube(unsigned int itemIdx);
    int quickslot_priority();
    void set_item(STStackableScript& script);
    bool verify();
    bool verifyUsablePlace(class CUser* user);
    bool verifyUsableServerType();
    int getTradeLimitCount() const;
    int getStackableLimit() const;

    int m_int148;                         // +0x148（AradItemInfo 首字段）
    char m_growType;                      // +0x14c
    char m_pad14d[3];                     // +0x14d
    std::string m_str150;                 // +0x150
    std::string m_str154;                 // +0x154
    std::string m_str158;                 // +0x158
    std::vector<int> m_vec15c;            // +0x15c
    std::vector<char> m_vec168;           // +0x168
    RecipeInfo m_recipe174;               // +0x174
    LotteryInfo m_lotto;                  // +0x1c0
    CMTRand* m_pRand;                     // +0x1e0
    MultiBoxLotteryInfo::MultiBoxLotteryInfoBody m_multiBox;  // +0x1e4
    std::vector<char> m_pkg21c;           // +0x21c
    std::vector<char> m_pkg228;           // +0x228
    std::vector<std::vector<char> > m_pkg234;  // +0x234
    stStackableBooster_t m_booster;       // +0x240
    BoosterSelectInfo m_boosterSelect;    // +0x264
    std::vector<stGlobalEffect_t> m_vec280;  // +0x280
    int m_int28c;                         // +0x28c
    stLimitCubeInfo m_limitCube;          // +0x290
    int m_nItemType;                      // +0x2b0
    unsigned short m_short2b4;            // +0x2b4
    char m_pad2b6[2];                     // +0x2b6
    int m_int2b8;                         // +0x2b8
    int m_int2bc;                         // +0x2bc
    std::vector<int> m_usableTypes;       // +0x2c0
    int m_int2cc;                         // +0x2cc
    int m_int2d0;                         // +0x2d0
    int m_int2d4;                         // +0x2d4
    char m_char2d8;                       // +0x2d8
    char m_char2d9;                       // +0x2d9
    char m_char2da;                       // +0x2da
    char m_pad2db[1];                     // +0x2db
    int m_int2dc;                         // +0x2dc
    char m_char2e0;                       // +0x2e0
    char m_char2e1;                       // +0x2e1
    char m_char2e4;                       // +0x2e4
    char m_char2e5;                       // +0x2e5
    char m_char2e6;                       // +0x2e6
    char m_char2e7;                       // +0x2e7
    int m_int2e8;                         // +0x2e8
    char m_char2ec;                       // +0x2ec
    char m_pad2ed[1];                     // +0x2ed
    unsigned short m_short2ee;            // +0x2ee
    char m_char2f0;                       // +0x2f0
    char m_pad2f1[3];                     // +0x2f1..0x2f4
    std::map<int, int> m_enchantMap;      // +0x2f4
    char m_char30c;                       // +0x30c
    char m_chars30d[2];                   // +0x30d
    char m_char30f;                       // +0x30f
    unsigned short m_short310;            // +0x310
    char m_char312;                       // +0x312
    char m_char313;                       // +0x313
    unsigned short m_short314;            // +0x314
    char m_char316;                       // +0x316
    char m_pad317[1];                     // +0x317
    int m_int318;                         // +0x318
    int m_int31c;                         // +0x31c
    char m_char320;                       // +0x320
    char m_char321;                       // +0x321
    char m_pad322[2];                     // +0x322
    int m_int324;                         // +0x324
    int m_int328;                         // +0x328
    int m_int32c;                         // +0x32c
    int m_int330;                         // +0x330
    int m_int334;                         // +0x334
    int m_int338;                         // +0x338
    int m_int33c;                         // +0x33c
    int m_int340;                         // +0x340
    char m_char344;                       // +0x344
    char m_pad345[3];                     // +0x345
    std::map<int, int> m_amplifyMap;      // +0x348
    int m_int360;                         // +0x360
    char m_char364;                       // +0x364
    StackableTail365 m_tail365;           // +0x365
    std::set<int> m_set368;               // +0x368
    std::set<int> m_set380;               // +0x380
    bool m_avatarOptionChange;            // +0x398
    char m_pad399[3];                     // +0x399
    int m_int39c;                         // +0x39c
    std::vector<int> m_vec3a0;            // +0x3a0
    std::vector<StackableAction::UsablePlace> m_usablePlaces;  // +0x3ac
    std::vector<StackableAction::UsableServerType> m_usableServers;  // +0x3b8
    bool m_randomOptionUnseal;            // +0x3c4
    char m_char3c5;                       // +0x3c5
    char m_pad3c6[2];                     // +0x3c6
    int m_int3c8;                         // +0x3c8
    int m_int3cc;                         // +0x3cc
    int m_int3d0;                         // +0x3d0
    int m_tradeLimitCount;                // +0x3d4
    std::set<int> m_set3d8;               // +0x3d8
};

#endif  // GAME_CSTACKABLEITEM_H_
