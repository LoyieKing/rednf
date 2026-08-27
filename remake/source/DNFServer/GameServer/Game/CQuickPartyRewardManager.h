#ifndef DNF_GAME_CQUICK_PARTY_REWARD_MANAGER_H_
#define DNF_GAME_CQUICK_PARTY_REWARD_MANAGER_H_

// ============================================================================
// df_game_r 还原 —— QuickParty::CQuickPartyRewardManager 及 IRewardType 族
//
// ORIG 权威符号（df_game_r nm 实证）：
//   QuickParty::CQuickPartyRewardManager C1 0x0826d688 / D1 0x0826d69c
//   QuickParty::QuickPartyData           C1 0x0826dad4 / D1 0x0826dd52（W）
//   QuickParty::BasicReward              C1 0x0826d9f0 / D1 0x0826da7a（W）
//   QuickParty::LevelDataRandomReward    C1 0x08a6efa2 / 复制 0x8a6f112 / D1 0x826e77e（W）
//   QuickParty::IRewardType::IRewardType(QuickPartyData&)     W 0x0826dbee
//   QuickParty::IBasicReward::{C1 0x826dc06, check 0x826dc2a} （W）
//   QuickParty::IRandomwReward::{C1 0x826dc78, check 0x826dc9c}（W，拼写即 ORIG 原样）
//   RewardCardGold/HellPartyDifficulty/KillMonsterExp/EquipItemDropRate/
//   CointLimitCount/ReturnAncientDungeonTicket 各自 C1（W）与 process（T）
//
// vtable 链验证（ORIG .rodata）：
//   _ZTVN10QuickParty11IRewardTypeE @0x8bf7500 —— 对象 vptr=0x8bf7508 起，
//   仅两槽：slot0 check、slot1 process，均 __cxa_pure_virtual(0x8724fe0)。
//   IBasicReward@0x8bf74f0 / IRandomwReward@0x8bf74e0：check 槽落各自空实现，
//   process 槽仍 pure；六个具体类槽1=自身 T process、槽0=继承基 check。
//   无虚析构槽 —— 全族不声明析构虚函数。
// ============================================================================

#include <map>
#include <string>
#include <vector>

class CParty;

namespace QuickParty
{

// ---- 枚举（权威值实证）----
// RandomBuffType：getRandomBuffTypeByName(0x08a6e100) 脚本名映射实证。
enum RandomBuffType
{
    RANDOM_BUFF_INCREASE_EXP       = 0,   // [increase exp]        → KillMonsterExp
    RANDOM_BUFF_INCREASE_STATS     = 1,   // [increase stats]
    RANDOM_BUFF_INCREASE_SPEED     = 2,   // [increase speed]
    RANDOM_BUFF_INCREASE_DROPPROB  = 3,   // [increase dropprob]   → EquipItemDropRate
    RANDOM_BUFF_INCREASE_ALLATTACK = 4,   // [increase allattack]
    RANDOM_BUFF_INCREASE_ALLRESIST = 5,   // [increase allresist]
    RANDOM_BUFF_INCREASE_BROKEN    = 6,   // [increase broken]
    RANDOM_BUFF_INCREASE_MOBHP     = 7,   // [increase mobhp]
    RANDOM_BUFF_INCREASE_ANTIDEVIL = 8,   // [increase antidevil]
    RANDOM_BUFF_INCREASE_COINLIMIT = 9,   // [increase coinlimit]  → CointLimitCount
    RANDOM_BUFF_RETURN_TICKET      = 10   // [return ticket]       → ReturnAncientDungeonTicket
};

// BasicRewardType：仅 init() 调用点实证（11→RewardCardGold、12→HellPartyDifficulty；
// 脚本节名未还原，命名推断）。
enum BasicRewardType
{
    BASIC_REWARD_CARD_GOLD        = 0xb,
    BASIC_REWARD_HELL_DIFFICULTY  = 0xc
};

// ENUM_REWARD_TYPE：类型名实证于
// _ZN10QuickParty24CQuickPartyRewardManager22get_reward_type_objectENS_16ENUM_REWARD_TYPEEj；
// get_reward_type_object 越界检查（<2）实证取值域 {0,1}，子命名推断。
enum ENUM_REWARD_TYPE
{
    ENUM_REWARD_TYPE_BASIC  = 0,   // 基础奖励族（RewardCardGold/HellPartyDifficulty）
    ENUM_REWARD_TYPE_RANDOM = 1    // 随机 buff 族（IRandomwReward 派生）
};

// RandomBuffDungeon（choose_random_buff 域；权威枚举名待事件批次，占位沿用）
enum RandomBuffDungeon
{
    RANDOM_BUFF_DUNGEON_NONE = 0
};

// ---- RewardData_Param（0x8）/ RewardData_Result（0x1c）----
// ORIG：Param C1/C2 W 0x08469976（m_party=party、+0x04=0）；Result clear W
// 0x0846998e（逐字段清零后 +0x14=1.0f、+0x18=1）、C1/C2 W 0x084699c6。
struct RewardData_Param
{
    RewardData_Param(CParty* party)
        : m_party(party), m_param4(0) {}

