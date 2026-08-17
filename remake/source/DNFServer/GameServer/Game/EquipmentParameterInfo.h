#ifndef GAME_EQUIPMENTPARAMETERINFO_H_
#define GAME_EQUIPMENTPARAMETERINFO_H_

// ============================================================================
// df_game_r 还原 —— EquipmentParameterInfo（G5 装备参数脚本，约 0x480 字节）
// 布局依据 ORIG clear（0x089193c0）/ 析构（0x0891a4c6）推导；
// getEquipment* 访问器（0x089c2e48..0x089c2e96）返回装备能力数组元素引用。
// 解析函数（importEquipmentParameterInfo / readAction / readCondtion /
// scanRandomOptionValue / scanIf / scanThen / importAllSkillItem /
// importItemOverpowerDesc / exportEquipmentParameterScript）在同一 TU。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>

enum ENUM_CHANGE_STATUS_TYPE
{
    ENUM_CHANGE_STATUS_TYPE_0 = 0
};

struct stVendingMachineOutput_t
{
    int m_field0;   // +0x00
};

struct STResultItemCnt
{
    int m_field0;   // +0x00
};

struct ItemAuraInfo
{
    int m_field0;   // +0x00
};

struct EquipmentSkillDataUp
{
    int m_field0;   // +0x00
};

struct LevelUpSkill
{
    int m_job;      // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
};

struct STAvatarAbilityVariation
{
    int m_field0;   // +0x00
};

struct VariableStat
{
    int m_field0;   // +0x00
};

class EquipmentParameterInfo
{
public:
    EquipmentParameterInfo();
    EquipmentParameterInfo(const EquipmentParameterInfo& other);
    ~EquipmentParameterInfo();
    EquipmentParameterInfo& operator=(const EquipmentParameterInfo& other);

    void clear();
    void addString(std::string str);
    int& getEquipmentPhysicalAttack(int level);
    int& getEquipmentPhysicalDefense(int level);
    int& getEquipmentMagicalAttack(int level);
    int& getEquipmentMagicalDefense(int level);

    // ---- 嵌套结构 ----
    struct RebirthAppendageinfo
    {
        RebirthAppendageinfo();
        void clear();
        int m_field0;   // +0x00
        int m_field4;   // +0x04
        int m_field8;   // +0x08
    };

    struct EquipmentAuraAppendageData
    {
        int m_field0;   // +0x00
    };

    struct SASPROPERTY
    {
        SASPROPERTY();
        SASPROPERTY(const SASPROPERTY& other);
        ~SASPROPERTY();
        SASPROPERTY& operator=(const SASPROPERTY& other);
        int m_field0;   // +0x00
        std::string m_str4;  // +0x04
    };

    // ---- 成员布局 ----
    int m_field0;                                      // +0x00（2）
    std::string m_str4;                                // +0x04
    std::vector<std::pair<ENUM_CHANGE_STATUS_TYPE, int> > m_vec0c;  // +0x0c
    char m_pad18[8];                                   // +0x18
    std::vector<stVendingMachineOutput_t> m_vec20;     // +0x20
    std::vector<std::pair<int, int> > m_vec2c;         // +0x2c
    char m_pad38[0x1c];                                // +0x38
    int m_phyAtt[2];                                   // +0x54
    int m_phyDef[2];                                   // +0x5c
    char m_pad64[0x10];                                // +0x64
    int m_74;                                          // +0x74
    int m_78;                                          // +0x78
    int m_7c;                                          // +0x7c
    int m_80;                                          // +0x80
    int m_84;                                          // +0x84
    int m_88;                                          // +0x88
    int m_8c;                                          // +0x8c
    char m_pad90[0x78];                                // +0x90
    int m_magAtt[2];                                   // +0x108
    int m_magDef[2];                                   // +0x110
    char m_pad118[0x40];                               // +0x118
    std::vector<int> m_vec158;                         // +0x158
    char m_pad164[0x18];                               // +0x164
    std::vector<int> m_vec17c;                         // +0x17c
    std::vector<ItemAuraInfo> m_aura188;               // +0x188
    std::vector<int> m_vec194;                         // +0x194
    char m_pad1a0[0x18];                               // +0x1a0
    std::vector<EquipmentSkillDataUp> m_skill1b8;      // +0x1b8
    std::map<int, std::string> m_descMap1c4;           // +0x1c4
    std::vector<EquipmentSkillDataUp> m_skill1dc;      // +0x1dc
    std::vector<std::vector<EquipmentSkillDataUp> > m_skill1e8;  // +0x1e8
    std::vector<std::string> m_vec1f4;                 // +0x1f4
    std::vector<EquipmentAuraAppendageData> m_aura200; // +0x200
    char m_pad20c[8];                                  // +0x20c
    std::vector<int> m_vec214;                         // +0x214
    char m_pad220[0x20];                               // +0x220
    std::vector<std::vector<float> > m_vec240;         // +0x240
    std::vector<LevelUpSkill> m_levelUpSkills24c;      // +0x24c
    char m_pad258[0xc];                                // +0x258
    std::vector<STAvatarAbilityVariation> m_avatar264; // +0x264
    char m_pad270[8];                                  // +0x270
    std::string m_str278;                              // +0x278
    std::string m_str27c;                              // +0x27c
    char m_pad280[0x108];                              // +0x280
    int m_388[5];                                      // +0x388
    char m_pad39c[0x74];                               // +0x39c
    int m_410;                                         // +0x410
    int m_414;                                         // +0x414
    char m_pad418[0x50];                               // +0x418（._119 匿名结构）
    std::vector<std::pair<int, int> > m_vec468;        // +0x468
    std::vector<VariableStat> m_var474;                // +0x474
};

// ---- 解析/导出函数（ORIG 符号） ----
bool importEquipmentParameterInfo(EquipmentParameterInfo* info,
                                  std::string& path, const char* dir,
                                  int& a, int& b);
void exportEquipmentParameterScript(FILE* file, EquipmentParameterInfo* info);
void readAction(const std::string& line, EquipmentParameterInfo* info,
                std::string& out, std::vector<float>& values, int idx);
void readCondtion(EquipmentParameterInfo* info, std::string& line,
                  std::vector<int>& values);
bool scanRandomOptionValue(EquipmentParameterInfo* info, std::string& line);
void scanIf(EquipmentParameterInfo* info, int idx);
void scanThen(std::string line, EquipmentParameterInfo* info, int idx);
void importAllSkillItem(EquipmentParameterInfo* info);
void importItemOverpowerDesc(EquipmentParameterInfo* info);

#endif  // GAME_EQUIPMENTPARAMETERINFO_H_
