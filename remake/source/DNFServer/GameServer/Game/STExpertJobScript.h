#ifndef DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_
#define DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_

// df_game_r STExpertJobScript —— 唯一声明点（CDataManagerScripts.cpp 实现；
// CUser::GetCurExpertJobLevel 真实调用；CExpertJobList.h 以 0x290 大小引用）。
//
// ORIG 符号（nm -S --defined-only，均为 W）：
//   _ZN17STExpertJobScript18isBoundaryExpValueEi  0x0849f420
//   _ZN17STExpertJobScript8GetLevelEj             0x08693ba0
//   _ZN17STExpertJobScript15GetCompoundRateEi     0x0849f366
//   _ZN17STExpertJobScript18GetRecipeInfoIndexEm  0x0832b726
//   _ZN17STExpertJobScript13GetRecipeInfoEi       0x0832b6b6
//
// 完整布局由 ImportExpertJobScript 反汇编（docs/class_func_reports/
// _lt_global_gt_/ImportExpertJobScript.md，0x896b0e4）逐段恢复：函数内
// 全部访问形如 `param_1 + local_58*0x290 + X` 且 local_58 恒为 0，故有效
// 偏移即 X。总大小 0x290，各容器/标量偏移与报告一一对应（连续、无 pad）。
//
// 32 位基线：std::string=COW 4 字节，std::map=0x18，std::vector=0xc。

#include <map>
#include <string>
#include <vector>

// ---- 专家职业脚本子结构（按 ImportExpertJobScript 反汇编字段）----

// 经验表项（0x0c 字节；[expertness exp]；vector 元素 stride i*12）。
// 读入 (int exp, int level, string name)；name 为 COW std::string（4 字节）。
struct ExpertJobExp
{
    int         m_exp;    // +0x00（GetLevel/isBoundaryExpValue 比较用）
    int         m_level;  // +0x04
    std::string m_name;   // +0x08（专家职业名）
};

// 物品选择率表项（0x14 字节；[additional result]/[big win result] 的
// multimap<int,stItemSelectRate> 值，也是 [disjoint result] 附加/大奖向量
// 元素）。get_additional_item（0x896d6ab）读 +0x0c/+0x10 作概率区间
// [min,max]、+0x04 为累加概率、+0x00 为 itemIndex（-1 哨兵）。
struct stItemSelectRate
{
    int   m_itemIndex;   // +0x00（-1 = 未选中哨兵）
    int   m_rate;        // +0x04（累加概率）
    float m_float8;      // +0x08
    int   m_rateMin;     // +0x0c（区间下限）
    int   m_rateMax;     // +0x10（区间上限）
};

// 分解机等级（[disjoint result] 键；3 字节，map 键需 operator<）。
// ORIG _ZN20DisjointMachineGradeC1Ehhh / _ZNK20DisjointMachineGradeltERKS_。
struct DisjointMachineGrade
{
    unsigned char m_grade;  // +0x00
    unsigned char m_field1; // +0x01
    unsigned char m_field2; // +0x02

    bool operator<(const DisjointMachineGrade& o) const
    {
        if (m_grade != o.m_grade) return m_grade < o.m_grade;
        if (m_field1 != o.m_field1) return m_field1 < o.m_field1;
        return m_field2 < o.m_field2;
    }
};

// 分解结果（[disjoint result] 值）。读 (int,float) 并存入附加/大奖物品向量
// （get_disjoint_additional_items 填充）。CDisjointer::get_disjoint_result
// 读值 +4 起的 +8/+0x14 两向量。
struct stDisjointResult
{
    int                               m_field0;   // +0x00
    float                             m_field4;   // +0x04
    std::vector<stItemSelectRate>     m_additionalItems;  // +0x08
    std::vector<stItemSelectRate>     m_bigwinItems;      // +0x14
};

// 怪物卡绑定率（[monstercard bind info] 值；2 个 ushort）。
struct stMonsterCardBindRate
{
    unsigned short m_field0;  // +0x00
    unsigned short m_field2;  // +0x02
};

// 怪物卡绑定结果（[monstercard bind list]/[bind only]/[alchemist|enchanter
// extraction result item] 元素；stride 0xc）。读 (int, ushort[, ushort])。
struct stMonsterCardBindResult
{
    int            m_field0;  // +0x00
    int            m_field4;  // +0x04
    unsigned short m_field8;  // +0x08
    unsigned char  m_fieldA;  // +0x0a
};

// 娃娃机结果（[doll_controller extraction result item] 元素；stride 0xc）。
// 读 (int, ushort, ushort, ushort)。
struct stDollControllerResult
{
    int            m_field0;  // +0x00
    unsigned short m_field4;  // +0x04
    unsigned short m_field6;  // +0x06
    unsigned short m_field8;  // +0x08
    unsigned char  m_fieldA;  // +0x0a
};

// 耐久修理费（[endurance repair cost] 元素；{int,int}）。
struct stEnduranceRepairCost
{
    int m_field0;  // +0x00
    int m_field4;  // +0x04
};

