// ============================================================================
// StrikerScript.h — striker.etc 脚本宿主（ORIG sizeof 0xb4）
//
// 依据 docs/class_func_reports/StrikerScript/{getInsance,importScript,
// importSkillDataUpVector,importSkillAttackPenalty,importSkillCoolTimePenalty,
// importParentLevelPenlaty,readSkillPenalty}.md 与 ORIG 反汇编：
//   StrikerScript::StrikerScript   T 0x08a9d88e（逐容器构造 + clear + create）
//   StrikerScript::~StrikerScript  T 0x08a9da28（destory + 逆序容器析构）
//   getInsance                     T 0x08a9d836（new 0xb4 单例 instance_）
//   importScript                   T 0x08a9ddb2
// 成员偏移全部来自 ctor/clear/importScript 汇编实证。
// 此前 CDataManager.cpp 内的最小镜像由本头取代（专属批次 Ctor_Group2）。
// ============================================================================
#ifndef GAME_STRIKER_SCRIPT_H_
#define GAME_STRIKER_SCRIPT_H_

#include <map>
#include <string>
#include <vector>

class STScriptFileList;  // ORIG 全局类（NpcScript.h 镜像 / 各 TU 本地模型）

// ---- ContentsType（ORIG 全局类；getString T 0x08ac3434 / getType T 0x08ac34a4，
//      定义见 EventRewardQuickPartyB2.cpp）----
struct ContentsType
{
    enum T
    {
        T_0 = 0
    };
    static const char* getString(T type);   // T 0x08ac3434
    static int getType(const char* name);   // T 0x08ac34a4
};

// ---- ContentsRevision（[contents revision] 行；ORIG 栈对象 16 字节）----
struct ContentsRevision
{
    int m_type;     // +0x00（ContentsType::getType）
    int m_field04;  // +0x04
    int m_field08;  // +0x08
    int m_field0c;  // +0x0c
};  // 0x10

// ---- StrikerSkill（[striker skill] 行；ctor 0x08a9ed1e 仅构造 +0xc string）----
struct StrikerSkill
{
    StrikerSkill();                        // ORIG _ZN12StrikerSkillC1Ev W
    int m_field00;      // +0x00
    int m_field04;      // +0x04
    int m_field08;      // +0x08
    std::string m_name; // +0x0c
};  // 0x10

// ---- StrikerSkillDataUp（[striker skill data up] 行，0x14）----
struct EquipmentSkillDataUp;  // 元素类型定义待装备参数域批次
struct StrikerSkillDataUp
{
    int m_level;                                  // +0x00
    int m_field04;                                // +0x04
    std::vector<EquipmentSkillDataUp> m_dataUps;  // +0x08（importSkillDataUp 填充）
};  // 0x14

// ---- TagSkillPenalty（readSkillPenalty 行；栈对象 0x10）----
struct TagSkillPenalty
{
    // [GI 调序 T1a] ORIG C1/C2 为**外联** T 0x08a9d768（仅 call clear，体 0x13）。
    // 头内 inline 只发 W 弱副本 ⇒ 按 P327「类内 inline ctor 不参与键名」，
    // 宿主 TU 的 GI 键名永远无法对齐 `_GLOBAL__I__ZN15TagSkillPenaltyC2Ev`（ORIG 0x08a9eced）；
    // 4.4-O0 不内联本调用（我方原 W 副本实测同为 0x13）。故改声明，定义落
    // StrikerWorldDropB2.cpp（= ORIG 该 CU 的对应 TU）。sizeof/偏移/mangled 零变化。
    TagSkillPenalty();  // ORIG T 0x08a9d768（定义 StrikerWorldDropB2.cpp）
    void clear()                    // ORIG T 0x08a9d77c：+0=0、+4=0、+8 字节 0、+0xc=0
    {
        m_skillIdx = 0;
        m_value = 0;
        m_isPercent = false;        // movb $0x0,0x8
        m_pad = 0;
    }
    bool isPenaltyLevel(int level);  // ORIG T 0x08a9d7a6（定义 OnlyOrigBatch11.cpp）
    int m_skillIdx;   // +0x00
    int m_value;      // +0x04
    bool m_isPercent; // +0x08（与 "%" 比较）
    int m_pad;        // +0x0c（对齐保留）
};  // 0x10