    CParty* m_party;   // +0x00
    int m_param4;      // +0x04
};                     // sizeof = 8

struct RewardData_Result
{
    RewardData_Result() { clear(); }

    void clear()
    {
        m_value00 = 0;
        m_value04 = 0;
        m_value08 = 0;
        m_value0c = 0;
        m_value10 = 0;
        m_rate14 = 1.0f;
        m_flag18 = 1;
    }

    int m_value00;     // +0x00（RewardCardGold/KillMonsterExp/CointLimitCount/HellPartyDifficulty 写入槽）
    int m_value04;     // +0x04（HellPartyDifficulty 第二写入槽）
    int m_value08;     // +0x08
    int m_value0c;     // +0x0c
    int m_value10;     // +0x10
    float m_rate14;    // +0x14（EquipItemDropRate 写入槽）
    int m_flag18;      // +0x18（clear 置 1）
};                     // sizeof = 0x1c

// ---- LevelData 族（布局：process 内 vector 元素 +0/+4 访问实证；各 0x8 字节）----
// ORIG C1 为显式非平凡构造（W）：两 int 字段全置 -1；D1 空（W 0x8a6fcb8/0x8a6fefa）。
// 头内联定义产出弱符号与 ORIG 一致。
struct LevelData
{
    LevelData();   // ORIG W 0x08a6ef72（外联定义 → 同弱符号）

    int m_field0;    // +0x00
    int m_value4;    // +0x04（比率基值，各 process 以 /100.0f 使用）
};

struct LevelDataHell
{
    LevelDataHell();   // ORIG W 0x08a6ef8a

    int m_field0;    // +0x00（写入 result.m_value04）
    int m_value4;    // +0x04（写入 result.m_value00）
};

// LevelDataRandomReward（0x14）：ctor 置 +0x00=0xb、string 默认+clear、
// vector<LevelData> 默认+clear（ORIG W 0x8a6efa2 逐指令）。头内联 → 弱符号同 ORIG。
class LevelDataRandomReward
{

public:
    LevelDataRandomReward()
        : m_field0(0xb)
    {
        m_name.clear();
        m_levelDatas.clear();
    }

    int m_field0;                          // +0x00（ctor 默认 0xb）
    std::string m_name;                    // +0x04
    std::vector<LevelData> m_levelDatas;   // +0x08
};                                         // sizeof = 0x14

// ---- BasicReward（0x20；QuickPartyData +0x18 内嵌子对象）----
// ctor：vector<LevelData> C1(+0)、vector<LevelDataHell> C1(+0xc)、+0x18/+0x1c=-1，
// 再双 clear（ORIG 0x826d9f0 逐指令）。
class BasicReward
{
public:
    BasicReward()
        : m_field18(-1), m_field1c(-1)
    {
        m_levelDatas.clear();
        m_hellDatas.clear();
    }
    ~BasicReward() {}