// 复合成功率表项（0x10 字节；[compound rate]；GetCompoundRate 遍历，
// 按 m_level 阈值分段）。
struct stCompoundRate
{
    int m_level;        // +0x00（阈值，与传入 level 比较 >=）
    int m_successRate;  // +0x04（基础成功率，IsSuccess 中 + variation）
    int m_expMin;       // +0x08（成功后经验随机下限）
    int m_expMax;       // +0x0c（成功后经验随机上限）
};

// recipe 信息项（map<int,stRecipeInfo> 的 mapped type）。learnRecipe 读取
// m_field8（+0x08）与 param+2 比较。填充值见 ImportExpertJobScript [items]。
struct stRecipeInfo
{
    char             m_pad[8];   // +0x00..+0x07
    unsigned short   m_field8;   // +0x08
};

class STExpertJobScript
{
public:
    bool isBoundaryExpValue(int exp);
    int GetLevel(unsigned int exp);
    stCompoundRate* GetCompoundRate(int level);
    int GetRecipeInfoIndex(unsigned long itemIndex);
    stRecipeInfo* GetRecipeInfo(int index);
    void Clear();
    short GetShopDollSlot(int index);   // ORIG W 0x85cb306：m_shopDollMap 命中返值，否则 -1

    std::vector<ExpertJobExp>                   m_expVec;                 // +0x00  [expertness exp]
    std::vector<std::pair<int, int> >           m_growSkillVec;           // +0x0c  [skill]
    std::map<int, unsigned short>               m_portableDisjointMap;    // +0x18  [portable disjoint info]
    int                                         m_disjointSelfService[3]; // +0x30  [disjoint self service]
    std::map<int, unsigned char>                m_shopDollMap;            // +0x3c  [shop doll info]
    std::map<int, unsigned short>               m_unsealScrollMap;        // +0x54  [unseal scroll level limit info]
    std::map<int, stMonsterCardBindRate>        m_monsterCardBindRateMap; // +0x6c  [monstercard bind info]
    int                                         m_bindListCnt[4];         // +0x84  [monstercard bind list] 计数
    std::vector<stMonsterCardBindResult>        m_bindList[4];            // +0x94  [monstercard bind list]
    int                                         m_bindOnlyCnt[4];         // +0xc4  [monstercard bind only] 计数
    std::vector<stMonsterCardBindResult>        m_bindOnly[4];            // +0xd4  [monstercard bind only]
    std::map<int, unsigned short>               m_alchemistExtractInfo;   // +0x104 [alchemist extraction info]
    std::map<int, unsigned short>               m_enchanterExtractInfo;   // +0x11c [enchanter extraction info]
    std::map<int, unsigned short>               m_dollExtractInfo;        // +0x134 [doll_controller extraction info]
    char                                        m_pad14c[0xc];            // +0x14c（未建模容器/字段）
    std::vector<stMonsterCardBindResult>        m_alchemistResult[3];     // +0x158 [alchemist extraction result item]
    int                                         m_enchanterResultCnt;     // +0x17c [enchanter extraction result item] 计数
    std::vector<stMonsterCardBindResult>        m_enchanterResult;        // +0x180 [enchanter extraction result item]
    int                                         m_extractResultCnt[3];    // +0x18c alchemist/doll 提取结果计数（共享）
    std::vector<stDollControllerResult>         m_dollResult[3];          // +0x198 [doll_controller extraction result item]
    std::map<DisjointMachineGrade, stDisjointResult> m_disjointResultMap; // +0x1bc [disjoint result]
    int                                         m_baseConst;              // +0x1d4 [base const]
    int                                         m_additionalConst;        // +0x1d8 [additional const]
    std::vector<int>                            m_clearQuests;            // +0x1dc [clear quests]
    std::vector<int>                            m_giveupCost;             // +0x1e8 [giveup cost]
    std::vector<stCompoundRate>                 m_compoundRateVec;        // +0x1f4 [compound rate]
    std::map<int, stRecipeInfo>                 m_recipeInfoMap;          // +0x200 [items] GetRecipeInfo
    std::map<unsigned long, int>                m_recipeIndexMap;         // +0x218 [items] GetRecipeInfoIndex
    std::map<int, int>                          m_recipeLevelMap;         // +0x230 [items] itemIndex→所需等级
    std::map<unsigned long, unsigned long>      m_recipeItemMap;          // +0x248 [items]
    std::vector<stEnduranceRepairCost>          m_enduranceRepairCost;    // +0x260 [endurance repair cost]
    int                                         m_enduranceReduce1;       // +0x26c [endurance reduce]
    int                                         m_enduranceReduce2;       // +0x270 [endurance reduce]
    int                                         m_enduranceInit;          // +0x274 [endurance initial value]
    std::vector<int>                            m_upgradeCost;            // +0x278 [upgrade cost]
    int                                         m_gainExp1;               // +0x284 [gain exp]
    int                                         m_gainExp2;               // +0x288 [gain exp]
    int                                         m_limitDisjointGold;      // +0x28c [limit disjoint gold]
};

#endif  // DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_
