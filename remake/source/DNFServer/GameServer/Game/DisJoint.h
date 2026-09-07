// ============================================================================
// DisJoint.h — 分解（Disjoint）系统脚本宿主
//
// 依据 docs/class_func_reports/DisJoint/{README,LoadDisJoint}.md、
// docs/class_func_reports/_lt_global_gt_/importDisjointScript.md 与 ORIG 反汇编：
//   DisjointScript::DisjointScript   W 0x08375d4c（逐容器构造 + clear）
//   DisjointScript::clear            W 0x08375bca
//   DisjointScript::~DisjointScript  W 0x0837dff8
//   DisJoint::DisJoint               W 0x0837e532（基类 + 6×vector<int> @+0x628）
//   DisJoint::LoadDisJoint           T 0x0847300c
// 布局偏移全部来自 ctor/clear/importDisjointScript/LoadDisJoint 汇编实证。
// 此前 CDataManager.cpp 内的 m_pad[0x628] 镜像由本头取代（专属批次 Ctor_Group2）。
// ============================================================================
#ifndef GAME_DISJOINT_H_
#define GAME_DISJOINT_H_

#include <vector>

class CItem;
// ---- DisJointResult（ORIG W ctor 0x082347a2：10×int 清零环，sizeof 0x28）----
// 字段写入点实证（GetResult 0x084733f6 / _GetAdditionalItem 0x084732b4）：
//   m_f[0]=+0x00 分解金钱；m_f[1]=+0x04 附加物数量；
//   m_f[5]=+0x14 cube 索引（m_cubeIndexVecs[4][0]）；
//   m_f[6]=+0x18 附加物 item idx；其余槽位仅由 ctor 清零。
struct DisJointResult
{
    DisJointResult();  // ORIG _ZN14DisJointResultC1Ev W 0x082347a2

    int m_f[10];       // +0x00..+0x24（10 int）
};  // sizeof == 0x28

// ---- LotteryInfo：唯一权威声明见 LotteryInfo.h（原本地副本删除，布局一致；
//      方法定义归属 CStackableItem.cpp）----
#include "LotteryInfo.h"

// ---- DisjointScript::._381（匿名结构；ctor 0x08348088 仅构造 +4 vector<int>；
//      GCC 匿名嵌套命名，定义在 DisjointScript 类内 m_additional 处）----

// ---- DisjointSkillScript（[skill info] 行元素；ctor W 0x084735fe 仅调 clear；
//      clear W 0x084735d2 置 {0xb,0,0,4}；dtor W 0x088e5b32 平凡）----
struct DisjointSkillScript
{
    DisjointSkillScript();   // ORIG _ZN19DisjointSkillScriptC1Ev W 0x084735fe
    void clear();            // ORIG _ZN19DisjointSkillScript5clearEv W 0x084735d2

    int m_field00;   // +0x00（clear 置 0xb）
    int m_field04;   // +0x04（clear 置 0）
    int m_field08;   // +0x08（clear 置 0）
    int m_field0c;   // +0x0c（clear 置 4）
};  // sizeof == 0x10（4×int；方法定义：CtorProducers_2.cpp）

class DisjointScript
{
public:
    DisjointScript();                   // ORIG W 0x08375d4c
    ~DisjointScript();                  // ORIG W 0x0837dff8
    void clear();                       // ORIG W 0x08375bca