    std::vector<LevelData> m_levelDatas;      // +0x00（相对 BasicReward）
    std::vector<LevelDataHell> m_hellDatas;   // +0x0c
    int m_field18;                            // +0x18（ctor 置 -1）
    int m_field1c;                            // +0x1c（ctor 置 -1）
};                                            // sizeof = 0x20

class IRewardType;
// ---- QuickPartyData（0x228；CQuickPartyRewardManager 的唯一基，偏移全实证）----
// ctor 0x826dad4：两 vector<int> C1、BasicReward C1、map C1、三容器 clear、
// +0x1a0[2][2]=-1 双重循环。dtor 逆序。
class QuickPartyData
{
public:
    QuickPartyData();
    ~QuickPartyData();

    std::vector<int> m_vec00;                // +0x00（importQuickPartyScript 填充）
    std::vector<int> m_dungeonIdxList;       // +0x0c（get_dungeon_idx_list 返回值）
    BasicReward m_basicReward;               // +0x18（0x20）
    std::map<RandomBuffType, LevelDataRandomReward> m_randomRewards;  // +0x38
    // 补齐垫（覆盖 ORIG +0x4c 区）：随工具链 sizeof(map) 浮动，钉住 +0x50。
    unsigned char m_padAfterMap[0x50 - 0x38 -
        sizeof(std::map<RandomBuffType, LevelDataRandomReward>)];
    int m_probs[12][7];                      // +0x50..0x19f（行 0..10 明细，行 11 总和）
    int m_minCounts[2][4];                   // +0x1a0..0x1bf（ctor 置前 [2][2]=-1；
                                             //  getRequireQuickPartyMinCount 按
                                             //  行 stride 0x10 / 列 stride 4 寻址实证）
    class IRewardType* m_rewards[2][13];     // +0x1c0..0x227（奖励对象表）
};                                           // sizeof = 0x228

// 布局断言（ORIG 0x228 / 概率表 +0x50 / 奖励表 +0x1c0 实证）。
static_assert(sizeof(QuickPartyData) == 0x228, "QuickPartyData size drift");
static_assert(sizeof(((QuickPartyData*)0)->m_probs) == 0x150, "probs drift");

// ---- IRewardType 抽象族 ----
// ORIG vtable 仅 check/process 两纯虚槽（0x8bf7508 链）；无虚析构。
class IRewardType
{
public:
    IRewardType(QuickPartyData& data);       // W 0x0826dbee：vptr + 引用

