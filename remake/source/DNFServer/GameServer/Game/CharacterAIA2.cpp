// ============================================================================
// CharacterAIA2.cpp —— 角色/AI 脚本依赖恢复（CharacterAIA2 批次独占 TU）。
//
// 本 TU 提供的 ORIG df_game_r 真实符号（nm 核验）：
//   STAICharacterCommonScript C1 T 0x088c8a78 / clear T 0x088c9592 /
//     D1 T 0x088c903c / D0 T 0x088c9574 / destroy T 0x088c98bc
//     （C1/D1 报告 docs/class_func_reports/STAICharacterCommonScript/ 完整转写；
//       clear/destroy/D0 报告缺失 → objdump 转写）
//   STAIInfoScript C1 W 0x08366f48 / D1 W 0x083671a4 / clear W 0x083673d4 /
//     destroy W 0x0836753a
//     （copy-ctor 报告 docs/class_func_reports/STAIInfoScript/STAIInfoScript.md
//       完整转写成员集；默认 C1/D1/clear/destroy 报告缺失 → objdump 转写）
//   CAICharacter C1 W 0x08373a9a / D1 W 0x08373b32（报告缺失 → objdump 转写）
//   importAICharacterCommonScript T 0x088c9ccc
//     （报告 _lt_global_gt_/importAICharacterCommonScript.md 完整转写）
//   importAIInfoScript T 0x08a390db
//     （报告 _lt_global_gt_/importAIInfoScript.md 完整转写）
//   CNPCDynamicInfo C1 W 0x0858191c（报告缺失 → objdump 转写）
//   PowerParameterScript::clear W 0x0837187a（报告缺失 → objdump 转写）
//   isKeyFile T 0x088c9a4c / getFileName T 0x088c9a98 / makeKeyFullPath T 0x08a38b5d
//     makeAIFullPath T 0x08a38af4 / makeFullPath T 0x088c9990 / stringToInt T 0x08a38a8b
//     （各 _lt_global_gt_/*.md 报告完整转写）
//   ACTIVEOBJECT_STATE_STRING D 0x093f4d80 / ACTIVESTATUS_ATTACK_TAG D 0x093f54e0
//     （ORIG .data 指针表逐项解引用 .rodata 字符串提取，NULL 终止实证）
// ============================================================================

#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "CDataManager.h"            // PowerParameterScript 完整声明 + stMonsterIndependentDrop_t
#include "STCharacterScript.h"       // STRDCharacterStatus / ActiveActionFileSet 权威布局
#include "EquipmentParameterInfo.h"  // EquipmentSkillDataUp + importSkillDataUp 声明
#include "DNFLexWrapper.h"           // loadRDARScriptFile/getDirectoryPath/Scan*
#include "CreatureScript.h"          // getElementName(ENUM_ELEMENT)

// ScriptDeps 域提供的全局（ORIG T 0x088bfd97）；无公共头声明，此处补一致声明。
int getItemRarity(const char* name);

// ORIG B 0x094f6920（char[0x100]，定义在 InitScriptProducers_5.cpp）。
extern char g_aiCharacterScriptBaseDirectory[0x100];

// ============================================================================
// ORIG 字符串表数据（extern "C"，nm: D 符号无 mangled 前缀）
// ============================================================================

// D 0x093f4d80：15 项 + NULL 终止（objdump -s .data/.rodata 逐指针提取）。
extern "C" const char* ACTIVEOBJECT_STATE_STRING[16];
const char* ACTIVEOBJECT_STATE_STRING[16] = {
    "[stand]",       "[sit]",         "[sit forever]", "[damage]",
    "[down]",        "[die]",         "[jump]",        "[jump attack]",
    "[attack]",      "[hold]",        "[throw]",       "[dash]",
    "[dash attack]", "[get item]",    "[buff]",        0,
};

// D 0x093f54e0：19 项 + NULL 终止。
extern "C" const char* ACTIVESTATUS_ATTACK_TAG[20];
const char* ACTIVESTATUS_ATTACK_TAG[20] = {
    "[slow]",   "[freeze]", "[poison]", "[stun]",
    "[curse]",  "[blind]",  "[lightning]", "[stone]",
    "[sleep]",  "[burn]",   "[weapon break]", "[bleeding]",
    "[haste]",  "[bless]",  "[element]", "[confuse]",
    "[hold]",   "[armor break]", "[none]", 0,
};

// ============================================================================
// 路径/表工具函数（各 _lt_global_gt_/*.md 报告全文转写）
// ============================================================================

// isKeyFile —— 报告 _lt_global_gt_/isKeyFile.md：非空且以 ".key" 结尾。
bool isKeyFile(const char* name)
{
    if (name == 0)
        return false;
    return std::strcmp(name + std::strlen(name) - 4, ".key") == 0;
}

// stringToInt —— 报告 _lt_global_gt_/stringToInt.md：线性查表，max==-1 视为 999。
int stringToInt(const char* s, const char** table, int maxCount)
{
    if (maxCount == -1)
        maxCount = 999;
    for (int i = 0; i < maxCount && table[i] != 0; ++i)
    {
        if (std::strcmp(table[i], s) == 0)
            return i;
    }
    return -1;
}

// getFileName —— 报告 _lt_global_gt_/getFileName.md：拷贝入串后从尾向前找
// '/' 或 '\\' 截掉目录前缀，再找 '.' 截掉扩展名。sret 返回。
std::string getFileName(const std::string& path)
{
    std::string out(path);
    std::size_t pos = std::string::npos;
    for (std::size_t i = out.size(); i-- > 0; )
    {
        if (out[i] == '/' || out[i] == '\\')
        {
            pos = i;
            break;
        }
    }
    if (pos != std::string::npos)
        out.erase(0, pos + 1);
    for (std::size_t i = out.size(); i-- > 0; )
    {
        if (out[i] == '.')
        {
            out.erase(i);
            break;
        }
    }
    return out;
}

// makeFullPath —— 报告 _lt_global_gt_/makeFullPath.md：path 不含 '/'/'\\' 时
// dir+path 回写 path 并返 true；否则不动返 false。首参按值传递（mangled Ss）。
bool makeFullPath(std::string directory, std::string& path)
{
    const std::size_t slash = path.find('/', 0);
    const std::size_t backslash = path.find('\\', 0);
    if (slash == std::string::npos && backslash == std::string::npos)
    {
        path = directory + path;
        return true;
    }
    return false;
}