    // ---- 布局（ctor/clear/importDisjointScript 实证）----
    std::vector<int> m_cubeIndexVecs[5];      // +0x00（[cube index]，步长 0xc）
    LotteryInfo m_legacyInfo[4];              // +0x3c（[legacy info]，步长 0x20）
    int m_skillSelect;                        // +0xbc（clear=-1；[skill info] 首值）
    std::vector<DisjointSkillScript> m_skills;  // +0xc0（[skill info] 行集）
    float m_fieldCC;                          // +0xcc（[skill prob] 第 1 浮点；clear 置 0）
    float m_skillProbD0;                      // +0xd0（[skill prob] 第 2 浮点；clear 置 0）
    char m_padD4[0xe0 - 0xd4];                // +0xd4..0xdf（未建模，clear/import 均不触）
    int m_avatarDisjoint[11][4][5];           // +0xe0（[avatar disjoint info]
                                              //      [job 0..10][grade 0..3][slot 0..4]，
                                              //      导入使用 grade 1..3，clear 全清零）
    int m_avatarPartsDisjoint[3][10];         // +0x450（[avatar disjoint for parts info]
                                              //      [grade-1][parts 0..9]）→ 止于 0x4c8
    char m_pad4c8[0x534 - 0x4c8];             // +0x4c8..0x533（未建模）
    int m_creationConst;                      // +0x534（[cube creation const] 首值）
    float m_creationRate[6];                  // +0x538..0x54f（[cube creation const] 概率）
    // [additional result const] 区（importDisjointScript 0x088e4d88 写入实证）：
    float m_additionalWeight[6];              // +0x550（组 i 首浮点；非 0 时启用该组）
    char m_pad568[0x56c - 0x568];             // +0x568..0x56b（ORIG 空洞）
    struct AdditionalConstEntry
    {
        int m_item;                           // +0x00
        int m_count;                          // +0x04
        int m_pad8;
    };  // sizeof == 0xc
    // ORIG 布局：const 表 @+0x56c（6 行×0xc）止于 0x5b4，而 m_additionalRate 表
    // @+0x5b0 与第 6 行 m_pad8（0x5b0..0x5b4）重叠。标准 C++ 无法线性表达重叠区，
    // 故用匿名 union：m_additionalConst[6]（+0x56c 步长 0xc）与
    // { pad[0x44], m_additionalRate[6] @+0x5b0 } 共享同一 0x5c 字节区（ORIG 布局实证）。
    union
    {
        AdditionalConstEntry m_additionalConst[6];
        struct
        {
            char m_pad44[0x44];               // +0x56c..0x5b0（const 表 m_item/m_count 区）
            float m_additionalRate[6];        // +0x5b0（组 i 第二浮点）→ 止于 0x5c8
        };
    };
    // [additional result] 组元素。ORIG 为匿名嵌套结构 → GCC 自动命名
    // DisjointScript::._381（符号 _ZN14DisjointScript5._381C1Ev t 0x08348088）。
    // GCC 无法声明 `class ._381`；命名类 `_381` 自然 mangled `4_381` 缺点号不匹配。
    // asm 标签对 ctor/dtor 无效（GCC 硬限制），故 ctor/dtor 在 CtorProducers_2.cpp
    // 显式定义 + `.set` 汇编别名强制 ORIG 带点号符号（编译器 mangling 必要性，非 §4.5 违例）。
    class _381
    {
    public:
        _381();    // ORIG _ZN14DisjointScript5._381C1Ev t 0x08348088（仅构造 +0x04 vector<int>）
        ~_381();   // ORIG _ZN14DisjointScript5._381D1Ev t 0x0834809e（逆序析构）
        int m_count;               // +0x00（[additional result] 组计数）
        std::vector<int> m_items;  // +0x04
    } m_additional[6];             // +0x5c8（[additional result]，步长 0x10
                                   //  （int 对齐 4，vector 迭代器 3×4=12；sizeof 0x10）
};  // sizeof == 0x628

class DisJoint : public DisjointScript
{
public:
    DisJoint();                                // ORIG W _ZN8DisJointC1Ev 0x0837e532
    ~DisJoint();                               // 定义由原 CDataManager 镜像迁移至此
    bool LoadDisJoint();                       // ORIG T _ZN8DisJoint12LoadDisJointEv 0x0847300c

    // 其余查询方法（GetPredictResult/GetResult 等）报告在案，定义待分解域批次。

    // ORIG T 0x08473116：rarity 表查找，越界回第 0 项。
    std::vector<int>* GetPredictResult(int rarity);
    int getPredictResultItemTypeCount(int rarity) const;  // ORIG T 0x08473148

    // ORIG T 0x08473184 _ZN8DisJoint17GetAddtionalSkillEiiRiS0_：
    // [skill info] 行查表（定义 DisJoint.cpp）
    void GetAddtionalSkill(int job, int level, int& outSkill, int& outLevel);

    // ORIG ctor 0x0837e532：基类 0x628 后 6×vector<int> 结果表
    std::vector<int> m_resultVecs[6];           // +0x628 → 止于 0x670？报告 0x658【推断】

    // ---- 分解结果（定义 DisJoint.cpp）----
    int GetResult(int itemIdx, DisJointResult& out);            // T 0x084733f6
    void _GetAdditionalItem(const CItem* item, int& outIdx,
                            int& outCount);                     // T 0x084732b4
    int _GetAdditionalItemInfo(int rarity);                     // T 0x08473224
};  // sizeof == 0x658

bool importDisjointScript(DisjointScript* script, const char* path);  // ORIG T 0x088e4d88（声明；定义待脚本导入域批次）

#endif  // GAME_DISJOINT_H_
