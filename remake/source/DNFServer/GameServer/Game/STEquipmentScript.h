#ifndef GAME_STEQUIPMENTSCRIPT_H_
#define GAME_STEQUIPMENTSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STEquipmentScript（G5 装备脚本表，尺寸 0x898）
// 布局依据 docs/class_func_reports/STEquipmentScript.md（Clear 0x0898e9e4 /
// ctor 0x0898db82 / dtor 0x0898e298）逐成员推导；基类 STItemScript（0x178）。
// CItemList.h 中的简化声明由本头替换（见 CItemList.h 头注释）。
// ============================================================================

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "EquipmentParameterInfo.h"
#include "STItemScript.h"

class STKeyCommand;

#ifndef GAME_CEQUIPITEM_H_
namespace ItemUpgradeType
{
enum T
{
    T_0 = 0
};
}

struct stAvatarTypeSelect_t
{
    int m_field0;       // +0x00
    int m_field4;       // +0x04
    int m_field8;       // +0x08
    int m_fieldc;       // +0x0c
    int m_field10;      // +0x10
    unsigned short m_socketType[4];  // +0x14
};
#endif

enum ENUM_EQUIPMENTTYPE
{
    ENUM_EQUIPMENTTYPE_0 = 0
};

#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

struct STChangeStatusRate
{
    int m_field0;   // +0x00
};

struct STEnchantSystemData
{
    int m_field0;   // +0x00
};

struct stSetItemBackImageInfo
{
    int m_field0;   // +0x00
};

struct EquipmentAniInfoScript
{
    int m_field0;   // +0x00
};

namespace effect
{
struct STEffect
{
    int m_field0;   // +0x00
};
}

class STEquipmentScript : public STItemScript
{
public:
    STEquipmentScript();
    ~STEquipmentScript();
    void Clear();

    EquipmentAniInfoScript& addAniInfo(ENUM_CHARACTERJOB job);
    EquipmentAniInfoScript& getAniInfo(ENUM_CHARACTERJOB job);
    bool isUpgradableLevel(ItemUpgradeType::T type, int level);
    bool isExistUpgradableLevel();

    struct artifact_info
    {
        artifact_info();
        bool isUsable() const;
        int m_field0;    // +0x00
        int m_field4;    // +0x04
        int m_field8;    // +0x08
        float m_c;       // +0x0c
        float m_10;      // +0x10
        float m_14;      // +0x14
        int m_18;        // +0x18
        int m_1c;        // +0x1c
        int m_20;        // +0x20
        int m_24;        // +0x24
        int m_28;        // +0x28
    };