// makeAIFullPath —— 报告 _lt_global_gt_/makeAIFullPath.md：dir+"ai/" 后接 makeFullPath。
void makeAIFullPath(std::string& dir, std::string& name)
{
    std::string combined = dir + "ai/";
    makeFullPath(combined, name);
}

// makeKeyFullPath —— 报告 _lt_global_gt_/makeKeyFullPath.md：dir+"key/" 后接 makeFullPath。
void makeKeyFullPath(std::string& dir, std::string& name)
{
    std::string combined = dir + "key/";
    makeFullPath(combined, name);
}

// ============================================================================
// 子类型（生命周期符号均为 ORIG W；报告缺失，objdump 转写）
// ============================================================================

// MinimumInfo：+0x00 std::string / +0x04 int（sizeof 8）。
// C1 W 0x088cc3e2 = string ctor；D1 W 0x088cc3f6 = string dtor；
// clear W 0x088cc190 = 仅置 (+this)+4 = 0。
struct MinimumInfo
{
    MinimumInfo() {}
    ~MinimumInfo() {}
    void clear() { m_field4 = 0; }

    std::string m_name;     // +0x00（[minimum info] 首个 ScanStr）
    int         m_field4;   // +0x04（第 5 个 ScanInt）
};

// APCSpeech（sizeof 0x18）：+0x00 vector<string> / +0x0c vector<int>。
// C1 W 0x088cc328 = 两 vector ctor + clear()；D1 W 0x088cc40a = 逆序 vector dtor；
// clear W 0x088cc390；add W 0x088cc3b2 = lines.push_back(s); probs.push_back(i)。
struct APCSpeech
{
    APCSpeech()
    {
        m_lines.clear();
        m_probs.clear();
    }
    ~APCSpeech() {}
    void clear()
    {
        m_lines.clear();
        m_probs.clear();
    }
    void add(const std::string& line, int prob)
    {
        m_lines.push_back(line);
        m_probs.push_back(prob);
    }

    std::vector<std::string> m_lines;  // +0x00
    std::vector<int>         m_probs;  // +0x0c
};
typedef char assert_apc_speech_size[(sizeof(APCSpeech) == 0x18) ? 1 : -1];

// STRDCharacterStatusRate（sizeof 0xa4 = 41 float）。
// C1 W 0x088cc2fa = Clear()；Clear W 0x088cc1a0：
//   +0x00..+0x24 十个 1.0f；循环 i=0..3 写 +0x28+4i；循环 k=0..16 写 +0x38+4k
//   （k=16 → +0x78）；尾部 +0x7c..+0xa0 十个 1.0f。41 float 全覆盖。
struct STRDCharacterStatusRate
{
    STRDCharacterStatusRate() { Clear(); }

    void Clear()
    {
        for (int i = 0; i < 10; ++i) m_head[i] = 1.0f;
        for (int i = 0; i <= 3; ++i) m_mid28[i] = 1.0f;
        for (int k = 0; k <= 16; ++k) m_mid38[k] = 1.0f;
        for (int i = 0; i < 10; ++i) m_tail[i] = 1.0f;
    }

    float m_head[10];   // +0x00..+0x24
    float m_mid28[4];   // +0x28..+0x34
    float m_mid38[17];  // +0x38..+0x78（k=16 → 0x78）
    float m_tail[10];   // +0x7c..+0xa0
};  // sizeof == 0xa4
typedef char assert_strd_rate_size[(sizeof(STRDCharacterStatusRate) == 0xa4) ? 1 : -1];

// ---- vector 元素 POD（元素步长由各自 vector<T>::push_back 的 construct 步长
//      objdump 实证；字段语义为【推断】，仅按解析写入顺序命名）----
struct STAICharacterEquipmentData  // push_back 步长 0xc（W 0x088cc8cc 区）
{
    int m_a;  // ScanInt 第 1 值
    int m_b;  // ScanInt 第 2 值
    int m_c;  // ScanInt 第 3 值
};  // 0xc
typedef char assert_eqdata_size[(sizeof(STAICharacterEquipmentData) == 0xc) ? 1 : -1];

struct STAICharacterSkillData  // 步长 0x8（W 0x088cc940 区）
{
    int m_a;  // ScanInt(&ok)
    int m_b;  // ScanInt(NULL)
};  // 0x8

struct STAICharacterItemData  // 步长 0x8（W 0x088cc9b4 区）
{
    int m_a;
    int m_b;
};  // 0x8

struct STAICharacterCustomSkillLevel  // 步长 0x10（W 0x088ccb62 区）
{
    int m_a;  // ScanInt(&ok)
    int m_b;  // ScanInt(NULL)
    int m_c;  // ScanInt(NULL)
    int m_d;  // ScanInt(NULL)
};  // 0x10

struct STAICharacterSkillCorrection  // C1 W 0x088cc30e：{0, 1.0f}
{
    int   m_skill;
    float m_rate;
    STAICharacterSkillCorrection() : m_skill(0), m_rate(1.0f) {}
};  // 0x8

// stEnamyDropAvatarElement_t：push_back 步长 0xc（W 0x088ccbd6 区）。
// 解析写入 {ScanInt, ScanInt, ScanInt 低字节}；第三字段高 3 字节 ORIG 为栈垃圾，
// 此处建模 unsigned char + pad（同样不初始化 pad，行为一致）。
struct stEnamyDropAvatarElement_t
{
    int           m_itemId;
    int           m_total;
    unsigned char m_flag;
    unsigned char m_pad[3];
};  // 0xc
typedef char assert_enamy_elem_size[(sizeof(stEnamyDropAvatarElement_t) == 0xc) ? 1 : -1];

// stEnamyDropAvatarInfo_t（sizeof 0x40）：C1 W 0x08368de8 = 构造 +0x30 vector；
// D1 W 0x08368dfe；clear W 0x088cc10c：
//   +0x00=0；+0x04=0；循环 i=0..3 清 +0x08+4i；循环 k=0..4 清 +0x18+4k；
//   +0x2c=0；+0x30 vector clear。
struct stEnamyDropAvatarInfo_t
{
    stEnamyDropAvatarInfo_t()
        : m_field0(0), m_field4(0), m_total(0)
    {
    }
    ~stEnamyDropAvatarInfo_t() {}

    void clear()
    {
        m_field0 = 0;
        m_field4 = 0;
        for (int i = 0; i <= 3; ++i) m_rates08[i] = 0.0f;
        for (int k = 0; k <= 4; ++k) m_rates18[k] = 0.0f;
        m_total = 0;
        m_elements.clear();
    }

