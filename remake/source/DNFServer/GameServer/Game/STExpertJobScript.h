#ifndef DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_
#define DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_

// df_game_r STExpertJobScript —— 唯一声明点（CDataManagerScripts.cpp 实现；
// CUser::GetCurExpertJobLevel 真实调用；CExpertJobList.h 以 0x290 大小引用）。
//
// 完整布局由 ImportExpertJobScript 反汇编（docs/class_func_reports/
// _lt_global_gt_/ImportExpertJobScript.md，0x896b0e4）逐段恢复；总大小 0x290。
//
// 32 位基线：std::string=COW 4 字节，std::map=0x18，std::vector=0xc。
//
// ORIG 嵌套类型（Wave5 only_orig 清单实证，mangled _ZN17STExpertJobScript…W）：
// ctor/clear/dtor 与查询方法定义于 STExpertJobOnlyOrig.cpp（独占 TU）。

#include <map>
#include <string>
#include <vector>

// ---- 专家职业脚本子结构（ORIG 全局作用域：_ZI12ExpertJobExp 等）----

// 经验表项（0x0c 字节；[expertness exp]；vector 元素 stride i*12）。
struct ExpertJobExp
{
    int         m_exp;    // +0x00（GetLevel/isBoundaryExpValue 比较用）
    char        m_level;  // +0x04（ORIG C1ERKS_/aSERKS_ 实证 movzbl+mov %dl 单字节拷贝）
    char        m_pad5[3];// +0x05
    std::string m_name;   // +0x08（专家职业名）
};

// 物品选择率表项（0x14 字节）。get_additional_item（0x896d6ab）读 +0x0c/+0x10 作
// 概率区间 [min,max]、+0x04 为累加概率、+0x00 为 itemIndex（-1 哨兵）。
struct stItemSelectRate
{
    stItemSelectRate();  // ORIG _ZN16stItemSelectRateC1Ev W 0x085d3bd6（全字段清零）

    int   m_itemIndex;   // +0x00（-1 = 未选中哨兵）
    int   m_rate;        // +0x04（累加概率）
    float m_float8;      // +0x08
    int   m_rateMin;     // +0x0c（区间下限）
    int   m_rateMax;     // +0x10（区间上限）
};

// 分解机等级（[disjoint result] 键；3 字节）。
struct DisjointMachineGrade
{
    DisjointMachineGrade();  // ORIG _ZN20DisjointMachineGradeC1Ev W 0x0896dbcc（三字节清零）

    unsigned char m_grade;  // +0x00
    unsigned char m_field1; // +0x01
    unsigned char m_field2; // +0x02

    DisjointMachineGrade(unsigned char a, unsigned char b, unsigned char c);
    // ORIG _ZN20DisjointMachineGradeC1Ehhh W 0x085d3b9e：
    // m_grade=b、m_field1=a、m_field2=c（get_disjoint_result_ 调用点实证）。
    bool operator<(const DisjointMachineGrade& o) const
    {
        if (m_grade != o.m_grade) return m_grade < o.m_grade;
        if (m_field1 != o.m_field1) return m_field1 < o.m_field1;
        return m_field2 < o.m_field2;
    }
};

// 分解结果（[disjoint result] 值）。
struct stDisjointResult
{
    int                               m_field0;   // +0x00
    float                             m_field4;   // +0x04
    std::vector<stItemSelectRate>     m_additionalItems;  // +0x08
    std::vector<stItemSelectRate>     m_bigwinItems;      // +0x14
    int                               m_field20;  // +0x20（copy ctor 0x896f5aa 拷贝实证）
};

// 耐久修理费（[endurance repair cost] 元素；{int,int}；ORIG 全局 vector 元素 0x85d3f66）。
struct stEnduranceRepairCost
{
    int m_field0;  // +0x00
    int m_field4;  // +0x04
};

// 复合成功率表项（0x10 字节；GetCompoundRate 遍历）。
struct stCompoundRate
{
    int m_level;        // +0x00（阈值，与传入 level 比较 >=）
    int m_successRate;  // +0x04（基础成功率）
    int m_expMin;       // +0x08（成功后经验随机下限）
    int m_expMax;       // +0x0c（成功后经验随机上限）
};

// recipe 信息项（map<int,stRecipeInfo> 的 mapped type）。learnRecipe 0x0832b3c0
// 读 m_field8（+0x08）与 param+2 比较。m_field8 具体语义 ORIG 报告未证，
// 保持占位名。
struct stRecipeInfo
{
    char             m_pad[8];   // +0x00..+0x07
    unsigned short   m_field8;   // +0x08（学习门槛值，语义未证）
};

