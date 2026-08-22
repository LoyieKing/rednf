#ifndef GAME_RANDOMOPTIONSCRIPT_H_
#define GAME_RANDOMOPTIONSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— RandomOptionScript（G5 随机选项脚本表，尺寸 0x230）
// 布局依据 ORIG clear（0x0837261e）/ ctor（0x08372904）推导；
// 方法对照 docs/class_func_reports/RandomOptionScript.md：
//   getPrefixGrade 0x08a73a80 / getPrefix 0x08a73956（sret 返回 string）
//   getOptionModificationCost 0x08a738a0（const）
// ============================================================================

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// ---- 随机选项脚本嵌套键/值类型（map 模板名与 ORIG mangled 名一致） ----
struct RandomApplicationKey
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    bool operator<(const RandomApplicationKey& o) const
    {
        return m_field0 < o.m_field0 ||
               (m_field0 == o.m_field0 && m_field4 < o.m_field4);
    }
};

struct RandomApplicationValue
{
    int m_field0;   // +0x00
};

struct LevelLimitDecisionValue
{
    int m_field0;   // +0x00
};

struct PartSelectonValue
{
    int m_field0;   // +0x00
};

struct BaseItemSelectionValue
{
    int m_field0;   // +0x00
};

struct BaseItemValue
{
    int m_field0;   // +0x00
};

struct QuantityRatio
{
    int m_field0;   // +0x00
};

struct OptionValueRatio
{
    int m_field0;   // +0x00
};

struct RandomOptionGive
{
    RandomOptionGive();
    RandomOptionGive(const RandomOptionGive& other);
    ~RandomOptionGive();
    void clear();

    std::vector<unsigned int> m_vec0;  // +0x00
    int m_c;                           // +0x0c
};

struct OptionQuantityValue
{
    int m_field0;   // +0x00
};

struct OptionGroupChoiceValue
{
    int m_field0;   // +0x00
};

struct OPTION_VALUE_RATIO
{
    std::vector<OptionValueRatio> m_vec0;  // +0x00
    int m_c;                               // +0x0c
    void clear();
};

struct DifferentWeightValue
{
    int m_field0;   // +0x00
};

struct OptionGroupValue
{
    int m_field0;   // +0x00
};

struct ChoosePrefix
{
    int m_min;      // +0x00
    int m_max;      // +0x04
    int m_grade;    // +0x08（grade + 1）
};

struct OPTION_MODIFICATION_COST
{
    short m_level;  // +0x00
    int m_cost2;    // +0x04
    int m_cost3;    // +0x08
};

struct ResetValues
{
    int m_field0;   // +0x00
};

class RandomOptionScript
{
public:
    RandomOptionScript();
    ~RandomOptionScript();
    void clear();
    int getPrefixGrade(int level) const;
    std::string getPrefix(int prefix, bool flag);
    std::string getPrefix(int prefix, int grade, bool flag);  // ORIG 0x08a73956

    int getOptionModificationCost(short level, int rarity) const;

    // ---- 成员布局（0x230 字节） ----
    std::map<RandomApplicationKey, RandomApplicationValue> m_map0;    // +0x00
    std::map<RandomApplicationKey, LevelLimitDecisionValue> m_map18;  // +0x18
    std::map<int, PartSelectonValue> m_map30;                         // +0x30
    std::map<RandomApplicationKey, BaseItemSelectionValue> m_map48;   // +0x48
    std::map<int, std::vector<std::pair<int, BaseItemValue> > > m_map60;  // +0x60
    std::map<unsigned long, int> m_map78;                             // +0x78
    std::map<RandomApplicationKey,
             std::vector<std::pair<int, struct RandomOptionGive> > > m_map90;  // +0x90
    std::vector<QuantityRatio> m_veca8;                               // +0xa8
    std::map<int, OptionQuantityValue> m_mapb4;                       // +0xb4
    std::map<unsigned long long, OptionGroupChoiceValue> m_mapcc;     // +0xcc
    OPTION_VALUE_RATIO m_ratioE4;                                     // +0xe4
    std::map<short, DifferentWeightValue> m_mapf4;                    // +0xf4
    std::vector<OPTION_MODIFICATION_COST> m_costVec10c;               // +0x10c
    std::map<RandomApplicationKey,
             std::vector<std::pair<int, struct RandomOptionGive> > > m_map118;  // +0x118
    std::map<unsigned long long, OptionGroupChoiceValue> m_map130;    // +0x130
    std::map<int, OptionGroupValue> m_map160;                         // +0x160
    std::map<int, int> m_map178;                                      // +0x178
    std::map<int, int> m_map190;                                      // +0x190
    std::map<int, std::vector<std::string> > m_prefixNormal;          // +0x1a8
    std::map<int, std::vector<std::string> > m_prefixPremium;         // +0x1c0
    std::vector<ChoosePrefix> m_choosePrefix1d8;                      // +0x1d8
    std::string m_str1e4;                                             // +0x1e4
    std::map<unsigned char, ResetValues> m_map1e8;                    // +0x1e8
    std::set<unsigned long> m_set200;                                 // +0x200
    std::set<unsigned long> m_set218;                                 // +0x218
};

bool importRandomOptionScript(RandomOptionScript& script, const char* path);

#endif  // GAME_RANDOMOPTIONSCRIPT_H_