    int                                     m_field0;          // +0x00
    int                                     m_field4;          // +0x04
    float                                   m_rates08[4];      // +0x08..+0x14
    float                                   m_rates18[5];      // +0x18..+0x28
    int                                     m_total;           // +0x2c
    std::vector<stEnamyDropAvatarElement_t> m_elements;        // +0x30
};  // sizeof == 0x3c
typedef char assert_enamy_info_size[(sizeof(stEnamyDropAvatarInfo_t) == 0x3c) ? 1 : -1];

// ============================================================================
// STAIInfoScript（多态：ORIG _ZTV/_ZTI/_ZTS14STAIInfoScript 存在；sizeof 0x224）
// copy-ctor 报告 STAIInfoScript/STAIInfoScript.md 完整转写成员集；
// 默认 C1/D1/clear/destroy 报告缺失 → objdump 转写。
// ============================================================================
class STAIInfoScript
{
public:
    STAIInfoScript();
    virtual ~STAIInfoScript();          // 发 D1/D0/vtable，匹配 ORIG 符号集
    void clear();                       // ORIG W 0x083673d4
    void destroy();                     // ORIG W 0x0836753a

    int m_thinkTerm;                    // +0x04
    int m_destinationChangeTerm;        // +0x08
    std::string m_dir;                  // +0x0c
    std::vector<std::string*> m_patterns[5];  // +0x10 easy/medium/hard/ultimate/hero
    int m_targetingBonus[22];           // +0x4c
    std::vector<int> m_targetingBonusStr[22]; // +0xa4
    int m_keepDistance;                 // +0x1ac
    std::vector<int> m_keepRange[5];    // +0x1b0/+0x1bc/+0x1c8/+0x1d4/+0x1e0
    int m_intelligence;                 // +0x1ec
    int m_field1f0;                     // +0x1f0
    int m_field1f4;                     // +0x1f4
    int m_vision;                       // +0x1f8
    int m_backVision;                   // +0x1fc
    bool m_noChangeTarget;              // +0x200
    char m_pad201[3];
    int  m_pod204[8];                   // +0x204..+0x220（外层 clear 清零）
};  // sizeof == 0x224
typedef char assert_stai_info_size[(sizeof(STAIInfoScript) == 0x224) ? 1 : -1];

// STAICharacterCommonScript（多态；sizeof 0x1014）。
// C1/D1 报告 docs/class_func_reports/STAICharacterCommonScript/ 完整转写；
// clear/destroy 报告缺失 → objdump 转写。
class STAICharacterCommonScript
{
public:
    STAICharacterCommonScript();           // ORIG T 0x088c8a78
    virtual ~STAICharacterCommonScript();  // D1 T 0x088c903c / D0 T 0x088c9574
    void clear();                          // ORIG T 0x088c9592
    void destroy();                        // ORIG T 0x088c98bc

    MinimumInfo m_minimumInfo;                     // +0x004
    STRDCharacterStatus m_status;                  // +0x00c（0x57 packed）
    char m_pad63[1];                               // +0x063
    std::vector<STAICharacterEquipmentData> m_equipment;       // +0x064
    std::vector<STAICharacterSkillData> m_skills[2];           // +0x070/+0x07c
    std::vector<STAICharacterSkillCorrection> m_corrections;   // +0x088
    std::vector<STAICharacterItemData> m_items[2];             // +0x094/+0x0a0
    STAIInfoScript m_info;                         // +0x0ac（0x224）
    std::vector<std::string> m_keyStreamFiles;     // +0x2d0
    std::vector<std::string> m_keyFiles;           // +0x2dc
    std::map<std::string, int> m_keyCooltimes;     // +0x2e8（sizeof 0x18 @gcc4.4）
    APCSpeech m_speeches[111];                     // +0x300（111×0x18 = 0xa68）
    std::vector<APCSpeech> m_onAttackSpeech;       // +0xd68
    std::vector<APCSpeech> m_onKeySpeech;          // +0xd74
    int m_rarity;                                  // +0xd80（clear=0）
    int m_appearancePoint;                         // +0xd84（clear=0）
    std::vector<std::pair<int, int> > m_deathTowerItems; // +0xd88
    std::vector<std::pair<int, int> > m_partyMembers;    // +0xd94
    int m_farAttackReactionRate;                   // +0xda0（clear=0）
    int m_pvpWinPointLow;                          // +0xda4（clear=0）
    int m_pvpWinPointHigh;                         // +0xda8（clear=0）
    STRDCharacterStatusRate m_statusRate;          // +0xdac（0xa4）
    std::vector<STAICharacterCustomSkillLevel> m_customSkills; // +0xe50
    float m_attackDamageRate;                      // +0xe5c（clear=1.0f）
    int m_addEquipStatusFromLevel;                 // +0xe60（clear=0）
    int m_armorSubtype;                            // +0xe64（clear=2）
    int m_defaultEquipment;                        // +0xe68（clear=1）
    std::vector<int> m_dollSpecialDungeons;        // +0xe6c
    STRDCharacterStatusRate m_specialStatusRate;   // +0xe78（0xa4）
    float m_attackDamageRateSpecial;               // +0xf1c（clear=1.0f）
    std::vector<int> m_championElements;           // +0xf20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops; // +0xf2c
    std::vector<int> m_limitPassiveObjects;        // +0xf38
    int m_overheadGaugeType;                       // +0xf44（clear=0）
    int m_overheadGaugePosX;                       // +0xf48（clear=0）
    int m_overheadGaugePosY;                       // +0xf4c（clear=0）
    stEnamyDropAvatarInfo_t m_avatarDrop;          // +0xf50（0x3c）
    int m_getupInvincible;                         // +0xf8c（clear=1）
    std::vector<EquipmentSkillDataUp> m_skillDataUps; // +0xf90
    std::vector<float> m_summonStatusRates;        // +0xf9c
    bool m_leaveBody;                              // +0xfa8（clear=true）
    char m_padfa9[3];
    ActiveActionFileSet m_actionSet;               // +0xfac（0x40）
    std::string m_lastAction;                      // +0xfec
    std::string m_procAction;                      // +0xff0
    bool m_addGravityInAerial;                     // +0xff4（clear=false）
    char m_padff5[3];
    float m_gravityAerialA;                        // +0xff8（clear=20.0f）
    float m_gravityAerialB;                        // +0xffc（clear=-310.0f）
    int m_noCooltime;                              // +0x1000（clear=0）
    bool m_checkAccountItem;                       // +0x1004（clear=true）
    char m_pad1005[3];
    int m_npcId;                                   // +0x1008（clear=-1）
    bool m_isHellMonster;                          // +0x100c（clear=false）
    char m_pad100d[3];
    int m_animationJob;                            // +0x1010（clear=11）
};  // sizeof == 0x1014
typedef char assert_stai_script_size[
    sizeof(STAICharacterCommonScript) == 0x1014 ? 1 : (int)-sizeof(STAICharacterCommonScript)];