class STExpertJobScript
{
public:
    // ORIG C1/C2 @0x0896e5cc：全部成员容器构造后尾调 Clear()（定义见 CExpertJobList.cpp）。
    STExpertJobScript();
    // ---- ORIG 嵌套类型（均为 W 符号；定义见 STExpertJobOnlyOrig.cpp）----
    // 怪物卡绑定率（[monstercard bind info] 值）。
    struct stMonsterCardBindRate
    {
        stMonsterCardBindRate();   // ORIG C1/C2 @0x0822b1de（两 word 清零）
        unsigned short m_field0;  // +0x00
        unsigned short m_field2;  // +0x02
    };
    // 怪物卡绑定结果（stride 0xc）。ctor 0x0896dc20：只清 w+4 后 d+0。
    struct stMonsterCardBindResult
    {
        stMonsterCardBindResult();               // ORIG C1/C2 @0x0896dc20
        ~stMonsterCardBindResult();              // ORIG D1/D2 @0x08974fac（空体）
        int            m_field0;  // +0x00
        int            m_field4;  // +0x04
        unsigned short m_field8;  // +0x08
        unsigned char  m_fieldA;  // +0x0a
    };
    // 娃娃机结果（[doll_controller extraction result item] 元素）。
    // ctor 0x0896deac：w@+8=0 → w+6=w+8 → w+4=w+6 → d+0=(uint)(word)w+4（链式置零）。
    struct stDollControllerResult
    {
        stDollControllerResult();                // ORIG C1/C2 @0x0896deac
        ~stDollControllerResult();               // ORIG D1/D2 @0x089751fa（空体）
        int            m_field0;  // +0x00
        unsigned short m_field4;  // +0x04
        unsigned short m_field6;  // +0x06
        unsigned short m_field8;  // +0x08
        unsigned char  m_fieldA;  // +0x0a
    };
    // 自助分解材料信息（对象 +0x30 区；Clear 以其 clear() 调用实证）。sizeof 0xc。
    struct stSelfDisjoinMaterialInfo
    {
        stSelfDisjoinMaterialInfo();   // ORIG C1/C2 @0x0896dc0c（体仅尾调 clear）
        int m_field0;  // +0x00
        int m_field4;  // +0x04
        int m_field8;  // +0x08
        void clear();  // ORIG 0x0896dbe6：+8=0; +4=+8; +0=+4
    };
    // 绑定信息（GetCommonBindInfo→this+0x84 / GetSpecialBindInfo→this+0xc4 各一；
    // Clear 于 +0x84/+0xc4 各调一次 clear() 实证）。
    // ctor 0x0896dc80：构造 4×vector<stMonsterCardBindResult>@+0x10（mov $0x3 循环
    // 4 次实证）后调 clear；clear 0x0896dc38：i=0..3 { cnt[i]=0; vec[i].clear() }。
    // sizeof 0x40 = cnt[4]+vec[4]（步长 0xc；衔接下一 BindInfo @+0xc4，0xc4-0x84=0x40）。
    struct stMonsterCardBindInfo
    {
        stMonsterCardBindInfo();     // ORIG C1/C2 @0x0896dc80
        ~stMonsterCardBindInfo();    // ORIG D1/D2 @0x0896e218（析构 4 vector）
        void clear();                // ORIG 0x0896dc38
        int                                  m_count[4];  // +0x00..0x0f
        std::vector<stMonsterCardBindResult> m_vec[4];    // +0x10..0x3f
    };
    // 炼金提取结果信息（this+0x14c）。cnt[3]@0 + vec<BindResult>[3]@0xc。sizeof 0x30。
    struct stAlchemistExtractionResultInfo
    {
        stAlchemistExtractionResultInfo();   // ORIG C1/C2 @0x0896dd86
        ~stAlchemistExtractionResultInfo();  // ORIG D1/D2 @0x0896e250
        void clear();                        // ORIG 0x0896dd3e
        int                                  m_count[3];  // +0x00..0x08
        std::vector<stMonsterCardBindResult> m_vec[3];    // +0x0c..0x2f
    };                                   // sizeof 0x30
    // 附魔提取结果信息（this+0x17c）。cnt@0 + vec@4。sizeof 0x10。
    struct stEnchanterExtractionResultInfo
    {
        stEnchanterExtractionResultInfo();   // ORIG C1/C2 @0x0896de64
        ~stEnchanterExtractionResultInfo();  // ORIG D1/D2 @0x0896e288
        void clear();                        // ORIG 0x0896de44
        int                                  m_count;  // +0x00
        std::vector<stMonsterCardBindResult> m_vec;    // +0x04..0x0f
    };                                   // sizeof 0x10
    // 娃娃机提取结果信息（this+0x18c）。cnt[3]@0 + vec<DollResult>[3]@0xc。sizeof 0x30。
    struct stDollControllerResultInfo
    {
        stDollControllerResultInfo();        // ORIG C1/C2 @0x0896df2e
        ~stDollControllerResultInfo();       // ORIG D1/D2 @0x0896e29e
        void clear();                        // ORIG 0x0896dee6
        int                                 m_count[3];  // +0x00..0x08
        std::vector<stDollControllerResult> m_vec[3];    // +0x0c..0x2f
    };                                   // sizeof 0x30

