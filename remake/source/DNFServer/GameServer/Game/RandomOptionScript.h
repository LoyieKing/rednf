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
#include "CItemAmplifier.h"  // ENUM_RARITY（ORIG mangled Es11ENUM_RARITY；guard 幂等）
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

    // ORIG W 0x08a787f8：D1/D2 空体 → 显式空析构（与 QuantityRatio/ResetValue 同型）。
    ~PartSelectonField() {}
};  // sizeof 8（_get_part_type / _get_part_type_by_ItemGroup 报告实证）

struct PartSelectonValue
{
    // ORIG C1 W 0x08a73b8e（双 vector ctor + clear）/ C1ERKS W 0x08a741fc /
    // D1 W 0x083ced2c（先 +0x10 后 +0x00 反序析构）/ clear W 0x08a73bf6，
    // 全 W → 头内联。
    PartSelectonValue() : m_vec0(), m_vec10() { clear(); }
    PartSelectonValue(const PartSelectonValue& o)
        : m_vec0(o.m_vec0), m_fieldC(o.m_fieldC), m_vec10(o.m_vec10) {}
    ~PartSelectonValue() {}
    void clear()
    {
        m_vec0.clear();
        m_fieldC = 0;
        m_vec10.clear();
    }

    std::vector<PartSelectonField> m_vec0;   // +0x00（_get_part_type 权重表）
    int m_fieldC;                            // +0x0c（randInt 上界快照）
    std::vector<PartSelectonField> m_vec10;  // +0x10（_get_part_type_by_ItemGroup 权重表）
};  // sizeof 0x1c

struct BaseItemSelectionValue
{
    // ORIG C1 W 0x08a73c22（vector ctor + clear）/ C1ERKS W 0x08a74308 /
    // D1 W 0x083cee22（仅 vector dtor）/ clear W 0x08a73c64，全 W → 头内联。
    BaseItemSelectionValue() : m_vec0() { clear(); }
    BaseItemSelectionValue(const BaseItemSelectionValue& o)
        : m_vec0(o.m_vec0), m_fieldC(o.m_fieldC) {}
    ~BaseItemSelectionValue() {}
    void clear()
    {
        m_vec0.clear();
        m_fieldC = 0;
    }

    std::vector<unsigned int> m_vec0;  // +0x00（_get_base_item 累积权重表）
    int m_fieldC;                      // +0x0c（randInt 上界；pair+0x10 实证）
};  // sizeof 0x10


struct QuantityRatio
{
    int m_field0;             // +0x00
    unsigned int m_weight4;   // +0x04（_get_option_quantity 读 +4 为权重；
                              //   步长 8 系 OptionValueRatio 同构推断，标注[推断]）
    // ORIG W 0x08a79264：D1/D2 空体（new_allocator<QuantityRatio>::destroy
    //   调用 D1 实证非 trivial）→ 显式空析构。
    ~QuantityRatio() {}
};  // sizeof 8

struct OptionValueRatio
{
    // ORIG C1 W 0x085f4176（三字清零）/ D1 W 0x08a799f8（空体用户析构），
    // 无 C1ERKS 符号（隐式拷贝），全 W → 头内联。
    OptionValueRatio() : m_scaleRange0(0), m_scaleBase4(0), m_weight8(0) {}
    ~OptionValueRatio() {}

    float m_scaleRange0;       // +0x00（handle::_get_option_value 以 (max-min) 加权；
                               //   W ctor 0x085f4176 三字清零）
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
    // ORIG C1 W 0x08a73ce2（双 vector ctor + clear）/ C1ERKS W 0x08a746a6 /
    // D1 W 0x083cf12a（先 +0xc 后 +0x00 反序析构）/ clear W 0x08a73d4a，
    // 全 W → 头内联。
    OptionGroupChoiceValue() : m_vec0(), m_vecC() { clear(); }
    OptionGroupChoiceValue(const OptionGroupChoiceValue& o)
        : m_vec0(o.m_vec0), m_vecC(o.m_vecC), m_field18(o.m_field18) {}
    ~OptionGroupChoiceValue() {}
    void clear()
    {
        m_vec0.clear();
        m_vecC.clear();
        m_field18 = 0;
    }

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
    // ORIG C1 W 0x08a73d76（vector ctor + clear）/ C1ERKS W 0x08a74912 /
    // D1 W 0x083cf30e（仅 vector dtor）/ clear W 0x08a73db8，全 W → 头内联。
    OptionGroupValue() : m_vec0() { clear(); }
    OptionGroupValue(const OptionGroupValue& o)
        : m_vec0(o.m_vec0), m_fieldC(o.m_fieldC) {}
    ~OptionGroupValue() {}
    void clear()
    {
        m_vec0.clear();
        m_fieldC = 0;
    }