// ---- 生命周期 ----

STAIInfoScript::STAIInfoScript()
{
    clear();
}

STAIInfoScript::~STAIInfoScript()
{
    // 成员逆序析构由编译器生成（ORIG D1 W 0x083671a4 同构）。
}

void STAIInfoScript::destroy()
{
    // ORIG 0x0836753a：释放 m_patterns 各桶内 new 出来的 std::string 后清空。
    for (int p = 0; p < 5; ++p)
    {
        std::vector<std::string*>& bucket = m_patterns[p];
        const std::size_t n = bucket.size();
        for (std::size_t i = 0; i < n; ++i)
            delete bucket[i];
        bucket.clear();
    }
}

void STAIInfoScript::clear()
{
    // ORIG 0x083673d4（objdump 逐条转写）。
    m_thinkTerm = 0x3e8;
    m_destinationChangeTerm = 0x3e8;
    for (int i = 0; i < 5; ++i)
        m_keepRange[i].clear();
    m_dir.clear();
    for (int i = 0; i <= 4; ++i)
        m_patterns[i].clear();
    for (int i = 0; i <= 21; ++i)
    {
        m_targetingBonus[i] = (i == 0) ? 0x32 : 0;
        m_targetingBonusStr[i].clear();
    }
    m_keepDistance = 0x64;
    m_intelligence = 0x64;
    m_field1f0 = 0x64;
    m_field1f4 = 0x1388;
    m_vision = 0x5dc;
    m_backVision = 0x5dc;
    m_noChangeTarget = true;
}

STAICharacterCommonScript::STAICharacterCommonScript()
{
    // ORIG C1 0x088c8a78：vptr + 逐成员构造后调 clear()。
    // 成员构造由编译器生成；m_status 无用户 ctor，由 clear 内 Clear() 补齐。
    clear();
}

STAICharacterCommonScript::~STAICharacterCommonScript()
{
    // ORIG D1 0x088c903c：vptr + destroy() + 成员逆序析构（编译器生成）。
    destroy();
}

void STAICharacterCommonScript::destroy()
{
    // ORIG 0x088c98bc：仅转发 STAIInfoScript::destroy。
    m_info.destroy();
}

void STAICharacterCommonScript::clear()
{
    // ORIG 0x088c9592（objdump 逐条转写，保持调用次序与默认值）。
    m_minimumInfo.clear();               // MinimumInfo(+4).clear
    m_status.Clear();                    // STRDCharacterStatus(+0xc).Clear
    m_equipment.clear();                 // +0x64
    m_skills[0].clear();                 // +0x70
    m_skills[1].clear();                 // +0x7c
    m_corrections.clear();               // +0x88
    m_items[0].clear();                  // +0x94
    m_items[1].clear();                  // +0xa0
    m_info.clear();                      // +0xac
    m_keyStreamFiles.clear();            // +0x2d0
    m_keyFiles.clear();                  // +0x2dc
    m_keyCooltimes.clear();              // +0x2e8
    m_rarity = 0;                        // +0xd80
    m_appearancePoint = 0;               // +0xd84
    m_deathTowerItems.clear();           // +0xd88
    m_partyMembers.clear();              // +0xd94
    m_farAttackReactionRate = 0;         // +0xda0
    m_pvpWinPointLow = 0;                // +0xda4
    m_pvpWinPointHigh = 0;               // +0xda8
    m_championElements.clear();          // +0xf20（ORIG 先于 StatusRate 清理）
    m_statusRate.Clear();                // +0xdac
    m_customSkills.clear();              // +0xe50
    m_attackDamageRate = 1.0f;           // +0xe5c
    m_addEquipStatusFromLevel = 0;       // +0xe60
    m_armorSubtype = 2;                  // +0xe64
    m_defaultEquipment = 1;              // +0xe68
    m_dollSpecialDungeons.clear();       // +0xe6c
    m_specialStatusRate.Clear();         // +0xe78
    m_attackDamageRateSpecial = 1.0f;    // +0xf1c
    m_independentDrops.clear();          // +0xf2c
    m_onAttackSpeech.clear();            // +0xd68
    m_onKeySpeech.clear();               // +0xd74
    m_limitPassiveObjects.clear();       // +0xf38
    m_overheadGaugeType = 0;             // +0xf44
    m_overheadGaugePosX = 0;             // +0xf48
    m_overheadGaugePosY = 0;             // +0xf4c
    m_avatarDrop.clear();                // +0xf50
    m_getupInvincible = 1;               // +0xf8c = 1
    m_skillDataUps.clear();              // +0xf90
    m_summonStatusRates.clear();         // +0xf9c
    m_leaveBody = true;                  // byte +0xfa8 = 1
    m_addGravityInAerial = false;        // byte +0xff4 = 0
    m_gravityAerialA = 20.0f;            // +0xff8 = 0x41a00000
    m_gravityAerialB = -310.0f;          // +0xffc = 0xc3960000
    m_noCooltime = 0;                    // +0x1000
    m_checkAccountItem = true;           // byte +0x1004 = 1
    for (int i = 0; i < 8; ++i)          // STAIInfoScript 尾部 POD 绝对 +0x2b0..+0x2cc
        m_info.m_pod204[i] = 0;          //   由外层 clear 负责清零
    m_npcId = -1;                        // +0x1008
    m_isHellMonster = false;             // byte +0x100c = 0
    m_animationJob = 11;                 // +0x1010 = 0xb
}

// ============================================================================
// importAIInfoScript —— ORIG T 0x08a390db
// 报告 _lt_global_gt_/importAIInfoScript.md 完整转写。
// ============================================================================
bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel);

bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel)
{
    if (*tag == "[think term]")
    {
        info->m_thinkTerm = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[destination change term]")
    {
        info->m_destinationChangeTerm = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[ai pattern]")
    {
        int bucket = 0;
        bool isType = false;
        std::string tok;
        while (ScanTypeOrStr(isType, tok, true))
        {
            if (isType)
            {
                if (tok == "[/ai pattern]")
                    break;
                if (tok == "[easy]")         bucket = 0;
                else if (tok == "[medium]")  bucket = 1;
                else if (tok == "[hard]")    bucket = 2;
                else if (tok == "[ultimate]") bucket = 3;
                else if (tok == "[hero]")    bucket = 4;
            }
            else
            {
                if (byLevel)
                    tok = info->m_dir + tok;   // ORIG: operator+ 后赋回
                else
                    makeAIFullPath(info->m_dir, tok);
                std::string* name = new std::string(tok);   // ORIG operator new(4) + string copy
                info->m_patterns[bucket].push_back(name);
            }
        }
        return true;
    }
    if (*tag == "[no change target]")
    {
        info->m_noChangeTarget = (ScanInt((bool*)0) != 0);
        return true;
    }
    if (*tag == "[targeting bonus]")
    {
        for (int i = 0; i <= 21; ++i)
        {
            info->m_targetingBonus[i] = ScanInt((bool*)0);
            if (i == 20)
            {
                std::string s;
                while (ScanStr(&s))
                {
                    int v = stringToInt(s.c_str(), ACTIVEOBJECT_STATE_STRING, -1);
                    info->m_targetingBonusStr[i].push_back(v);
                }
            }
            else if (i == 21)
            {
                std::string s;
                while (ScanStr(&s))
                {
                    int v = stringToInt(s.c_str(), ACTIVESTATUS_ATTACK_TAG, 0x13);
                    info->m_targetingBonusStr[i].push_back(v);
                }
            }
        }
        return true;
    }
    if (*tag == "[keep distance with target]")
    {
        info->m_keepDistance = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[keep range distance with target]" ||
        *tag == "[keep range distance with target 1]" ||
        *tag == "[keep range distance with target 2]" ||
        *tag == "[keep range distance with target 3]" ||
        *tag == "[keep range distance with target 4]" ||
        *tag == "[keep range distance with target 5]")
    {
        for (int i = 0; i < 4; ++i)
            info->m_keepRange[i].push_back(ScanInt((bool*)0));
        return true;
    }
    if (*tag == "[warlike]")
    {
        info->m_intelligence = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[intelligence]")
    {
        info->m_intelligence = ScanInt((bool*)0);
        info->m_field1f0 = ScanInt((bool*)0);
        info->m_field1f4 = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[vision]")
    {
        info->m_vision = ScanInt((bool*)0);
        return true;
    }
    if (*tag == "[back vision]")
    {
        info->m_backVision = ScanInt((bool*)0);
        return true;
    }
    return false;
}

// ============================================================================
// importAICharacterCommonScript —— ORIG T 0x088c9ccc
// 报告 _lt_global_gt_/importAICharacterCommonScript.md 完整转写。
// 返回 bool：文件加载失败 / [rarity] 解析失败或=6 / [pvp win point] 逆序 /
//   [drop avatar item] 任一扫描失败 / [animation job] 失败 /
//   兜底 importAIInfoScript 失败 → false；ScanType 终结 → true。
// ============================================================================
bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel);

bool importAICharacterCommonScript(STAICharacterCommonScript* script,
                                   const char* path, bool byLevel)
{
    if (!loadRDARScriptFile(g_aiCharacterScriptBaseDirectory, path))
        return false;

    script->clear();
    getDirectoryPath(script->m_info.m_dir, "%s%s",
                     g_aiCharacterScriptBaseDirectory, path);

    std::string tag;
    std::string tok;
    bool ok = false;

    while (true)
    {
        if (!ScanType(tag, true))
            return true;                       // LAB_088cc01f: ebx=1

        if (tag == "[minimum info]")
        {
            ScanStr(&script->m_minimumInfo.m_name);
            for (int i = 0; i < 4; ++i) ScanInt((bool*)0);
            script->m_minimumInfo.m_field4 = ScanInt((bool*)0);
            for (int i = 0; i < 6; ++i) ScanInt((bool*)0);
            ScanStr(&tok);
            for (int i = 0; i < 3; ++i) ScanInt((bool*)0);
        }
        else if (tag == "[NPC]")
        {
            script->m_npcId = ScanInt((bool*)0);          // +0x1008
        }
        else if (tag == "[additional character status]")
        {
            STRDCharacterStatus& st = script->m_status;
            st.m_hpMax       = ScanInt((bool*)0);         // +0x00
            st.m_mpMax       = ScanInt((bool*)0);         // +0x04
            st.m_physAttack  = (unsigned short)ScanInt((bool*)0);  // +0x08
            st.m_physDefense = (unsigned short)ScanInt((bool*)0);  // +0x0a
            st.m_magAttack   = (unsigned short)ScanInt((bool*)0);  // +0x0c
            st.m_magDefense  = (unsigned short)ScanInt((bool*)0);  // +0x0e
            st.m_inventoryLimit = ScanInt((bool*)0);      // +0x3a
            st.m_hpRegen     = (unsigned short)ScanInt((bool*)0);  // +0x3e
            st.m_mpRegen     = (unsigned short)ScanInt((bool*)0);  // +0x40
            st.m_moveSpeed   = ScanInt((bool*)0);         // +0x42
            st.m_attackSpeed = (unsigned short)ScanInt((bool*)0);  // +0x46
            st.m_castSpeed   = (unsigned short)ScanInt((bool*)0);  // +0x48
            st.m_hitRecovery = (unsigned short)ScanInt((bool*)0);  // +0x4a
            st.m_jumpPower   = (unsigned short)ScanInt((bool*)0);  // +0x4c
            st.m_weight      = ScanInt((bool*)0);         // +0x4e
            st.m_field52     = (unsigned char)ScanInt((bool*)0);   // +0x52
            for (int j = 0; j <= 3; ++j)                  // elemResist @+0x10
                st.m_elemResist[j] = (unsigned short)ScanInt((bool*)0);
            for (int n = 0; ; ++n)                        // activeResist @+0x18
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                if (n <= 16)
                    st.m_activeResist[n] = (short)v;  // ORIG 无界写；按数组边界裁剪
            }
        }
        else if (tag == "[equipment]")
        {
            while (true)
            {
                STAICharacterEquipmentData e;
                e.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                e.m_b = ScanInt((bool*)0);
                e.m_c = ScanInt((bool*)0);
                script->m_equipment.push_back(e);
            }
        }
        else if (tag == "[skill]" || tag == "[quick skill]")
        {
            while (true)
            {
                STAICharacterSkillData s;
                s.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                s.m_b = ScanInt((bool*)0);
                if (tag == "[quick skill]")
                    script->m_skills[0].push_back(s);      // +0x70
                else
                    script->m_skills[1].push_back(s);      // +0x7c
            }
        }
        else if (tag == "[item]" || tag == "[quick item]")
        {
            while (true)
            {
                STAICharacterItemData it;
                it.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                it.m_b = ScanInt((bool*)0);
                if (tag == "[quick item]")
                    script->m_items[0].push_back(it);      // +0x94
                else
                    script->m_items[1].push_back(it);      // +0xa0
            }
        }
        else if (tag == "[skill correction]")
        {
            while (true)
            {
                const int first = ScanInt(&ok);
                if (!ok)
                    break;
                STAICharacterSkillCorrection c;
                c.m_skill = first;
                c.m_rate = ScanFloat((bool*)0);
                script->m_corrections.push_back(c);
            }
        }
        else if (tag == "[key stream]")
        {
            bool again = true;
            while (again)
            {
                again = false;
                std::string keyFile;
                bool keyOk = ScanStr(&keyFile);
                if (!keyOk)
                    break;
                if (isKeyFile(keyFile.c_str()))
                {
                    tok = keyFile;
                    keyFile.clear();
                }
                else
                {
                    keyOk = ScanStr(&tok);
                }
                if (keyFile.empty())
                    keyFile = getFileName(tok);
                script->m_keyFiles.push_back(keyFile);
                if (keyOk)
                {
                    makeKeyFullPath(script->m_info.m_dir, tok);
                    script->m_keyStreamFiles.push_back(tok);
                    again = true;
                }
            }
        }
        else if (tag == "[key cooltime]")
        {
            while (ScanStr(&tok))
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_keyCooltimes[tok] = v;
            }
        }
        else if (tag == "[rarity]")
        {
            if (!ScanStr(&tok))
                return false;
            if (tok == "common")         script->m_rarity = 0;
            else if (tok == "uncommon")  script->m_rarity = 1;
            else if (tok == "rare")      script->m_rarity = 2;
            else if (tok == "unique")    script->m_rarity = 3;
            else if (tok == "epic")      script->m_rarity = 4;
            else if (tok == "chronicle") script->m_rarity = 5;
            script->m_rarity = getItemRarity(tok.c_str());
            if (script->m_rarity == 6)
                return false;
        }
        else if (tag == "[appearance point]")
        {
            script->m_appearancePoint = ScanInt((bool*)0);
        }
        else if (tag == "[death tower item]")
        {
            script->m_deathTowerItems.clear();
            while (true)
            {
                const int first = ScanInt(&ok);
                if (!ok)
                    break;
                const int second = ScanInt((bool*)0);
                script->m_deathTowerItems.push_back(std::pair<int, int>(first, second));
            }
        }
        else if (tag == "[leave body]")
        {
            script->m_leaveBody = (ScanInt((bool*)0) != 0);
        }
        else if (tag == "[waiting action]")   { ScanStr(&script->m_actionSet.m_waiting); }
        else if (tag == "[move action]")      { ScanStr(&script->m_actionSet.m_move); }
        else if (tag == "[dash action]")      { ScanStr(&script->m_actionSet.m_dash); }
        else if (tag == "[sit action]")       { ScanStr(&script->m_actionSet.m_sit); }
        else if (tag == "[damage action 1]")  { ScanStr(&script->m_actionSet.m_damage1); }
        else if (tag == "[damage action 2]")  { ScanStr(&script->m_actionSet.m_damage2); }
        else if (tag == "[down action]")      { ScanStr(&script->m_actionSet.m_down); }
        else if (tag == "[overturn action]")  { ScanStr(&script->m_actionSet.m_overturn); }
        else if (tag == "[jump action]")      { ScanStr(&script->m_actionSet.m_jump); }
        else if (tag == "[jumpattack action]"){ ScanStr(&script->m_actionSet.m_jumpattack); }
        else if (tag == "[attack action]" || tag == "[action list]")
        {
            script->m_actionSet.m_attackList.clear();
            std::string line;
            while (ScanStr(&line))
                script->m_actionSet.m_attackList.push_back(line);
        }
        else if (tag == "[etc action]" || tag == "[custom action]")
        {
            script->m_actionSet.m_etc.clear();
            std::string line;
            while (ScanStr(&line))
                script->m_actionSet.m_etc.push_back(line);
        }
        else if (tag == "[last action]")  { ScanStr(&script->m_lastAction); }
        else if (tag == "[proc action]")  { ScanStr(&script->m_procAction); }
        else if (tag == "[add gravity in aerial]")
        {
            script->m_addGravityInAerial = (ScanInt((bool*)0) != 0);
            script->m_gravityAerialA = ScanFloat((bool*)0);
            script->m_gravityAerialB = ScanFloat((bool*)0);
        }
        else if (tag == "[speech on situation]")
        {
            std::string line;
            for (int i = 0; i < 111; ++i)
                script->m_speeches[i].clear();           // 24*i + 0x300
            script->m_onAttackSpeech.clear();            // +0xd68
            script->m_onKeySpeech.clear();               // +0xd74
            while (true)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, tok, true))
                    break;                               // 段结束（esi=1）
                if (isType && tok == "[/speech on situation]")
                    break;
                ScanStr(&line);
                const int prob = ScanInt((bool*)0);
                const bool isAttack = (tok == "[on attack]");
                const bool isKey = (tok == "[on key]");
                if (!isAttack && !isKey)
                {
                    int slot = -1;
                    if (tok == "[on damage]")      slot = 0x65;
                    else if (tok == "[on die]")    slot = 0x66;
                    else if (tok == "[on revenge]") slot = 0x67;
                    else if (tok == "[on etc]")    slot = 0x68;
                    else if (tok == "[on appear]") slot = 0x69;
                    else if (tok == "[on disappear]") slot = 0x6a;
                    else if (tok == "[on go to help]") slot = 0x6b;
                    else if (tok == "[on stand]")  slot = 0x6c;
                    else
                        return false;                // ORIG: esi=0 → 返回失败
                    if ((unsigned)slot <= 0x6e)      // cmpl $0x6e; ja skip
                        script->m_speeches[slot].add(line, prob);
                }
                else
                {
                    // ORIG ScanFloatStr：kind==0 数值桶 / kind==1 在 m_keyFiles 找名字；
                    // fistpl 前 fldcw 切截断模式 → C 风格 (int) 转换同语义。
                    float fv = 0.0f;
                    int slot = -1;
                    const int kind = ScanFloatStr(fv, tok);
                    if (kind == 0)
                        slot = (int)fv;
                    else if (kind == 1)
                    {
                        for (std::size_t i = 0; i < script->m_keyFiles.size(); ++i)
                        {
                            if (script->m_keyFiles[i] == tok)
                            {
                                slot = (int)i;
                                break;
                            }
                        }
                    }
                    std::vector<APCSpeech>& bucket =
                        isAttack ? script->m_onAttackSpeech : script->m_onKeySpeech;
                    if (slot >= 0 && bucket.size() < (std::size_t)slot + 1)
                    {
                        APCSpeech filler;
                        bucket.resize((std::size_t)slot + 1, filler);
                    }
                    if (slot >= 0)
                        bucket[(std::size_t)slot].add(line, prob);
                }
            }
        }
        else if (tag == "[common champion elemental property]")
        {
            script->m_championElements.clear();
            while (ScanStr(&tok))
            {
                for (int e = 0; e < 5; ++e)
                {
                    if (tok == getElementName((ENUM_ELEMENT)e))
                    {
                        script->m_championElements.push_back(e);
                        break;
                    }
                }
            }
        }
        else if (tag == "[party member]")
        {
            script->m_partyMembers.clear();
            while (true)
            {
                const int a = ScanInt(&ok);
                if (!ok)
                    break;
                const int b = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_partyMembers.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (tag == "[far attack reaction rate]")
        {
            const float f = ScanFloat(&ok);
            // ORIG flds [_DAT_08e0a1e0]=100.0f; fmulp; fldcw(截断); fistp
            script->m_farAttackReactionRate = (int)(f * 100.0f);
        }
        else if (tag == "[pvp win point]")
        {
            script->m_pvpWinPointLow = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_pvpWinPointHigh = ScanInt(&ok);
            if (!ok)
                return false;
            if (script->m_pvpWinPointHigh < script->m_pvpWinPointLow)
                return false;
        }
        else if (tag == "[character status rate]" ||
                 tag == "[character status rate special]")
        {
            STRDCharacterStatusRate& r = (tag == "[character status rate]")
                ? script->m_statusRate : script->m_specialStatusRate;
            const bool special = (tag != "[character status rate]");
            for (int i = 0; i < 10; ++i) r.m_head[i] = ScanFloat((bool*)0);
            for (int k = 0; k <= 16; ++k) r.m_mid38[k] = ScanFloat((bool*)0);
            for (int i = 0; i < 10; ++i) r.m_tail[i] = ScanFloat((bool*)0);
            for (int t = 0; t <= 3; ++t)
                r.m_mid28[t] = ScanFloat((bool*)0);      // (t+0x370)*4+0x14 → Rate+0x28+4t
            for (int n = 0; ; ++n)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                // ORIG 无界连续追加 (n+0x374)*4+0x14 → Rate+0x38+4n 起；
                // 此处按数组边界裁剪以避免内存破坏（可观察行为：合法范围内一致）。
                if (n <= 16)
                    r.m_mid38[n] = f;
                (void)special;
            }
        }
        else if (tag == "[custom skill level]")
        {
            while (true)
            {
                STAICharacterCustomSkillLevel c;
                c.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                c.m_b = ScanInt((bool*)0);
                c.m_c = ScanInt((bool*)0);
                c.m_d = ScanInt((bool*)0);
                script->m_customSkills.push_back(c);
            }
        }
        else if (tag == "[attack damage rate]")
        {
            script->m_attackDamageRate = ScanFloat(&ok);
        }
        else if (tag == "[add equipment status from level]")
        {
            script->m_addEquipStatusFromLevel = ScanInt(&ok);
        }
        else if (tag == "[armor subtype]")
        {
            script->m_armorSubtype = ScanInt(&ok);
        }
        else if (tag == "[default equipment]")
        {
            script->m_defaultEquipment = ScanInt(&ok);
        }
        else if (tag == "[doll special dungeon]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_dollSpecialDungeons.push_back(v);
            }
        }
        else if (tag == "[attack damage rate special]")
        {
            script->m_attackDamageRateSpecial = ScanFloat(&ok);
        }
        else if (tag == "[attack stuck rate]")
        {
            // ORIG fstps 0x5f —— 绝对 +0x5f = STRDCharacterStatus(+0xc)+0x53 = m_field53
            *reinterpret_cast<float*>(&script->m_status.m_field53) = ScanFloat(&ok);
        }
        else if (tag == "[limit passiveobject]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_limitPassiveObjects.push_back(v);
            }
        }
        else if (tag == "[overhead gauge type]")
        {
            script->m_overheadGaugeType = ScanInt((bool*)0);
        }
        else if (tag == "[overhead gauge pos]")
        {
            script->m_overheadGaugePosX = ScanInt((bool*)0);
            script->m_overheadGaugePosY = ScanInt((bool*)0);
        }
        else if (tag == "[getup invincible]")
        {
            script->m_getupInvincible = ScanInt((bool*)0);
        }
        else if (tag == "[drop avatar item]")
        {
            for (int i = 0; i <= 3; ++i)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_rates08[i] = f;
            }
            for (int k = 0; k <= 4; ++k)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_rates18[k] = f;
            }
            script->m_avatarDrop.m_field0 = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_avatarDrop.m_field4 = ScanInt(&ok);
            if (!ok)
                return false;
            while (true)
            {
                stEnamyDropAvatarElement_t el;
                el.m_itemId = ScanInt(&ok);
                if (!ok)
                    break;
                el.m_total = ScanInt(&ok);
                if (!ok)
                    return false;
                el.m_flag = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_total += el.m_total;
                el.m_total = script->m_avatarDrop.m_total;   // 回填累计值
                script->m_avatarDrop.m_elements.push_back(el);
            }
        }
        else if (tag == "[skill data up]")
        {
            importSkillDataUp(script->m_skillDataUps);
        }
        else if (tag == "[summon monster status rate]")
        {
            while (true)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                script->m_summonStatusRates.push_back(f);
            }
        }
        else if (tag == "[no cooltime]")
        {
            script->m_noCooltime = 1;
        }
        else if (tag == "[check accountitem]")
        {
            script->m_checkAccountItem = (ScanInt((bool*)0) == 1);
        }
        else if (tag == "[hell monster]")
        {
            script->m_isHellMonster = (ScanInt((bool*)0) != 0);
        }
        else if (tag == "[animation job]")
        {
            const int v = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_animationJob = v;
        }
        else
        {
            if (!importAIInfoScript(&script->m_info, &tag, false))
                return false;
        }

        if (byLevel)
            break;                       // ORIG do{}while(!param_3)
    }
    return true;
}