    // ---- 方法 ----
    bool isBoundaryExpValue(int exp);
    int GetLevel(unsigned int exp);
    stCompoundRate* GetCompoundRate(int level);
    int GetRecipeInfoIndex(unsigned long itemIndex);
    stRecipeInfo* GetRecipeInfo(int index);
    void Clear();
    short GetShopDollSlot(int index);   // ORIG W 0x85cb306：m_shopDollMap 命中返值，否则 -1
    short GetPortableDisjointPossibleLevel(int level);  // ORIG W 0x0822b1f4

    // ---- worklist 查询方法（Wave5 only_orig；ORIG 地址标注）----
    int GetUpgradeCost(unsigned int index);                       // 0x085d3d3a
    stMonsterCardBindInfo* GetCommonBindInfo();                   // 0x0822b366：return this+0x84
    stMonsterCardBindInfo* GetSpecialBindInfo();                  // 0x0822b358：return this+0xc4
    bool GetMonsterCardBindRate(int key, stMonsterCardBindRate& out);  // 0x0822b2d0
    stEnduranceRepairCost* _GetEnduranceRepairCost(unsigned int grade);   // 0x085d3c26
    int GetDisjointMachineRepair(int grade, int endurance, int& outCost);  // 0x085d3cc8
    int GetDisjointMachineMaxGrade();                             // 0x085d3c0e：enduranceRepairCost.size()
    int GetDisjointMachineRepairCost(int grade, int count);       // 0x085d3c74
    int GetUnsealScrollPossibleLevel(int level);                  // 0x0822b262
    int GetAlchemistExtractionPossibleLevel(int level);           // 0x084a44b6
    int GetEnchanterExtractionPossibleLevel(int level);           // 0x084a452a
    int GetDollControllerExtractionPossibleLevel(int level);      // 0x084a459e
    stAlchemistExtractionResultInfo* GetAlchemistExtractionResultInfo();  // 0x084a4612
    stEnchanterExtractionResultInfo* GetEnchanterExtractionResultInfo();  // 0x084a462e
    stDollControllerResultInfo* GetDollControllerExtractionResultInfo();  // 0x084a4620

    // ---- 数据成员（偏移经 Import/Clear/查询 getter 反汇编逐段核实；总大小 0x290）----
    std::vector<ExpertJobExp>                   m_expVec;                 // +0x00  [expertness exp]
    std::vector<std::pair<int, int> >           m_growSkillVec;           // +0x0c  [skill]
    std::map<int, unsigned short>               m_portableDisjointMap;    // +0x18  [portable disjoint info]
    stSelfDisjoinMaterialInfo                   m_selfDisjoin;            // +0x30  [disjoint self service]
    std::map<int, unsigned char>                m_shopDollMap;            // +0x3c  [shop doll info]
    std::map<int, unsigned short>               m_unsealScrollMap;        // +0x54  [unseal scroll level limit info]
    std::map<int, stMonsterCardBindRate>        m_monsterCardBindRateMap; // +0x6c  [monstercard bind info]
    stMonsterCardBindInfo                       m_commonBindInfo;         // +0x84  [monstercard bind list]
    stMonsterCardBindInfo                       m_specialBindInfo;        // +0xc4  [monstercard bind only]
    std::map<int, unsigned short>               m_alchemistExtractInfo;   // +0x104 [alchemist extraction info]
    std::map<int, unsigned short>               m_enchanterExtractInfo;   // +0x11c [enchanter extraction info]
    std::map<int, unsigned short>               m_dollExtractInfo;        // +0x134 [doll_controller extraction info]
    stAlchemistExtractionResultInfo             m_alchemistResultInfo;    // +0x14c [alchemist extraction result item]
    stEnchanterExtractionResultInfo             m_enchanterResultInfo;    // +0x17c [enchanter extraction result item]
    stDollControllerResultInfo                  m_dollResultInfo;         // +0x18c [doll_controller extraction result item]
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

typedef STExpertJobScript::stMonsterCardBindRate   stMonsterCardBindRate;
typedef STExpertJobScript::stMonsterCardBindResult stMonsterCardBindResult;
typedef STExpertJobScript::stDollControllerResult  stDollControllerResult;

#endif  // DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_
