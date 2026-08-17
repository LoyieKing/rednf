#ifndef GAME_STSKILLSCRIPT_H_
#define GAME_STSKILLSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STSkillScript（G5 数据/脚本域，尺寸 0x2ac）
// 布局依据 ORIG Clear（0x083750b4）/ 析构（0x0837a0fa）逐成员推导。
// SkillUseItem / SkillLevelFeature / STPassiveFeatureData / STKeyCommand
// 为 ORIG 独立类（weak inline），同一 TU 实现。
// ============================================================================

#include <string>
#include <utility>
#include <vector>

// ---- 键命令单元（0x18 字节；与 CSkill.h 同布局，避免重复声明冲突） ----
#ifndef DNF_GAME_CSKILL_H_
struct STKeyCommandUnit
{
    char m_pad[0x18];
};
#endif

// ---- 技能等级特征（0x24 字节；Clear 0x08a9b164）----
class SkillLevelFeature
{
public:
    SkillLevelFeature();
    SkillLevelFeature(const SkillLevelFeature& other);
    ~SkillLevelFeature();
    SkillLevelFeature& operator=(const SkillLevelFeature& other);
    void clear();

    int m_field0[2];                  // +0x00
    std::string m_str8;               // +0x08
    std::vector<std::pair<int, int> > m_vecC;  // +0x0c
    std::vector<float> m_vec18;       // +0x18
};

// ---- 技能使用物品（0x0c 字节；Clear 0x08a9b118）----
class SkillUseItem
{
public:
    SkillUseItem();
    ~SkillUseItem();
    void clear();

    int m_index;      // +0x00（-1）
    int m_useLevel[2];// +0x04
};

// ---- 被动技能特征数据（0x10 字节；Clear 0x08a9b256）----
class STPassiveFeatureData
{
public:
    STPassiveFeatureData();
    ~STPassiveFeatureData();
    void clear();

    char m_enable;   // +0x00（1）
    int m_4;         // +0x04
    int m_8;         // +0x08
    int m_c;         // +0x0c
};

// ---- 技能键命令（0x24 字节；Clear 0x083660ec）----
class STKeyCommand
{
public:
    STKeyCommand();
    ~STKeyCommand();
    void clear();

    std::vector<STKeyCommandUnit> m_vec0;  // +0x00
    std::vector<int> m_vecC;               // +0x0c
    int m_18;                              // +0x18（1）
    int m_1c;                              // +0x1c
    char m_20;                             // +0x20（1）
};

class STSkillScript
{
public:
    STSkillScript();
    ~STSkillScript();
    void Clear();

    int m_field0;                         // +0x00
    std::string m_str4;                   // +0x04
    std::string m_str8;                   // +0x08
    std::string m_strc;                   // +0x0c
    std::string m_str10;                  // +0x10
    int m_14;                             // +0x14
    std::vector<int> m_vec18;             // +0x18
    std::vector<int> m_vec24;             // +0x24
    int m_30;                             // +0x30（1）
    int m_34;                             // +0x34（1）
    int m_38;                             // +0x38
    int m_3c;                             // +0x3c（1）
    int m_40;                             // +0x40（1）
    std::vector<int> m_vec44;             // +0x44
    std::vector<int> m_vec50;             // +0x50
    int m_5c;                             // +0x5c
    int m_60;                             // +0x60
    int m_arr64[6];                       // +0x64
    int m_arr7c[6];                       // +0x7c
    struct StGrowType
    {
        int m_0[2];   // +0x00
        int m_8[2];   // +0x08
    } m_growType[6];                      // +0x94（16 字节/个）
    std::vector<int> m_vecf4;             // +0xf4
    int m_arr100[6];                      // +0x100
    int m_arr138[2];                      // +0x138
    int m_118;                            // +0x118
    std::vector<SkillUseItem> m_useItems11c;  // +0x11c
    std::vector<int> m_vec128;            // +0x128
    int m_134;                            // +0x134（1）
    std::vector<int> m_vec140;            // +0x140
    std::vector<int> m_vec14c;            // +0x14c
    int m_158;                            // +0x158（1）
    int m_15c;                            // +0x15c
    std::string m_str160[2];              // +0x160
    int m_arr168[2];                      // +0x168
    STKeyCommand m_keyCommand170;         // +0x170
    std::string m_str194;                 // +0x194
    std::vector<int> m_vec198;            // +0x198
    int m_1a4;                            // +0x1a4（-1）
    std::vector<std::vector<float> > m_vec1a8;  // +0x1a8
    std::vector<SkillLevelFeature> m_levelFeatures1b4;  // +0x1b4
    std::vector<SkillLevelFeature> m_levelFeatures1c0;  // +0x1c0
    int m_1cc;                            // +0x1cc
    int m_1d0;                            // +0x1d0
    char m_1d4;                           // +0x1d4
    char m_1d5;                           // +0x1d5
    int m_1d8;                            // +0x1d8（-1）
    std::vector<int> m_vec1dc;            // +0x1dc
    char m_1e8;                           // +0x1e8
    int m_1ec;                            // +0x1ec
    int m_1f0;                            // +0x1f0
    int m_1f4;                            // +0x1f4
    std::vector<std::string> m_vec1f8;    // +0x1f8
    std::vector<int> m_vec204;            // +0x204
    std::vector<int> m_vec210;            // +0x210
    int m_21c;                            // +0x21c
    int m_220;                            // +0x220
    int m_224;                            // +0x224（-1）
    char m_228[0x11];                     // +0x228（m_228[3]=1）
    int m_23c;                            // +0x23c
    int m_240;                            // +0x240
    int m_244;                            // +0x244
    int m_248;                            // +0x248
    int m_24c;                            // +0x24c
    int m_250;                            // +0x250
    int m_254;                            // +0x254
    int m_258;                            // +0x258（-1）
    int m_25c;                            // +0x25c（-1）
    char m_260;                           // +0x260
    int m_264;                            // +0x264
    std::vector<STPassiveFeatureData> m_passive268;  // +0x268
    std::string m_str274;                 // +0x274
    std::string m_str278;                 // +0x278
    char m_280;                           // +0x280（1）
    std::vector<int> m_vec284;            // +0x284
    char m_290;                           // +0x290
    std::vector<int> m_vec294;            // +0x294
    std::vector<int> m_vec2a0;            // +0x2a0
};

int ImportSkillScript(STSkillScript* script, const char* path,
                      int job, int a, int b);

#endif  // GAME_STSKILLSCRIPT_H_