    virtual void check(RewardData_Param param) = 0;                            // 槽0
    virtual void process(RewardData_Param param, RewardData_Result& result) = 0;  // 槽1

protected:
    QuickPartyData& m_data;                  // +0x04
};

class IBasicReward : public IRewardType
{
public:
    IBasicReward(QuickPartyData& data);      // W 0x0826dc06
    virtual void check(RewardData_Param param) {}   // ORIG 0x826dc2a：空实现
};

class IRandomwReward : public IRewardType    // 拼写与 ORIG 一致（IRandomwReward）
{
public:
    IRandomwReward(QuickPartyData& data);    // W 0x0826dc78
    virtual void check(RewardData_Param param) {}   // ORIG 0x826dc9c：空实现

protected:
    RandomBuffType m_type;                   // +0x08（派生 ctor 写入）
};

// ---- 六个具体奖励类（process 均 T 符号，报告逐一转写）----
class RewardCardGold : public IBasicReward
{
public:
    RewardCardGold(BasicRewardType type, QuickPartyData& data);   // W 0x0826dc30（type 不存储）
    virtual void process(RewardData_Param param, RewardData_Result& result);  // T 0x0826d070
};

class HellPartyDifficulty : public IBasicReward
{
public:
    HellPartyDifficulty(BasicRewardType type, QuickPartyData& data);  // W 0x0826dc54
    virtual void process(RewardData_Param param, RewardData_Result& result);  // T 0x0826d116
};

class KillMonsterExp : public IRandomwReward
{
public:
    KillMonsterExp(RandomBuffType type, QuickPartyData& data);   // W 0x0826dca2
    virtual void process(RewardData_Param param, RewardData_Result& result);  // T 0x0826d198
};

class EquipItemDropRate : public IRandomwReward
{
public:
    EquipItemDropRate(RandomBuffType type, QuickPartyData& data);  // W 0x0826dcce
    virtual void process(RewardData_Param param, RewardData_Result& result);  // T 0x0826d2aa
};

class CointLimitCount : public IRandomwReward
{
public:
    CointLimitCount(RandomBuffType type, QuickPartyData& data);  // W 0x0826dcfa
    virtual void process(RewardData_Param param, RewardData_Result& result);  // T 0x0826d4ac
};

class ReturnAncientDungeonTicket : public IRandomwReward
{
public:
    ReturnAncientDungeonTicket(RandomBuffType type, QuickPartyData& data);  // W 0x0826dd26
    virtual void process(RewardData_Param param, RewardData_Result& result);  // T 0x0826d598
};
class QuestItemDropRate : public IRandomwReward
{
public:
    // ORIG 无独立 C1 符号（实例从未创建）；仅 T @0x0826d398 process。
    virtual void process(RewardData_Param param, RewardData_Result& result);
};

class DimensionInoutCount : public IRandomwReward
{
public:
    // ORIG 无独立 C1 符号；仅 T @0x0826d5ac process。
    virtual void process(RewardData_Param param, RewardData_Result& result);
};


// importQuickPartyScript（ORIG T 0x08a6e282）：脚本导入器本体待脚本导入域批次
// （报告 docs/class_func_reports/QuickParty/importQuickPartyScript.md）。
bool importQuickPartyScript(const char* path, QuickPartyData* data);

// ---- 管理器（继承 QuickPartyData，sizeof 仍 0x228）----
class CQuickPartyRewardManager : public QuickPartyData
{
public:
    CQuickPartyRewardManager();              // T 0x0826d688（调 QuickPartyData C1）
    ~CQuickPartyRewardManager();             // T 0x0826d69c（调 QuickPartyData D1）

    bool init();                             // T 0x0826d6b0（CDataManager::init 调用）
    void set_rand_buff_total_prob();         // T 0x0826d91a（init 尾部调用）
    // ORIG T 0x0826d860（报告 QuickParty__CQuickPartyRewardManager/
    // get_random_buff_prob.md）：m_probs[type][dungeon] 查询（仅上界校验，
    // 越界返回 0；行 11 为 set_rand_buff_total_prob 汇总行）。
    int get_random_buff_prob(RandomBuffType type, RandomBuffDungeon dungeon);

    // ORIG @0x0826d896：返回 +0x0c 处副本 index 列表。
    std::vector<int>& get_dungeon_idx_list() { return m_dungeonIdxList; }

    // ORIG W 0x0826d8e2
    // （docs/class_func_reports/QuickParty__CQuickPartyRewardManager/
    //   get_reward_type_object.md）：类别 <2 且 idx <0xd 时返回
    // m_rewards[type][idx]（(cat*13+idx+0x70)*4 寻址实证），否则返回 0。
    IRewardType* get_reward_type_object(ENUM_REWARD_TYPE type, unsigned int idx)
    {
        if ((int)type < 2 && idx < 0xd)
            return m_rewards[(int)type][idx];
        return 0;
    }

    // ORIG T 0x0826d8a2：type∈[0,1]、diff∈[0,3] 时查 m_minCounts[type][diff]，
    // 否则 -1。（报告 getRequireQuickPartyMinCount.md）
    int getRequireQuickPartyMinCount(int type, int dungeonDiff);
};

}  // namespace QuickParty

#endif  // DNF_GAME_CQUICK_PARTY_REWARD_MANAGER_H_