// ---- ParentLevelPenalty（[striker level penalty] 行；2×int+2×float）----
struct ParentLevelPenalty
{
    int m_field00;    // +0x00
    int m_field04;    // +0x04
    float m_rate08;   // +0x08
    float m_rate0c;   // +0x0c
};  // 0x10

// ---- StrikerSkillAddCommand（[striker skill macro] 行；setType 0x08a9d7cc）----
struct StrikerSkillAddCommand
{
    int m_field00;  // +0x00
    int m_field04;  // +0x04
    // [b28d-34] P211 异型化：ORIG setType 0x08a9d7cc 的 store 为 RHS-first
    // （`mov 0xc(%ebp),%edx; mov 0x8(%ebp),%eax; mov %edx,0x8(%eax)`）；字段 int
    // 与形参 int 同型 ⇒ 4.4 LHS-first，改 unsigned int（与形参异型）即翻正，
    // 无中转（探针 /tmp/p34b.cpp A/B 对照，sizeof/偏移/mangled 不变）。
    unsigned int m_type;  // +0x08（setType: 0<=v<=8 写入）
    void setType(int type)
    {
        // [b28c-30] P223 多源尾标签形：ORIG 0x08a9d7cc 为
        // `cmpl $0; js +0x1a` / `cmpl $8; jg +0x1d` / body / `jmp +0x1e`
        // + 两个 nop 岛（+0x1a 的 nop;jmp、+0x1d 的 nop）；短路 `&&` 形产
        // 单分支+bool 物化（11 insn），多源 goto 连续尾标签复现双岛（15 insn）。
        if (type < 0) goto L_end;
        if (type > 8) goto L_end;
        m_type = type;
    L_end:
        ;
    }
};  // 0xc

// ---- StrikerDirectionType（ctor 0x08a9d7ec / setType 0x08a9d7fa）----
struct StrikerDirectionType
{
    enum T { T_0 = 0, T_1 = 1 };      // ORIG mangled NS_1TE（嵌套枚举 T）
    StrikerDirectionType() : m_type(T_0) {}
    void setType(int type)
    {
        // b28c-L19：ORIG 0x08a9d7fa 双 nop 岛（js +0x19 / jg +0x1c / body 尾 jmp），
        // P223 goto 尾空标签形精确复现（探针 /tmp/l19_probe6.cpp 逐条全等）。
        if (type < 0) goto L_neg;
        if (type > 1) goto L_pos;
        m_type = (T)type;
        goto L_end;
    L_neg:
        ;
        goto L_end;
    L_pos:
        ;
    L_end:
        ;
    }
    bool isType(T type);           // ORIG T 0x08a9d81a（定义 OnlyOrigBatch11.cpp）
    // b28d-D66：ORIG isType 0x08a9d81a 为 `mov (%eax),%eax; cmp 0xc(%ebp),%eax`
    // （同型 enum==enum 的 mem 直比形，P209 的 unsigned 形参寄存器装载被消）⇒ 字段真型
    // 是 enum T 而非 unsigned int；探针 /tmp/d66_probe4.cpp A/B 双结构实证：字段 enum 时
    // isType 逐条等于 ORIG，且 setType（含 nop 岛与 RHS-first store 序）与 unsigned 版
    // 逐字节相同。sizeof/偏移/mangled 零影响。
    T m_type;  // +0x00
};  // 0x4

class StrikerScript
{
public:
    static StrikerScript* getInsance();          // ORIG _ZN13StrikerScript10getInsanceEv
    static StrikerScript* instance_;             // ORIG _ZN13StrikerScript9instance_E B 0x09500978

    StrikerScript();                             // ORIG T 0x08a9d88e
    ~StrikerScript();                            // ORIG T 0x08a9da28

    void clear();                                // ORIG T 0x08a9dbb4
    void create();                               // ORIG T 0x08a9dc70
    void destory();                              // ORIG T 0x08a9dcbe
    bool importScript(const char* path);         // ORIG T 0x08a9ddb2

