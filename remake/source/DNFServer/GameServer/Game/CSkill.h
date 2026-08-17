#ifndef DNF_GAME_CSKILL_H_
#define DNF_GAME_CSKILL_H_

// df_game_r 技能定义类 CSkill 还原（G1 玩家域第一批）。
// 布局依据 ORIG 构造/析构（0x08374944 / 0x08379fd8）与各访问器反汇编推导：
// 2 个 list<int>（+0x08/+0x10）、list<_pre_skill>（+0x18）、
// fitness/maxLevel 数组、STSecondGrowTypeFitness[6]（+0x54，16 字节/个）、
// 4 个 vector（+0xe4/+0xf0/+0x108/+0x124）、skillName[100]（+0x131）等。
// sizeof(CSkill) = 0x19c（new 常量验证：CSkillList::init 0x08351090 movl $0x19c）。

#include <list>
#include <string>
#include <vector>

// ---- 依赖最小声明（权威定义属其它 TU）----
class CDataManager;
class CUser;

// STSecondGrowTypeFitness：16 字节（fitness[2] + maxLevel[2]）
class STSecondGrowTypeFitness
{
public:
    STSecondGrowTypeFitness();
    void clear();

    int m_fitness[2];    // +0x00
    int m_maxLevel[2];   // +0x08
};

// CSkill 键命令单元（+0x124 vector 元素，0x18 字节）
#ifndef GAME_STSKILLSCRIPT_H_
struct STKeyCommandUnit
{
    char m_pad[0x18];
};
#endif

// 前置技能记录（全局结构，mangling 10_pre_skill）
struct _pre_skill
{
    int m_index;    // +0x00 技能索引
    int m_jobType;  // +0x04 职业
    int m_group;    // +0x08 组
};

class CSkill
{
public:
    CSkill();
    ~CSkill();

    // ---- 简单访问器（ORIG weak inline，逐条对照反汇编）----
    int get_index() const;
    int get_group() const;
    void set_index(int idx);
    void set_group(int group);
    void set_job_type(int job);
    void set_skill_type(char type);
    void set_level_limit(int limit);
    void reform_ui_group_no(int& uiGroupNo, bool flag, int count);
    void SetStealable(bool flag);
    void SetStealLevelPenalty(int value);
    void SetStealSPPenalty(int value);
    int GetStealLevelPenalty() const;
    int GetStealSPPenalty() const;
    void set_fitGrowType(int idx, int value);
    void set_fitness(int idx, int value);
    int get_fitness(int idx) const;
    void set_type_max_level(int idx, int value);
    int get_type_max_level(int idx) const;
    void set_fitness_second_growtype(int secondGrowType, int value);
    int get_fitness_second_growtype(int secondGrowType) const;
    void set_second_growtype_fitness(int firstGrowType, int secondGrowType, int value);
    int get_second_growtype_fitness(int firstGrowType, int secondGrowType) const;
    void set_second_growtype_max_level(int firstGrowType, int secondGrowType, int value);
    int get_second_growtype_max_level(int firstGrowType, int secondGrowType) const;
    void set_prelearned_skill();
    void add_need_sp(int value);
    void add_need_sfp(int value);
    void add_learned_skill(_pre_skill skill);
    void setSkillName(const char* name);
    const char* getSkillName() const;
    bool is_active_skill() const;
    bool IsSpecialSkill() const;
    char isCommandCustomizing() const;

    // ---- 主逻辑（ORIG strong）----
    int IsStealable(int firstGrowType, int secondGrowType) const;
    bool can_learn(int level, int firstGrowType, int secondGrowType,
                   int targetLevel) const;
    int getMaxSkillLevel(int level, int firstGrowType,
                         int secondGrowType) const;
    int get_need_sp_for_level(int level, int targetLevel) const;
    int get_need_sfp_for_level(int level, int targetLevel) const;
    int get_need_sp_sfp_for_level(int level, int targetLevel) const;
    int get_guildSkillNeedGSP(int count) const;
    int get_guildSkillNeedGold(int count) const;
    int get_needGuildLevel(int count) const;

    // ---- 布局（对齐 ORIG 偏移）----
    int m_index;                            // +0x00
    int m_jobType;                          // +0x04
    std::list<int> m_needSP;                // +0x08
    std::list<int> m_needSFP;               // +0x10
    std::list<_pre_skill> m_preSkillList;   // +0x18
    char m_skillType;                       // +0x20
    char m_pad21[3];                        // +0x21
    int m_fitness[6];                       // +0x24
    int m_typeMaxLevel[6];                  // +0x3c
    STSecondGrowTypeFitness m_secondGrowTypeFitness[6];  // +0x54
    int m_fitnessSecondGrowType[2];         // +0xb4
    int m_group;                            // +0xbc
    int m_fieldC0;                          // +0xc0
    int m_levelLimit;                       // +0xc4
    int m_fieldC8;                          // +0xc8（每级 SP）
    int m_fitGrowType[6];                   // +0xcc
    std::vector<int> m_guildSkillNeedGSP;   // +0xe4
    std::vector<int> m_guildSkillNeedGold;  // +0xf0
    int m_needGuildLevel;                   // +0xfc
    int m_needGuildLevelAdd;                // +0x100
    int m_field104;                         // +0x104
    std::vector<std::vector<int> > m_field108;  // +0x108
    bool m_stealable;                       // +0x114
    char m_pad115[3];                       // +0x115
    int m_stealSPPenalty;                   // +0x118
    int m_stealLevelPenalty;                // +0x11c
    char m_isSpecialSkill;                  // +0x120
    char m_pad121[3];                       // +0x121
    std::vector<STKeyCommandUnit> m_keyCommandList;  // +0x124
    char m_isCommandCustomizing;            // +0x130
    char m_skillName[100];                  // +0x131
    char m_pad195[3];                       // +0x195
    int m_pvpSkillGrade;                    // +0x198
};

#endif  // DNF_GAME_CSKILL_H_
