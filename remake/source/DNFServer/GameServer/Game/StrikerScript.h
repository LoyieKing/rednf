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
    TagSkillPenalty() { clear(); }  // ORIG C1/C2 T 0x08a9d768：仅调 clear()
    void clear()                    // ORIG T 0x08a9d77c：+0=0、+4=0、+8 字节 0、+0xc=0
    {
        m_skillIdx = 0;
        m_value = 0;
        m_isPercent = false;        // movb $0x0,0x8
        m_pad = 0;
    }
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
    int m_type;     // +0x08（setType: 0<=v<=8 写入）
    void setType(int type)
    {
        if (type >= 0 && type <= 8)
        {
            m_type = type;
        }
    }
};  // 0xc

// ---- StrikerDirectionType（ctor 0x08a9d7ec / setType 0x08a9d7fa）----
struct StrikerDirectionType
{
    StrikerDirectionType() : m_type(0) {}
    void setType(int type)
    {
        if (type >= 0 && type <= 1)
        {
            m_type = type;
        }
    }
    int m_type;  // +0x00
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
    void* m_fileList;                            // +0x90（create/destory 管理；ORIG STScriptFileList*）
    std::vector<StrikerSkillAddCommand> m_macroCommands;   // +0x94（[striker skill macro]）
    unsigned char m_loaded;                      // +0xa0（importScript 成功置 1）
    char m_padA1[3];
    int m_commonCoolTime;                        // +0xa4（[common cool time]）
    StrikerDirectionType m_direction;            // +0xa8
    int m_positionType;                          // +0xac（[position] 第 1 值）
    int m_upgradeRevision;                       // +0xb0（clear=99；[upgrade revision]）
};  // sizeof == 0xb4

#endif  // GAME_STRIKER_SCRIPT_H_