// ============================================================================
// CNPCDynamicInfo —— C1 W 0x0858191c（报告缺失 → objdump 转写）：
//   m_script = a; +0x04 = 3; +0x08 = 0x7fffffff; +0x0c = 0。
// 完整类型归 NPC 动态域；本 TU 仅需产生该 ctor 符号
// （CtorProducers_1.cpp 的 CNPCDynamicInfoManager::add 经 asm 标签引用）。
// ============================================================================
class CNPCScript;  // 仅持指针，前置声明足够

class CNPCDynamicInfo
{
public:
    CNPCDynamicInfo(const CNPCScript* script);   // ORIG W 0x0858191c

    const CNPCScript* m_script;  // +0x00（changeMood/giveGiftItem 等报告域使用）
    int m_moodState;             // +0x04（ctor 置 3）
    int m_field8;                // +0x08（ctor 置 0x7fffffff）
    int m_fieldc;                // +0x0c（ctor 置 0）
};

CNPCDynamicInfo::CNPCDynamicInfo(const CNPCScript* script)
    : m_script(script), m_moodState(3), m_field8(0x7fffffff), m_fieldc(0)
{
}

// ============================================================================
// PowerParameterScript::clear —— ORIG W 0x0837187a
// 报告缺失 → objdump 转写（字段名映射 CDataManager.h 权威声明）。
// 序列：+0x00..+0x14 六个 int 清零；+0x1c=0；vec20.clear；
//   +0x30/+0x34/+0x48/+0x4c/+0x50/+0x54=0；+0x58=0；+0x5c=5；+0x60=5；
//   vec70.clear；+0x7c..+0x98 八个 int 清零；vec38.clear；+0x44=0；
//   +0x64/+0x68=0；+0xa8=0；vecAC.clear；+0xb8=0；vecBC.clear；vecC8.clear；
//   +0xd4=0。【+0x18/+0x2c/+0x6c 与 +0x9c powerWarServerList 不触碰】（ORIG 固有）
// ============================================================================
void PowerParameterScript::clear()
{
    m_powerJoinCost = 0;               // +0x00
    m_powerJoinGuildLevel = 0;         // +0x04
    m_powerJoinQuest = 0;              // +0x08
    m_killPointToEndPowerWar = 0;      // +0x0c
    m_expReward = 0;                   // +0x10
    m_ghostTime = 0;                   // +0x14
    m_powerUpRatio = 0;                // +0x1c
    m_vec20.clear();                   // +0x20
    m_refreshProcessInfo = 0;          // +0x30
    m_abuserBanTime = 0;               // +0x34
    m_powerUpGeneratePeriod = 0;       // +0x48
    m_powerUpAssaultCount = 0;         // +0x4c
    m_powerUpAssaultVictoryRatio = 0;  // +0x50
    m_powerWarPointMultiple = 0;       // +0x54
    m_straightVictories = 0;           // +0x58
    m_pad5c = 5;                       // +0x5c（ORIG clear 默认 5）
    m_pad60 = 5;                       // +0x60（ORIG clear 默认 5）
    m_vec70.clear();                   // +0x70
    m_basicPowerWarPoint = 0;          // +0x7c
    m_firstGradeBonusPoint = 0;        // +0x80
    m_rankingBonusGrade = 0;           // +0x84
    m_rankingBonusDecreasePoint = 0;   // +0x88
    m_decreasePointPerDay = 0;         // +0x8c
    m_requiredUserPoint4Dungeon = 0;   // +0x90
    m_powerWarDungeonInfo0 = 0;        // +0x94
    m_powerWarDungeonInfo1 = 0;        // +0x98
    m_vec38.clear();                   // +0x38
    m_pcRoomBonus = 0;                 // +0x44
    m_minEnterLevel = 0;               // +0x64
    m_minEnterAge = 0;                 // +0x68
    m_powerUpPointMod = 0;             // +0xa8
    m_vecAC.clear();                   // +0xac
    m_powerWarCooldownMinutes = 0;     // +0xb8
    m_vecBC.clear();                   // +0xbc
    m_vecC8.clear();                   // +0xc8
    m_victoriousRewardTime = 0;        // +0xd4
}

