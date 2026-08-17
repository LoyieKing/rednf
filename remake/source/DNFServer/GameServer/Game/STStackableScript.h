#ifndef GAME_STSTACKABLESCRIPT_H_
#define GAME_STSTACKABLESCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STStackableScript（G5 堆叠物脚本表，尺寸 0x518）
// 布局依据 ORIG Clear（0x0899087a）/ 析构（0x0899013c）逐成员推导；
// 基类 STItemScript（0x178）。Arad_STStackableScript 为 +0x178 内嵌类。
// CItemList.h 中的简化声明由本头替换。
// ============================================================================

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "STItemScript.h"

struct STStackablePackageData
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
};

#ifndef GAME_CSTACKABLEITEM_H_
namespace StackableAction
{
enum UsablePlace
{
    UsablePlace_0 = 0
};
enum UsableServerType
{
    UsableServerType_0 = 0
};
}
#endif

struct CRabbitBox
{
    int m_field0;   // +0x00
};

struct STPassiveObjectInStackable
{
    int m_field0;   // +0x00
};

struct STStatusGenerate
{
    int m_field0;   // +0x00
};

class STStatChangeVector
{
public:
    void clear();
    int m_field0;   // +0x00
};

#ifndef GAME_CSTACKABLEITEM_H_
struct stStackableBooster_t
{
    int m_field0;   // +0x00
};

struct stLimitCubeInfo
{
    int m_field0;   // +0x00
};

struct BoosterSelectInfo
{
    int m_field0;   // +0x00
};

class RecipeInfo
{
public:
    void additionalInfo(class STStackableScript& script);
    int m_field0;   // +0x00
};
#endif

// ---- 附魔键/值（CStackableItem.h 无同名定义） ----
struct STEnchantSystemMapKey
{
    int m_field0;   // +0x00
    bool operator<(const STEnchantSystemMapKey& o) const
    {
        return m_field0 < o.m_field0;
    }
};

struct STEnchantSystemMapData
{
    int m_field0;   // +0x00
};

enum ENUM_MODULE_TYPE
{
    ENUM_MODULE_TYPE_0 = 0
};

class EquipmentParameterInfo;
class STStackableScript;

class Arad_STStackableScript
{
public:
    Arad_STStackableScript();
    ~Arad_STStackableScript();
    Arad_STStackableScript& operator=(const Arad_STStackableScript& other);
    void AradStClear();

    int m_0;          // +0x00
    char m_4;         // +0x04（-1）
    std::string m_str8;   // +0x08
    std::string m_strc;   // +0x0c
    std::string m_str10;  // +0x10
    char m_pad14[4];  // +0x14
};

class STStackableScript : public STItemScript
{
public:
    STStackableScript();
    STStackableScript(STStackableScript& other);
    ~STStackableScript();
    STStackableScript& operator=(const STStackableScript& other);
    void Clear();

    Arad_STStackableScript m_arad178;                 // +0x178
    std::vector<int> m_vec190;                        // +0x190
    std::vector<CRabbitBox> m_rabbit19c;              // +0x19c
    int m_1a8;                                        // +0x1a8
    int m_1ac;                                        // +0x1ac
    int m_1b0;                                        // +0x1b0
    int m_1b4;                                        // +0x1b4（-1）
    char m_1b8;                                       // +0x1b8
    char m_1b9;                                       // +0x1b9
    char m_1ba;                                       // +0x1ba
    char m_1bb;                                       // +0x1bb
    char m_1bc;                                       // +0x1bc
    char m_1bd;                                       // +0x1bd
    char m_1be;                                       // +0x1be
    char m_1bf;                                       // +0x1bf
    char m_1c0;                                       // +0x1c0
    char m_pad1c1[3];                                 // +0x1c1
    int m_1c4;                                        // +0x1c4
    char m_1c8;                                       // +0x1c8
    char m_pad1c9[0xf];                               // +0x1c9
    int m_1d4;                                        // +0x1d4
    std::string m_str1d8;                             // +0x1d8
    int m_1dc;                                        // +0x1dc
    int m_1e0;                                        // +0x1e0（0x27）
    int m_1e4;                                        // +0x1e4
    char m_pad1e8[4];                                 // +0x1e8
    std::vector<int> m_vec1ec;                        // +0x1ec
    std::vector<StackableAction::UsablePlace> m_place1f8;  // +0x1f8
    std::vector<STPassiveObjectInStackable> m_passive208;  // +0x208
    std::vector<StackableAction::UsableServerType> m_server214;  // +0x214
    std::vector<int> m_vec224;                        // +0x224
    std::string m_str230;                             // +0x230
    std::string m_str234;                             // +0x234
    std::string m_str238;                             // +0x238
    int m_23c;                                        // +0x23c（1）
    int m_240;                                        // +0x240（-1）
    std::vector<std::vector<int> > m_vec244;          // +0x244
    std::vector<int> m_vec250;                        // +0x250
    std::vector<int> m_vec25c;                        // +0x25c
    std::vector<STStackablePackageData> m_package268; // +0x268
    std::vector<std::vector<STStackablePackageData> > m_package274;  // +0x274
    stStackableBooster_t m_booster280;                // +0x280
    std::vector<STStackablePackageData> m_package2a4; // +0x2a4
    char m_pad2b0[0x3c];                              // +0x2b0
    STStatusGenerate m_status2ec;                     // +0x2ec
    STStatChangeVector m_statChange34c;               // +0x34c
    std::string m_str368;                             // +0x368
    std::vector<std::pair<int, int> > m_vec374;       // +0x374
    char m_pad380[0x24];                              // +0x380
    std::map<int, int> m_map3a4;                      // +0x3a4
    std::vector<std::pair<int, int> > m_vec3c0;       // +0x3c0
    char m_pad3cc[8];                                 // +0x3cc
    stLimitCubeInfo m_cube3d4;                        // +0x3d4
    std::map<STEnchantSystemMapKey, STEnchantSystemMapData> m_enchant3f4;  // +0x3f4
    std::string m_str410;                             // +0x410
    BoosterSelectInfo m_boosterSelect41c;             // +0x41c
    char m_pad438[0x48];                              // +0x438..+0x47f
    std::set<int> m_set480;                           // +0x480
    std::set<int> m_set498;                           // +0x498
    std::string m_str4b4;                             // +0x4b4
    std::map<int, EquipmentParameterInfo*> m_module4bc;  // +0x4bc
    std::set<int> m_set4d4;                           // +0x4d4
    char m_pad4e0[0x1c];                              // +0x4e0
    std::vector<int> m_vec4fc;                        // +0x4fc
    std::set<int> m_set50c;                           // +0x50c
};

bool ImportStackableScript(STStackableScript* script, const char* path,
                           bool flag, int idx);
bool StackableScriptSpecialMonsterDrop(STStackableScript* script,
                                       std::string str, const char* path);

#endif  // GAME_STSTACKABLESCRIPT_H_
