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
#include "CItemList.h"   // ::BaseItemKey / ::BaseItemValue 权威声明
#include <string>
#include <utility>
#include <vector>

// ---- 随机选项脚本嵌套键/值类型（map 模板名与 ORIG mangled 名一致） ----
struct RandomApplicationKey
{
    // ORIG C1(u8,u8) 0x085f414e 仅写 +0/+1 两字节；op< 0x085f4104 按
    // unsigned char 先比 +0 再比 +1（报告缺失，objdump 直转）。
    RandomApplicationKey(unsigned char a, unsigned char b)
        : m_field0(a), m_field1(b)
    {
    }   // ORIG W 0x085f414e
    RandomApplicationKey() : m_field0(0), m_field1(0) {}   // ORIG W 0x08a73b7c
    unsigned char m_field0;   // +0x00（rarity）
    unsigned char m_field1;   // +0x01（level）
    bool operator<(const RandomApplicationKey& o) const
    {
        return m_field0 < o.m_field0 ||
               (m_field0 == o.m_field0 && m_field1 < o.m_field1);
    }
};  // sizeof 2


struct RandomApplicationValue
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04（_random_application：randInt 上界分量）
    int m_field8;   // +0x08（_random_application：阈值比较分量）
};

struct LevelLimitDecisionValue
{
    int m_field0;   // +0x00（_get_level_limit：randInt 下界）
    int m_field4;   // +0x04（_get_level_limit report：bound = m_field0 + m_field4）
};

struct PartSelectonField
{
    int m_partType;   // +0x00
    int m_weight;     // +0x04
};  // sizeof 8（_get_part_type / _get_part_type_by_ItemGroup 报告实证）

struct PartSelectonValue
{
    std::vector<PartSelectonField> m_vec0;   // +0x00（_get_part_type 权重表）
    int m_fieldC;                            // +0x0c（randInt 上界快照）
    std::vector<PartSelectonField> m_vec10;  // +0x10（_get_part_type_by_ItemGroup 权重表）
};  // sizeof 0x1c

struct BaseItemSelectionValue
{
    std::vector<unsigned int> m_vec0;  // +0x00（_get_base_item 累积权重表）
    int m_fieldC;                      // +0x0c（randInt 上界；pair+0x10 实证）
};  // sizeof 0x10


struct QuantityRatio
{
    int m_field0;             // +0x00
    unsigned int m_weight4;   // +0x04（_get_option_quantity 读 +4 为权重；
                              //   步长 8 系 OptionValueRatio 同构推断，标注[推断]）
};  // sizeof 8

struct OptionValueRatio
{
    float m_scaleRange0;       // +0x00（handle::_get_option_value 以 (max-min) 加权；
                               //   W ctor 0x085f4176 三字清零，报告缺失 objdump 实证）
    float m_scaleBase4;        // +0x04（ROI::RegenVer_get_option_value 以 min 加权）
    unsigned int m_weight8;    // +0x08（CRandomListEx 权重字）
};  // sizeof 0xc

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
    short m_minA;   // +0x00（*(short*)(pair+4)：数量下限）
    short m_maxB;   // +0x02（*(short*)(pair+6)：数量上限，0 视为无效）
};  // sizeof 4

struct OptionGroupChoiceValue
{
    std::vector<int> m_vec0;                // +0x00（组 id 候选，at() 取值）
    std::vector<unsigned int> m_vecC;       // +0x0c（CRandomList<unsigned> 权重表）
    int m_field18;                          // +0x18（randInt 上界；pair+0x20 实证）
};  // sizeof 0x1c

struct OPTION_VALUE_RATIO
{
    std::vector<OptionValueRatio> m_vec0;  // +0x00
    int m_c;                               // +0x0c
    void clear();
};

struct DifferentWeightValue
{
    DifferentWeightValue();   // W 0x085f419a
    int m_field0;   // +0x00（W ctor 0x085f419a 双字清零；_get_option_value 两参来源）
    int m_field4;   // +0x04
};  // sizeof 8

struct OptionGroup
{
    int m_type0;      // +0x00（低字节写入 RandomOption 槽首字节）
    int m_weight4;    // +0x04（CRandomList<OptionGroup> 权重；give_option 扣减量）
};  // sizeof 8（for_each 按值两字传递，报告缺失 objdump 实证）

struct OptionGroupValue
{
    std::vector<OptionGroup> m_vec0;  // +0x00（give_option 候选组表，拷贝后逐个 erase）
    int m_fieldC;                     // +0x0c（randInt 上界；pair+0x10 实证）
};  // sizeof 0x10

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

struct ResetValue
{
    int m_min;   // +0x00（同时充当 CRandomList<ResetValue> 的权重字）
    int m_max;   // +0x04
};  // sizeof 8（reset_option 报告实证）

struct ResetValues
{
    std::vector<ResetValue> m_values;  // +0x00（reset_option 报告：vector::size/begin/end 于 pair+4）
    int m_count;                       // +0x0c（randInt 上界-1 来源；pair+0x10 实证）
};  // sizeof 0x10

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
    // ORIG RegenVer_choose_item 报告实证：+0x60 为 map<BaseItemKey,
    // map<int,BaseItemValue>>（键=u8 rarity/u8 level/u16 partType）。
    std::map<BaseItemKey, std::map<int, BaseItemValue> > m_baseItems60;  // +0x60
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
    // +0x148：ORIG 多态 provider 对象（CRandomOptionGive 派生，vptr+双槽，
    //   sizeof 0x18；由 importRandomOptionScript 批次建模构造）。本批仅保留
    //   偏移占位，保证 m_map160 及其后成员偏移不变。
    unsigned char m_providerPad148[0x18];                             // +0x148
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

// ---- 脚本内嵌 provider 抽象（script+0x148 对象的静态视图）------------------
// vtable 槽序按 _get_option_type(槽0)/_choose_option_group(槽1) 报告实证：
//   槽0 返回 map<RandomApplicationKey, map<int, RandomOptionGain>>& —— 即
//     _get_option_type 的 find/end 宿主；槽1 返回 u64→OptionGroupChoiceValue 表。
// 注意：ORIG 槽0/槽1 即业务虚函数，无虚析构前置，勿调整顺序。
class CRandomOptionGive
{
public:
    virtual std::map<RandomApplicationKey,
                     std::map<int, RandomOptionGive> >& getGiveMap() = 0;        // 槽0
    virtual std::map<unsigned long long, OptionGroupChoiceValue>&
                                                     getGroupChoiceMap() = 0;    // 槽1
protected:
    ~CRandomOptionGive() {}
};

#endif  // GAME_RANDOMOPTIONSCRIPT_H_