    std::vector<OptionGroup> m_vec0;  // +0x00（give_option 候选组表，拷贝后逐个 erase）
    int m_fieldC;                     // +0x0c（randInt 上界；pair+0x10 实证）
};  // sizeof 0x10

struct ChoosePrefix
{
    ChoosePrefix(int min, int max, int grade) : m_min(min), m_max(max), m_grade(grade) {}  // ORIG W 0x08a73dd6
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
    // 12 字节实证：ORIG _Vector_base<ResetValue>::~_Vector_base
    //   (0x0839fe6a) 步长 sar$2+imul 0xaaaaaaab = /12；_M_insert_aux
    //   (0x08a77b45) 异常清理 lea(%edx,%edx,2)+shl$2 亦 /12。
    // ORIG W 0x08a7a958：D1/D2 空体；new_allocator<ResetValue>::destroy(0x08a7a95e)
    //   call D1 实证非 trivial → 显式空析构（同 QuantityRatio/PartSelectonField 型）。
    int m_min;      // +0x00（reset_option 报告：区间下限）
    int m_max;      // +0x04（区间上限；pair 用 (m_max-m_min-1) 作 randInt 界）
    int m_weight8;  // +0x08（CRandomList<unsigned>::operator()(ResetValue)
                    //   0x085f5e9e 读 0x14(%ebp) 作权重累加）
    ~ResetValue() {}
};  // sizeof 0xc

struct ResetValues
{
    // ORIG C1 W 0x08a73df6（vector ctor + clear）/ clear W 0x08a73e38 /
    // C1ERKS W 0x08a74cda / D1 W 0x083cf4b4（仅 vector dtor），全 W → 头内联。
    ResetValues() : m_values() { clear(); }
    ResetValues(const ResetValues& o)
        : m_values(o.m_values), m_count(o.m_count) {}
    ~ResetValues() {}
    void clear()
    {
        m_values.clear();
        m_count = 0;
    }

    std::vector<ResetValue> m_values;  // +0x00（reset_option 报告：vector::size/begin/end 于 pair+4）
    int m_count;                       // +0x0c（randInt 上界-1 来源；pair+0x10 实证）
};  // sizeof 0x10

// ---- 脚本内嵌 provider 抽象（script+0x148 对象的静态视图）------------------
// vtable 槽序按 _get_option_type(槽0)/_choose_option_group(槽1) 报告实证：
//   槽0 返回 map<RandomApplicationKey, map<int, RandomOptionGive>>& —— 即
//     _get_option_type 的 find/end 宿主；槽1 返回 u64→OptionGroupChoiceValue 表。
// 注意：ORIG 槽0/槽1 即业务虚函数，无虚析构前置，勿调整顺序。
// 派生类 CRandomOptionGiveNormal(+0x148)/CModifiedRandomOptionGive(+0x154)
// 布局均为 vptr+双指针（ORIG W ctor/getter 0x08372550..0x08372640 objdump）。
class CRandomOptionGive
{
public:
    virtual std::map<RandomApplicationKey,
                     std::map<int, RandomOptionGive> >& get_option_type() = 0;        // 槽0
    virtual std::map<unsigned long long, OptionGroupChoiceValue>&
                                     get_option_group_choice() = 0;                    // 槽1
// ORIG 该基类无析构（nm 无 D1/D2/D0；派生类亦无 D1）——声明析构会使
// 派生类生成 D1，RandomOptionScript C1 异常清理链多出 provider 的 D1 调用。
protected:
};