    // ---- 成员布局（+0x178 起，基类 STItemScript 0x178） ----
    int m_178;                                    // +0x178（-1）
    int m_17c;                                    // +0x17c（0x46）
    int m_180;                                    // +0x180（-1）
    int m_184;                                    // +0x184（-1）
    int m_188;                                    // +0x188（-1）
    std::vector<std::string> m_vec18c;            // +0x18c
    std::vector<int> m_vec198;                    // +0x198
    std::string m_str1a4;                         // +0x1a4
    std::vector<std::string> m_vec1a8;            // +0x1a8
    int m_field1b4;                               // +0x1b4（0x1a）
    int m_1b8;                                    // +0x1b8
    int m_1bc;                                    // +0x1bc
    EquipmentParameterInfo m_param1c0;            // +0x1c0
    std::map<ENUM_CHARACTERJOB, EquipmentAniInfoScript> m_aniInfo640;  // +0x640
    std::vector<std::string> m_vec658;            // +0x658
    std::string m_str664;                         // +0x664
    std::string m_str668;                         // +0x668
    std::vector<int> m_vec66c;                    // +0x66c
    std::map<int, EquipmentParameterInfo> m_paramMap678;  // +0x678
    std::vector<EquipmentParameterInfo> m_paramVec690;    // +0x690
    char m_69c;                                   // +0x69c
    int m_6a0;                                    // +0x6a0（-1）
    std::string m_str6a4;                         // +0x6a4
    std::string m_str6a8;                         // +0x6a8
    std::string m_str6ac;                         // +0x6ac
    std::string m_str6b0;                         // +0x6b0
    std::string m_str6b4;                         // +0x6b4
    std::string m_str6b8;                         // +0x6b8
    int m_6bc;                                    // +0x6bc
    std::vector<stAvatarTypeSelect_t> m_avatar6c0;  // +0x6c0
    std::vector<unsigned short> m_vec6cc;         // +0x6cc
    int m_6d8;                                    // +0x6d8（-1）
    int m_6dc;                                    // +0x6dc（-1）
    int m_6e0;                                    // +0x6e0
    int m_6e4;                                    // +0x6e4（-1）
    std::vector<STChangeStatusRate> m_rate6e8;    // +0x6e8
    char m_6f4;                                   // +0x6f4
    char m_6f5;                                   // +0x6f5
    char m_6f6;                                   // +0x6f6
    int m_6f8;                                    // +0x6f8
    std::map<ItemUpgradeType::T, std::pair<int, int> > m_upgrade6fc;  // +0x6fc
    std::set<int> m_set714;                       // +0x714
    char m_72c;                                   // +0x72c
    char m_72d;                                   // +0x72d
    int m_730;                                    // +0x730
    int m_734;                                    // +0x734
    char m_pad738[0x2c];                          // +0x738
    int m_764;                                    // +0x764
    std::vector<ENUM_EQUIPMENTTYPE> m_equipTypes768;  // +0x768
    std::vector<int> m_vec774;                    // +0x774
    std::vector<int> m_vec780;                    // +0x780
    char m_78c;                                   // +0x78c
    int m_790;                                    // +0x790
    int m_794;                                    // +0x794
    int m_798;                                    // +0x798（-1）
    int m_79c;                                    // +0x79c（-1）
    std::vector<int> m_vec7a0;                    // +0x7a0
    char m_7ac;                                   // +0x7ac
    std::vector<std::string> m_vec7b0;            // +0x7b0
    char m_7bc;                                   // +0x7bc
    int m_7c0;                                    // +0x7c0（-1）
    int m_7c4;                                    // +0x7c4（-1）
    char m_7c8;                                   // +0x7c8
    std::vector<STEnchantSystemData> m_enchant7cc;  // +0x7cc
    std::vector<std::pair<int, int> > m_vec7d8;   // +0x7d8
    std::vector<std::pair<int, int> > m_vec7e4;   // +0x7e4
    std::string m_str7f0;                         // +0x7f0
    std::string m_str7f4;                         // +0x7f4
    STKeyCommand* m_keyCommand;                   // +0x804
    int m_808;                                    // +0x808（3）
    unsigned short m_80c;                         // +0x80c
    unsigned short m_80e;                         // +0x80e
    int m_810;                                    // +0x810
    char m_814;                                   // +0x814（2）
    int m_818;                                    // +0x818
    char m_pad81c[0x10];                          // +0x81c
    char m_82c;                                   // +0x82c
    std::vector<int> m_vec830;                    // +0x830
    std::string m_str83c;                         // +0x83c
    std::vector<stSetItemBackImageInfo> m_setItem840;  // +0x840
    std::vector<effect::STEffect> m_effect84c;    // +0x84c
    std::vector<std::string> m_vec858;            // +0x858
    std::vector<bool> m_vec864;                   // +0x864
    std::string m_str878;                         // +0x878
    int m_87c;                                    // +0x87c
    char m_880;                                   // +0x880（-1）
    int m_884;                                    // +0x884
    char m_888;                                   // +0x888
    char m_889;                                   // +0x889（1）
    char m_88a;                                   // +0x88a
    char m_88b;                                   // +0x88b
    int m_88c;                                    // +0x88c
    int m_890;                                    // +0x890
};

bool ImportEquipmentScript(STEquipmentScript* script, const char* path,
                           bool flag, int a, int idx);
bool EquipmentScriptSpecialMonsterDrop(STEquipmentScript* script,
                                       std::string str, const char* path);
bool importArtifactInfo(STEquipmentScript::artifact_info& info,
                        std::string str);

#endif  // GAME_STEQUIPMENTSCRIPT_H_