    void importSkillDataUpVector();              // ORIG T 0x08a9e4d6
    void importSkillAttackPenalty();             // ORIG T 0x08a9e5d0
    void importSkillCoolTimePenalty();           // ORIG T 0x08a9e5fc
    void importParentLevelPenlaty();             // ORIG T 0x08a9e654
    void readSkillPenalty(std::vector<TagSkillPenalty>& out);  // ORIG T 0x08a9e6cc
    bool importComboList();                    // ORIG T 0x08a9e79c
    void importStrikerLevelPenalty();          // ORIG T 0x08a9e628
    int getPrecedeSkillIndex(short firstGrowType, short secondGrowType,
                             int skillIndex);  // ORIG T 0x08a9ea96
    int getComboIndex(short firstGrowType, short secondGrowType,
                      int skillIndex);         // ORIG T 0x08a9e97a
    int getAddCommand(short firstGrowType, int command);   // ORIG T 0x08a9eb3a
    int isCommonSkill(short firstGrowType, int skillIndex);  // ORIG T 0x08a9ebee

    // ---- 布局（ctor 0x08a9d88e / clear 0x08a9dbb4 实证）----
    int m_tagCostTime;                           // +0x00（[tag cost time]）
    int m_tagCharacterSetting;                   // +0x04（[tag character setting]）
    int m_strikerCombo;                          // +0x08（[striker combo]）
    int m_strikerStartTime;                      // +0x0c（[striker start time]）
    int m_strikerEndTime;                        // +0x10（clear=500；[striker end time]）
    std::vector<ContentsType::T> m_moduleImpossible;   // +0x14
    std::vector<int> m_dungeonImpossible;              // +0x20
    std::vector<ContentsRevision> m_contentsRevision;  // +0x2c
    int m_field38;                               // +0x38（[position] 第 2 值）
    int m_field3c;                               // +0x3c（[position] 第 3 值）
    std::vector<StrikerSkillDataUp> m_skillDataUp;     // +0x40
    std::vector<TagSkillPenalty> m_attackPenalty;      // +0x4c
    std::vector<TagSkillPenalty> m_coolTimePenalty;    // +0x58
    int m_field64;                               // +0x64（clear 置 0）
    int m_field68;                               // +0x68（clear 置 0）
    std::vector<ParentLevelPenalty> m_parentLevelPenalty;   // +0x6c（ctor 0x08a9d88e
                                                            //  构造 vector@+0x6c；
                                                            //  importParentLevelPenlaty
                                                            //  0x08a9e654 clear/push_back 实证）
    std::map<int, std::vector<StrikerSkill>*> m_skillMap;  // +0x78
    // [SweepCtorProd2] void* → STScriptFileList*（P301）：ORIG destory 0x08a9dcdf
    // `mov 0x90(%eax),%ebx` 单段直落；void* + reinterpret_cast 使 4.4 多一条
    // `mov %eax,%ebx` 中转。sizeof/偏移/mangled 零变化。
    STScriptFileList* m_fileList;                // +0x90（create/destory 管理）
    std::vector<StrikerSkillAddCommand> m_macroCommands;   // +0x94（[striker skill macro]）
    unsigned char m_loaded;                      // +0xa0（importScript 成功置 1）
    char m_padA1[3];
    int m_commonCoolTime;                        // +0xa4（[common cool time]）
    StrikerDirectionType m_direction;            // +0xa8
    int m_positionType;                          // +0xac（[position] 第 1 值）
    int m_upgradeRevision;                       // +0xb0（clear=99；[upgrade revision]）

    // ORIG W _ZN13StrikerScript21getStrikerSkillVectorEss（实现待 striker 域批次；
    // checkSkillIndex 消费。LinkClosureC 批次按 ORIG mangled 补声明。）
    std::vector<StrikerSkill>* getStrikerSkillVector(short firstGrowType,
                                                     short secondGrowType);
    bool checkSkillIndex(short firstGrowType, short secondGrowType,
                         int skillIndex);
                                                           // ORIG T _ZN13StrikerScript15checkSkillIndexEssi
                                                           //   @0x08a9e868（ValidateStrikerInfo 消费；
                                                           //   实现 owner＝striker 脚本域批次）
};  // sizeof == 0xb4

#endif  // GAME_STRIKER_SCRIPT_H_