// ---- 派生 provider（原 CRandomOptionGiveNormal.h/CModifiedRandomOptionGive.h 定义移入本头，
//      打破二者 include RandomOptionScript.h 的循环；独立头改为仅 include 本头）----
class CRandomOptionGiveNormal : public CRandomOptionGive
{
public:
    CRandomOptionGiveNormal();
    virtual std::map<RandomApplicationKey,
                     std::map<int, RandomOptionGive> >& get_option_type();
    virtual std::map<unsigned long long, OptionGroupChoiceValue>&
                                     get_option_group_choice();
    void set(std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >*,
             std::map<unsigned long long, OptionGroupChoiceValue>*);

private:
    std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >* m_pMapType4;    // +0x04
    std::map<unsigned long long, OptionGroupChoiceValue>* m_pMapChoice8;              // +0x08
};

class CModifiedRandomOptionGive : public CRandomOptionGive
{
public:
    CModifiedRandomOptionGive();
    virtual std::map<RandomApplicationKey,
                     std::map<int, RandomOptionGive> >& get_option_type();
    virtual std::map<unsigned long long, OptionGroupChoiceValue>&
                                     get_option_group_choice();
    void set(std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >*,
             std::map<unsigned long long, OptionGroupChoiceValue>*);

private:
    std::map<RandomApplicationKey, std::map<int, RandomOptionGive> >* m_pMapType4;    // +0x04
    std::map<unsigned long long, OptionGroupChoiceValue>* m_pMapChoice8;              // +0x08
};

// 派生 provider 完整类型（RandomOptionScript.h 定义基类后引入；
// 二者 include-guard 幂等，无环）。RandomOptionScript 成员
// m_providerNormal148(+0x148)/m_providerModified154(+0x154) 需要完整类型。

class RandomOptionScript
{
public:
    RandomOptionScript();
    ~RandomOptionScript();
    void clear();
    int getPrefixGrade(int level);  // ORIG 0x08a73a80（非 const，_ZN mangled）
    std::string getPrefix(int prefix, int grade, bool flag);  // ORIG 0x08a73956

    int getOptionModificationCost(short level, ENUM_RARITY rarity) const;

    // ---- 成员布局（0x230 字节） ----
    std::map<RandomApplicationKey, RandomApplicationValue> m_map0;    // +0x00
    std::map<RandomApplicationKey, LevelLimitDecisionValue> m_map18;  // +0x18
    std::map<int, PartSelectonValue> m_map30;                         // +0x30
    std::map<RandomApplicationKey, BaseItemSelectionValue> m_map48;   // +0x48
    // ORIG 实证（choose_item/RegenVer_choose_item mangled）：+0x60 为
    // map<int, map<int, BaseItemValue>>——键 = BaseItemKey 4 字节按小端
    // int 打包（rarity|level<<8|partType<<16）；插入端 CItemList::insert_item
    // 用 *(int*)&key 写同一表（CDataManager +0x4e70 别名视图）。
    std::map<int, std::map<int, BaseItemValue> > m_baseItems60;  // +0x60
    std::map<unsigned long, int> m_map78;                             // +0x78
    std::map<RandomApplicationKey,
             std::map<int, struct RandomOptionGive> > m_map90;  // +0x90（ORIG mangled S_Ii16RandomOptionGive）
    std::vector<QuantityRatio> m_veca8;                               // +0xa8
    std::map<int, OptionQuantityValue> m_mapb4;                       // +0xb4
    std::map<unsigned long long, OptionGroupChoiceValue> m_mapcc;     // +0xcc
    OPTION_VALUE_RATIO m_ratioE4;                                     // +0xe4
    std::map<short, DifferentWeightValue> m_mapf4;                    // +0xf4
    std::vector<OPTION_MODIFICATION_COST> m_costVec10c;               // +0x10c
    std::map<RandomApplicationKey,
             std::map<int, struct RandomOptionGive> > m_map118;  // +0x118（ORIG mangled S_Ii16RandomOptionGive）
    std::map<unsigned long long, OptionGroupChoiceValue> m_map130;    // +0x130
    // +0x148：ORIG 多态 provider 对象（CRandomOptionGive 派生，vptr+双槽，
    //   sizeof 0x18）。clear() 依 ORIG 序 set(&m_map90,&m_mapcc) /
    //   set(&m_map118,&m_map130)。
    CRandomOptionGiveNormal m_providerNormal148;                      // +0x148
    CModifiedRandomOptionGive m_providerModified154;                  // +0x154
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