// ============================================================================
// CAICharacter —— C1 W 0x08373a9a / D1 W 0x08373b32
// 报告缺失 → objdump 转写。布局与 CAICharacterList.cpp 本地镜像 ABI 一致
// （sizeof 0x74：init 的 operator new $0x74 实证）。
// C1：+0x10 vector<pair<int,int>> ctor / +0x24 vector<stMonsterIndependentDrop_t>
//   ctor / +0x30 stEnamyDropAvatarInfo_t ctor / +0x6c std::string ctor。
// D1：逆序析构。其余 POD 字段不初始化（ORIG 同）。
// ============================================================================
class CAICharacter
{
public:
    CAICharacter();    // ORIG W 0x08373a9a
    ~CAICharacter();   // ORIG W 0x08373b32

    int m_idx;                                                    // +0x00
    int m_level;                                                  // +0x04
    int m_field8;                                                 // +0x08（getRarity 取值处）
    int m_appearancePoint;                                        // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;                // +0x10
    int m_pvpWinLow;                                              // +0x1c
    int m_pvpWinHigh;                                             // +0x20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops;   // +0x24
    stEnamyDropAvatarInfo_t m_avatarDrops;                        // +0x30（0x40）
    std::string m_name;                                           // +0x6c
    unsigned char m_rarity;                                       // +0x70（set 写入）
    char m_pad71[3];
};  // sizeof == 0x74
typedef char assert_cai_character_size[(sizeof(CAICharacter) == 0x74) ? 1 : -1];

CAICharacter::CAICharacter()
    : m_dropItems(), m_independentDrops(), m_avatarDrops(), m_name()
{
}

CAICharacter::~CAICharacter()
{
}
