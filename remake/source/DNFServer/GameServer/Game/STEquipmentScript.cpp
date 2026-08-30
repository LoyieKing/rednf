// ============================================================================
// df_game_r 还原 —— STEquipmentScript（G5 装备脚本表）
// 逐函数对照 docs/class_func_reports/：
//   STEquipmentScript/            ctor 0x0898db82 / dtor 0x0898e298 /
//                                 Clear 0x0898e9e4 / addAniInfo 0x0898efa8 /
//                                 getAniInfo 0x0898ef1a / isUpgradableLevel
//                                 0x0898efc8 / isExistUpgradableLevel
//                                 0x0898f066
//   STEquipmentScript__artifact_info/  ctor 0x0898f084 / isUsable 0x0898f0fe
//   EquipmentAniInfoScript/       ctor 0x0898d03e / clear 0x0898d142
//   _lt_global_gt_/ImportEquipmentScript.md  T 0x08991aeb（9821 insn）
// 支撑类型生命周期（ORIG 同编译单元段）：
//   STEquipmentAniScript::C1/clear   T 0x0898ca36 / 0x0898ce62
//   stAvatarTypeSelect_t::init       T 0x0898d1fc
//   GrowthEquipmentInfo::C1/clear    W 0x089c2fc4 / 0x089c300c
//   Position::Position               W 0x089c2c42
//   effect::STEffect::clear          W 0x089c2c64
// ============================================================================

#include "STEquipmentScript.h"
#include "STSkillScript.h"

#include <string.h>

#include "CCubeStatistic.h"    // getItemGroupNameTable()（ORIG 0x088c0409）
#include "DNFLexWrapper.h"

// ---- 跨 TU 符号（ORIG 真实符号；定义归各自属主批次，此处仅按 ABI 引用） ----
#include "SyncScriptDeps.h"    // toTString（importCommand 键名转换）
#include "CEnvironment.h"      // SpinLock / less_TCHAR（GetExpertJobTypeList）
#include "NpcScript.h"

// ---- 跨 TU 符号（InitScriptProducers_5.cpp 提供） ----
void InitMapEquipmentAniScript();  // ORIG T 0x089c0586
extern std::map<std::string, STEquipmentAniScript>* g_mapEquipmentAniScript;  // B 0x094f7bf8
// ORIG 全局脚本文件列表（权威视图同 NpcScript.h：pad 0x18 + 查找方法）

extern STScriptFileList g_stEquipmentScriptFileList;        // ORIG B 0x94fac00

int GetEquipmentType(const char* name);                     // T 0x08990f1f
int getAvatarEmblemType(const char* name);                  // T 0x08990f66
int scanModuleType();                                       // T 0x088c0b9a
int getModuleType(const char* name);                        // T 0x088c0bfc
int getCalculationType(const char* name);                   // T 0x088c227b
int scanWorldType();                                        // T 0x088c1430
bool importEffect(effect::STEffect& effect);                // T 0x088c1a0d
bool importCommand(STKeyCommand* command);                  // T 0x088c167a
static bool GetItemCategory(STItemScript* item);                   // t 0x089bfdc5
static bool GetAuraAvatarOption(STEquipmentScript* script);        // t 0x089c019b
static bool GetExpertJobTypeList(STItemScript* item);       // t 0x089bf6ca
static bool GetExpertJobValueMaterialVariation(STItemScript* it);  // t 0x089bfc2f
static bool GetExpertJobValueCompoundRate(STItemScript* item);     // t 0x089bfc84
static bool GetExpertJobResultVariation(STItemScript* item);       // t 0x089bfcb5
static bool GetExpertJobDisjointBigWinRate(STItemScript* item);    // t 0x089bfd0b
static bool GetExpertJobDisjointResultVariation(STItemScript* i);  // t 0x089bfd3c
static bool GetExpertJobAdditionalGainExp(STItemScript* item);     // t 0x089bfd6d
int getCharacterJob(const char* name);                      // T 0x088be57e
int getCharacterJobToEnumValue(std::string& job);           // T 0x089bbc0f
int getEquipmentTypeToEnumValue(std::string& type);         // T 0x089bc76e
int loadAppendageType(const std::string& token);            // T 0x0889253b
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
                         int type, bool flag);                      // T 0x08a97637
std::map<std::string, STEquipmentAniScript>* getMapEquipmentAniScript();  // T 0x089c05dd
void setEquipmentAniScriptLay(
    const std::map<std::string, STEquipmentAniScript>* table,
    STEquipmentAniScript* ani, std::string* key, int layerId,
    std::string* path, STEquipmentScript* script,
    ENUM_CHARACTERJOB job, equipmentAniType::T aniType);    // T 0x089c1cb9
bool importEquipmentParameterInfo(EquipmentParameterInfo* param,
                                  std::string& token, const char* path,
                                  int& countA, int& countB);  // T 0x089a75ea

namespace
{
const char* kEquipmentScriptBaseDir = "Script/EquipmentScript";

// motion 类标签公共体（[waiting motion]..[back motion]，ORIG 各 case 同构）：
// ScanStr 读格式串，按动画上下文取路径参数做 snprintf。产物（asStack_12xx/13xx
// 暂存）在本函数内无任何消费点——服务器侧解析后即弃，仅保留 token 消费与格式化。
void ScanMotionString(char* buf, unsigned int size, bool aniMode, int levelAniIdx,
                      const std::string& normalPath, const std::string& levelPath,
                      const std::string& jobPath)
{
    std::string fmt;
    ScanStr(&fmt);
    const char* arg = aniMode ? jobPath.c_str()
                        : (levelAniIdx == -1 ? normalPath.c_str()
                                             : levelPath.c_str());
    snprintf(buf, size, fmt.c_str(), arg);
}

}  // namespace

STEquipmentStatInfo::STEquipmentStatInfo()
{
    clear();
}

void STEquipmentStatInfo::clear()  // ORIG 0x0898d272：10 直接 + 循环 i<=3(+0x28)
                                   // + +0x38 + 循环 j<=16(+0x3c) + +0x80/+0x84
                                   // + 循环 k<=3(+0x88)；38 int 全清，sizeof 0x98
{
    m_hpMax = 0;
    m_mpMax = 0;
    m_phyAtt = 0;
    m_phyDef = 0;
    m_magAtt = 0;
    m_magDef = 0;
    m_equipPhyAtt = 0;
    m_equipPhyDef = 0;
    m_equipMagAtt = 0;
    m_equipMagDef = 0;
    for (int i = 0; i <= 3; ++i)
    {
        (&m_refFire)[i] = 0;        // +0x28..+0x34 元素耐性
    }
    m_refAll = 0;                   // +0x38
    for (int j = 0; j <= 16; ++j)
    {
        (&m_refSlow)[j] = 0;        // +0x3c..+0x7c 状态耐性
    }
    m_refAllState = 0;              // +0x80
    m_84 = 0;                       // +0x84
    for (int k = 0; k <= 3; ++k)
    {
        (&m_88)[k] = 0;             // +0x88..+0x94
    }
}

STEquipmentAniScript::STEquipmentAniScript()
{
    clear();
}

void STEquipmentAniScript::clear()  // ORIG 0x0898ce62（m_field0=0，各 string 置 ""）
{
    m_field0 = 0;
    m_str04.clear();
    m_str08.clear();
    m_str0c.clear();
    m_str10.clear();
    m_str14.clear();
    m_str18.clear();
    m_str1c.clear();
    m_str20.clear();
    m_str24.clear();
    m_str28.clear();
    m_str2c.clear();
    m_str30.clear();
    m_str34.clear();
    m_str38.clear();
    m_str3c.clear();
    m_str40.clear();
    m_str44.clear();
    m_str48.clear();
    m_str4c.clear();
    m_str50.clear();
    m_str54.clear();
    m_str58.clear();
    // ORIG 0x0898ce62 未清 +0x5c（23 个 string 逐一 =""，缺 +0x5c；拷贝构造/operator= 均
    // 拷贝 +0x5c，故 +0x5c 是 string 成员，仅 clear 漏清——按 ORIG 实证保留不清）
    m_str60.clear();
    m_vec64.clear();
    m_vec70.clear();
}

void stAvatarTypeSelect_t::init()  // ORIG 0x0898d1fc
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    for (int i = 0; i <= 4; ++i)
    {
        m_socketType[i] = 0;
    }
}

GrowthEquipmentInfo::GrowthEquipmentInfo()
{
    // ORIG 0x089c2fc4：仅成员 string 构造 + 调 clear()，无字符串赋值。
    clear();
}

void GrowthEquipmentInfo::clear()  // ORIG 0x089c300c
{
    m_level = 0;
    m_type = -1;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
    m_str14.clear();
}

EquipmentAniInfoScript::EquipmentAniInfoScript()
{
    clear();
}

void EquipmentAniInfoScript::clear()  // ORIG 0x0898d142
{
    m_ani0.clear();
    m_c = -1;
    m_10 = -1;
    m_ani18.clear();
    m_24 = -1;
    m_28 = 1;
    m_2c = 0;
    m_30 = -1;
    m_34 = -1;
    m_path4c.clear();
    m_38 = 0;
    m_3c = 0;
    m_wav40.clear();
    m_expand14 = 0;
    m_growth50.clear();
    m_level5c.clear();
}

Position::Position()
{
    m_x = 0;
    m_y = 0;
    m_z = 0;
}

effect::STEffect::STEffect()
{
    clear();
}

void effect::STEffect::clear()  // ORIG 0x089c2c64
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
    m_4 = 0;
    m_8 = 0;
    m_c_ = 0;
    m_10 = 0x14;
    m_str14.clear();
}

// ===================== STEquipmentScript 本体 =====================

STEquipmentScript::STEquipmentScript()
{
    Clear();
}

STEquipmentScript::~STEquipmentScript()
{
    if (m_keyCommand)
    {
        delete m_keyCommand;
    }
    m_keyCommand = 0;
}

void STEquipmentScript::Clear()  // ORIG 0x0898e9e4（顺序保持汇编原序）
{
    STItemScript::Clear();
    m_6bc = 0;
    m_764 = 0;
    m_str6a4 = "";
    m_str6ac = "";
    m_str6b8 = "";
    m_str668 = "";
    m_178 = -1;
    m_17c = 0x46;
    m_180 = -1;
    m_184 = -1;
    m_188 = -1;
    m_str1a4 = "";
    m_vec1a8.clear();
    m_field1b4 = 0x1a;
    m_1b8 = 0;
    m_1bc = 0;
    m_param1c0.clear();
    m_aniInfo640.clear();
    m_vec658.clear();
    m_str664 = "";
    m_vec66c.clear();
    m_paramMap678.clear();
    m_6a0 = -1;
    m_equipTypes768.clear();
    m_vec774.clear();
    m_vec780.clear();
    m_78c = 0;
    m_794 = 0;
    m_790 = 0;
    m_798 = -1;
    m_79c = -1;
    m_vec7a0.clear();
    m_7ac = 0;
    m_vec7b0.clear();
    m_avatar6c0.clear();
    m_vec6cc.clear();
    m_6d8 = -1;
    m_6dc = -1;
    m_6e4 = -1;
    m_7bc = 0;
    m_7c0 = -1;
    m_7c4 = -1;
    m_7c8 = 0;
    m_6f4 = 0;
    m_6f5 = 0;
    m_6f6 = 0;
    m_fieldd4 = 0;                 // ORIG 原序写基类区 +0xd4
    m_6f8 = 0;
    m_vec7d8.clear();
    m_vec7e4.clear();
    m_str7f0.clear();
    m_enchant7cc.clear();
    m_str7f4.clear();
    m_keyCommand = 0;
    m_808 = 3;
    m_80c = 0;
    m_80e = 0;
    m_810 = 0;
    m_paramVec690.clear();
    m_69c = 0;
    m_814 = 2;
    m_818 = 0;
    m_effect84c.clear();
    m_set714.clear();
    m_set714.insert(0x14);
    m_72c = 0;
    m_730 = 0;
    m_734 = 0;
    m_6e0 = 0;
    m_vec858.clear();
    m_vec864.clear();
    m_str878.clear();
    m_87c = 0;
    m_880 = -1;
    m_884 = 0;
    m_888 = 0;
    m_889 = 1;
    m_82c = 0;
    m_vec830.clear();
    m_str83c = "";
    m_setItem840.clear();
    m_88a = 0;
    m_88b = 0;
    m_890 = 0;
    m_88c = 0;
    m_72d = 0;
    m_82c = 0;                     // ORIG 尾部对 82c/830/83c/840 重复清一遍
    m_vec830.clear();
    m_str83c = "";
    m_setItem840.clear();
}

EquipmentAniInfoScript& STEquipmentScript::addAniInfo(ENUM_CHARACTERJOB job)  // 0x0898efa8
{
    return m_aniInfo640[job];
}

EquipmentAniInfoScript* STEquipmentScript::getAniInfo(ENUM_CHARACTERJOB job)  // 0x0898ef1a
{
    std::map<ENUM_CHARACTERJOB, EquipmentAniInfoScript>::iterator it =
        m_aniInfo640.find(job);
    if (it != m_aniInfo640.end())
    {
        return &it->second;
    }
    if (job == 0xb)
    {
        return 0;                  // ORIG 返回 NULL
    }
    return getAniInfo((ENUM_CHARACTERJOB)0xb);
}

bool STEquipmentScript::isUpgradableLevel(ItemUpgradeType::T type, int level)  // 0x0898efc8
{
    std::map<ItemUpgradeType::T, std::pair<int, int> >::iterator it =
        m_upgrade6fc.find(type);
    if (it != m_upgrade6fc.end())
    {
        if (level < it->second.first || it->second.second < level)
        {
            return false;
        }
    }
    return true;
}

bool STEquipmentScript::isExistUpgradableLevel()  // 0x0898f066
{
    return m_upgrade6fc.size() != 0;
}

STEquipmentScript::artifact_info::artifact_info()  // 0x0898f084
{
    m_minLevel = 0;
    m_physAttack = 0;
    m_magAttack = 0;
    m_mpRate = 0.0f;
    m_chargeRate = 0.0f;
    m_overChargeRate = 0.0f;
    m_expRate = 0.0f;
    m_physCrit = 0.0f;
    m_magCrit = 0.0f;
    m_stuck = 0.0f;
    m_piercing = 0;
}

bool STEquipmentScript::artifact_info::isUsable() const  // 0x0898f0fe
{
    if (m_minLevel == 0 && m_physAttack == 0 && m_magAttack == 0 &&
        m_mpRate == 0.0f && m_chargeRate == 0.0f && m_overChargeRate == 0.0f &&
        m_expRate == 0.0f && m_physCrit == 0.0f && m_magCrit == 0.0f &&
        m_stuck == 0.0f && m_piercing == 0)
    {
        return false;
    }
    return true;
}

// ===================== 脚本导入（T 0x08991aeb，9821 insn） =====================
// ORIG 用 static map<char const*,int> 查找表 + SpinLock 双检锁把关键字分派为三段
// switch（主属性 local_98 / 动画扩展 local_70 / 升级子键 local_3c）；未命中依次落
// 到 importEquipmentParameterInfo（参数区解析器）与 processModuleTagSet。
// 此处按同文件其它 ST*Script 导入惯例以字符串比较实现等价分派（见 ImportMapScript）。
//
// 控制流实证（汇编 0x08991d82/0x0899c9bb/0x0899c9e2/0x0899c9f1）：
//   while (ScanType(token)) { 分派…; if (stopFlag) break; } → 收尾 return true；
//   ScanType 失败（EOF）同样跳收尾（jne 0x0899c9f1→0x0899c9f2），并非失败路径；
//   失败路径：参数解析器 false 且双计数 <0 → return 0（不 unload），以及各 case
//   内显式失败（unaff_EBX=0 → cleanup return 0，不 unload）。
//
// 标签→body 对齐方法：以 ORIG switch 路由表（cmp id; je LAB）为准取各 LAB 的
// inited 分支体；关键样本已用 objdump 逐块复核（[weight]/[icon]/[attack info]/
// [hide layer] 族/[usable period] 等）。

bool ImportEquipmentScript(STEquipmentScript* script, const char* path,
                           bool flag, int a, int idx)
{
    std::string token;             // local_1074
    std::string tmp;               // local_1070
    std::string fullPath;          // local_1078
    std::string scanBuf;           // local_107c（各 case 的第二读取暂存）
    bool okFlag = false;           // 各 case 的 ScanInt(bool*) 成功标志
    char printBuf[0x200];          // local_1554

    // 动画三视图暂存（local_125c/local_12d8/local_1354；首成员非 0 视为待提交）
    STEquipmentAniScript normalLayer;
    STEquipmentAniScript jobLayer;
    STEquipmentAniScript levelLayer;
    GrowthEquipmentInfo growth;         // local_10b4
    std::string normalPath;             // local_1088（""）
    std::string jobPath;                // local_109c（""）
    std::string levelPath;              // local_10bc（""）

    bool stopFlag = false;              // local_9a
    int curJob = 0xb;                   // local_b0（0xb = 全职业缺省）
    bool aniMode = false;               // local_a9（[expand ani]..[/expand ani] 区）
    int levelAniIdx = -1;               // local_10a0（level section 状态）
    int normalLayerId = 0;              // local_b4
    int levelLayerId = 0;               // local_a4
    int jobLayerId = 0;                 // local_a8
    int upgMin = -1;                    // local_1164
    int upgMax = -1;                    // local_1168
    int upgType = 5;                    // local_116c
    int cnt[4];                         // local_1098（参数解析器计数输出）
    cnt[0] = 0;
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0;
    EquipmentParameterInfo* curParam = &script->m_param1c0;  // local_a0
    std::vector<std::pair<int, int> > setAbilityBackup;      // local_10c8

    if (!loadRDARScriptFile(kEquipmentScriptBaseDir, path))
    {
        return false;
    }
    script->Clear();
    getDirectoryPath(fullPath, "%s%s", kEquipmentScriptBaseDir, path);
    script->m_str664 = fullPath;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                       // EOF → 收尾（ORIG jne 0x0899c9f1）
        }

        // ---------------- 第一段分派（主属性区，switch(local_98)） ----------------
        bool matched = true;
        if (token == "[name]")                                   // 0x67d
        {
            ScanStr(&script->m_strac);
            if (flag)
            {
                stopFlag = true;
            }
        }
        else if (token == "[name2]")                             // 0x682
        {
            ScanStr(&script->m_strb0);
        }
        else if (token == "[explain]")                           // 0x685
        {
            ScanStr(&script->m_strb4);
        }
        else if (token == "[explain2]")                          // 0x689
        {
            ScanStr(&script->m_strb4);
        }
        else if (token == "[basic explain]")                     // 0x68d
        {
            ScanStr(&script->m_str6a4);
        }
        else if (token == "[detail explain]")                    // 0x690
        {
            ScanStr(&script->m_str6a8);
        }
        else if (token == "[parameter basic explain]")           // 0x693
        {
            ScanStr(&curParam->m_str278);
        }
        else if (token == "[parameter detail explain]")          // 0x696
        {
            ScanStr(&curParam->m_str27c);
        }
        else if (token == "[flavor text]")                       // 0x699
        {
            ScanStr(&script->m_strb8);
        }
        else if (token == "[attack info]")                       // 0x69c
        {
            ScanStr(&script->m_str6b8);
        }
        else if (token == "[fullset explain]")                   // 0x69f
        {
            ScanStr(&script->m_str6ac);
        }
        else if (token == "[fullset basic explain]")             // 0x6a2
        {
            ScanStr(&script->m_str6b0);
        }
        else if (token == "[fullset detail explain]")            // 0x6a5
        {
            ScanStr(&script->m_str6b4);
        }
        else if (token == "[set name]")                          // 0x6a8
        {
            ScanStr(&script->m_str668);
        }
        else if (token == "[grade]")                             // 0x6ab
        {
            ScanInt(&script->m_field4);
        }
        else if (token == "[spectrum]")                          // 0x6ae
        {
            unsigned int v1 = (unsigned int)ScanInt((bool*)0);
            unsigned int v2 = (unsigned int)ScanInt((bool*)0);
            unsigned int v3 = (unsigned int)ScanInt((bool*)0);
            int v4 = ScanInt((bool*)0);
            script->m_790 = (int)((v3 & 0xff) << 0x10 | v1 & 0xff |
                                  (v2 & 0xff) << 8 | v4 << 0x18);
            script->m_794 = ScanInt((bool*)0);
        }
        else if (token == "[clear avatar]")                      // 0x6b6
        {
            if (ScanInt((bool*)0) == 1)
            {
                script->m_7ac = 1;
            }
        }
        else if (token == "[attach type]")                       // 0x6bb
        {
            ScanStr(&tmp);
            if (tmp == "[free]")               script->m_field8 = 0;
            else if (tmp == "[trade]")         script->m_field8 = 1;
            else if (tmp == "[trade delete]")  script->m_field8 = 2;
            else if (tmp == "[sealing]")       script->m_field8 = 3;
            else if (tmp == "[sealing trade]") script->m_field8 = 4;
            else if (tmp == "[account]")       script->m_field8 = 5;
        }
        else if (token == "[creation rate]")                     // 0x6cc
        {
            ScanInt(&script->m_fieldc);
        }
        else if (token == "[rarity]")                            // 0x6cf
        {
            ScanInt(&script->m_field10);
        }
        else if (token == "[weight]")                            // 0x6d2
        {
            ScanInt(&script->m_field34);
        }
        else if (token == "[icon]")                              // 0x6d5
        {
            ScanStr(&script->m_str6c);
            ScanInt(&script->m_field70);
        }
        else if (token == "[aura hud icon]")                     // 0x6da
        {
            ScanInt(&script->m_188);
        }
        else if (token == "[tarotcard view]")                    // 0x6df
        {
            ScanStr(&script->m_str7c);
            ScanInt(&script->m_field80);
        }
        else if (token == "[extra icon]")                        // 0x6e5
        {
            ScanStr(&tmp);
            script->m_vec18c.push_back(tmp);
            script->m_vec198.push_back(ScanInt((bool*)0));
        }
        else if (token == "[field image]")                       // 0x6ea
        {
            ScanStr(&script->m_str74);
            ScanInt(&script->m_field78);
        }
        else if (token == "[field animation]")                   // 0x6ee
        {
            ScanStr(&script->m_str1a4);
        }
        else if (token == "[custom animation]")                  // 0x6f1
        {
            script->m_vec1a8.clear();
            while (ScanStr(&tmp))
            {
                script->m_vec1a8.push_back(tmp);
            }
        }
        else if (token == "[usable job]")                        // 0x71c
        {
            while (ScanStr(&tmp))
            {
                int job = getCharacterJob(tmp.c_str());
                if (job == 0xb)
                {
                    for (int i = 0; i < 0xb; ++i)
                    {
                        script->m_usable[i] = 1;
                    }
                    script->m_fielda0 = 1;
                }
                else
                {
                    script->m_usable[job] = 1;
                }
            }
        }
        else if (token == "[suitable job]")                      // 0x72a
        {
            script->m_set44.clear();
            while (ScanStr(&tmp))
            {
                int job = getCharacterJob(tmp.c_str());
                if (job == 0xb)
                {
                    for (int i = 0; i < 0xb; ++i)
                    {
                        script->m_set44.insert(i);
                    }
                }
                else
                {
                    script->m_set44.insert(job);
                }
            }
        }
        else if (token == "[max having count]")                  // 0x739
        {
            ScanInt(&script->m_field60);
        }
        else if (token == "[minimum level]")                     // 0x73e
        {
            ScanInt(&script->m_field5c);
        }
        else if (token == "[maximum level]")                     // 0x741
        {
            ScanInt(&script->m_17c);
        }
        else if (token == "[minimum rank]")                      // 0x744
        {
            ScanInt(&script->m_field64);
        }
        else if (token == "[control index]")                     // 0x747
        {
            ScanInt(&script->m_180);
        }
        else if (token == "[required skill]")                    // 0x74a
        {
            ScanInt(&script->m_184);
        }
        else if (token == "[cool time]")                         // 0x74d
        {
            ScanInt(&script->m_field68);
        }
        else if (token == "[equipment type]")                    // 0x750
        {
            ScanStr(&tmp);
            script->m_strbc = tmp;
            script->m_field1b4 = GetEquipmentType(tmp.c_str());
            ScanInt(&script->m_fielda4);
        }
        else if (token == "[avatar task num]")                   // 0x75a
        {
            ScanInt((bool*)0);
        }
        else if (token == "[weapon full avatar]")                // 0x762
        {
            script->m_72d = 1;
        }
        else if (token == "[hide equipment]")                    // 0x766
        {
            script->m_equipTypes768.clear();
            while (ScanStr(&tmp))
            {
                if (tmp == "[/hide equipment]")
                {
                    break;
                }
                int eqType = 0x16;
                if (tmp == "[hat avatar]")          eqType = 0;
                else if (tmp == "[hair avatar]")    eqType = 1;
                else if (tmp == "[face avatar]")    eqType = 2;
                else if (tmp == "[coat avatar]")    eqType = 3;
                else if (tmp == "[pants avatar]")   eqType = 4;
                else if (tmp == "[shoes avatar]")   eqType = 5;
                else if (tmp == "[breast avatar]")  eqType = 6;
                else if (tmp == "[waist avatar]")   eqType = 7;
                else if (tmp == "[skin avatar]")    eqType = 8;
                else if (tmp == "[aurora avatar]")  eqType = 9;
                else if (tmp == "[weapon]")         eqType = 10;
                else if (tmp == "[title name]")     eqType = 0xb;
                if (eqType != 0x16)
                {
                    script->m_equipTypes768.push_back((ENUM_EQUIPMENTTYPE)eqType);
                }
            }
        }
        else if (token == "[hide layer]")                        // 0x792
        {
            script->m_vec774.clear();
            for (;;)
            {
                okFlag = false;
                int v = ScanInt(&okFlag);
                if (!okFlag)
                {
                    break;
                }
                script->m_vec774.push_back(v);
            }
        }
        else if (token == "[hide growtype avatar]")              // 0x79e
        {
            script->m_vec780.clear();
            for (;;)
            {
                okFlag = false;
                int v = ScanInt(&okFlag);
                if (!okFlag)
                {
                    break;
                }
                script->m_vec780.push_back(v);
            }
        }
        else if (token == "[usable period]")                     // 0x7aa
        {
            ScanInt(&script->m_fieldf4);
        }
        else if (token == "[expiration date]")                   // 0x7ad
        {
            ScanInt(&script->m_fieldf8);
        }
        else if (token == "[usable even peace]")                 // 0x7bf
        {
            script->m_78c = (char)(ScanInt((bool*)0) == 1);
        }
        else if (token == "[sub type]")                          // 0x7c3
        {
            if (!aniMode)
            {
                if (levelAniIdx == -1)
                {
                    ScanInt(&script->m_1b8);
                }
                else
                {
                    jobLayerId = ScanInt((bool*)0);
                }
            }
            else
            {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                info.m_3c = ScanInt((bool*)0);
            }
        }
        else if (token == "[durability]")                        // 0x7d3
        {
            ScanInt(&script->m_1bc);
        }
        else if (token == "[creature species]")                  // 0x7d6
        {
            ScanInt(&script->m_6bc);
        }
        else if (token == "[creature minimum level]")            // 0x7d9
        {
            ScanInt(&script->m_artifact738.m_minLevel);
        }
        else if (token == "[creature physical attack]")          // 0x7dc
        {
            ScanInt(&script->m_artifact738.m_physAttack);
        }
        else if (token == "[creature magical attack]")           // 0x7df
        {
            ScanInt(&script->m_artifact738.m_magAttack);
        }
        else if (token == "[creature skill consume mp rate]")    // 0x7e2
        {
            script->m_artifact738.m_mpRate = ScanFloat(2, (bool*)0);
        }
        else if (token == "[creature skill charge time rate]")   // 0x7e5
        {
            script->m_artifact738.m_chargeRate = ScanFloat(2, (bool*)0);
        }
        else if (token == "[creature skill over charge time rate]")  // 0x7e8
        {
            script->m_artifact738.m_overChargeRate = ScanFloat(2, (bool*)0);
        }
        else if (token == "[creature experience amount rate]")   // 0x7eb
        {
            script->m_artifact738.m_expRate = ScanFloat(2, (bool*)0);
        }
        else if (token == "[creature physical critical hit]")    // 0x7ee
        {
            script->m_artifact738.m_physCrit = ScanFloat(1, (bool*)0);
        }
        else if (token == "[creature magical critical hit]")     // 0x7f1
        {
            script->m_artifact738.m_magCrit = ScanFloat(1, (bool*)0);
        }
        else if (token == "[creature stuck]")                    // 0x7f4
        {
            script->m_artifact738.m_stuck = ScanFloat(1, (bool*)0);
        }
        else if (token == "[creature piercing]")                 // 0x7f7
        {
            ScanInt(&script->m_artifact738.m_piercing);
        }
        else if (token == "[output index]")                      // 0x7ff
        {
            ScanInt(&script->m_764);
        }
        else if (token == "[price]")                             // 0x802
        {
            ScanInt(&script->m_field14);
        }
        else if (token == "[value]")                             // 0x805
        {
            ScanInt(&script->m_field24);
        }
        else if (token == "[repair price]")                      // 0x808
        {
            ScanInt(&script->m_178);
        }
        else if (token == "[finish point price]")                // 0x80b
        {
            ScanInt(&script->m_field20);
        }
        else if (token == "[cash]")                              // 0x80e
        {
            ScanInt(&script->m_field18);
        }
        else if (token == "[medal]")                             // 0x811
        {
            ScanInt(&script->m_field1c);
        }
        else if (token == "[usable upgrade chracter level]")     // 0x81a
        {
            script->m_field134 = (unsigned short)ScanInt((bool*)0);
        }
        else if (token == "[need material]")                     // 0x81e
        {
            ScanInt(&script->m_pair28.first);
            ScanInt(&script->m_pair28.second);
        }
        else if (token == "[recieve postal message]")            // 0x823
        {
            ScanStr(&script->m_str174);
        }
        else if (token == "[Uncompoundable]")                    // 0x828
        {
            okFlag = false;
            int v = ScanInt(&okFlag);
            if (okFlag && v == 1)
            {
                script->m_82c = 1;
            }
        }
        else if (token == "[remove avatar]")                     // 0x83d
        {
            for (;;)
            {
                if (!ScanStr(&scanBuf))
                {
                    break;
                }
                script->m_vec830.push_back(getModuleType(scanBuf.c_str()));
            }
        }
        else if (token == "[auction free]")                      // 0x845
        {
            script->m_field30 = 1;
        }
        else if (token == "[hidden option]")                     // 0x849
        {
            script->m_field31 = 1;
        }
        else
        {
            matched = false;
        }

        if (!matched)
        {
            // 未命中第一段：交给参数区解析器（ORIG code_r0x08994db2）。
            bool consumed;
            if (curParam == &script->m_param1c0)
            {
                consumed = importEquipmentParameterInfo(curParam, token, path,
                                                        cnt[3], cnt[2]);
            }
            else
            {
                consumed = importEquipmentParameterInfo(curParam, token, path,
                                                        cnt[1], cnt[0]);
            }
            if (!consumed && cnt[3] < 0 && cnt[2] < 0)
            {
                return false;            // ORIG fail-return（不 unload）
            }
            if (!consumed)
            {
                // ---------------- 第二段分派（动画/扩展区，switch(local_70)） ---
                matched = true;

            // ---- 动画区骨架 ----
            if (false) {}
            #define EQUIP_MOTION_BRANCH(tag) else if (token == tag) { \
                ScanMotionString(printBuf, sizeof(printBuf), aniMode, levelAniIdx, \
                                 normalPath, levelPath, jobPath); }

            if (token == "[expand ani]")                             // 0x867
            {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                aniMode = true;
                info.m_24 = ScanInt((bool*)0);
                info.m_28 = ScanInt((bool*)0);
                info.m_2c = ScanInt((bool*)0);
                info.m_38 = ScanInt((bool*)0);
                info.m_expand14 = 1;
            }
            else if (token == "[/expand ani]")                       // 0x870
            {
                aniMode = false;
            }
            else if (token == "[expand path]")                       // 0x873
            {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                ScanStr(&info.m_path4c);
            }
            else if (token == "[level section ani]")                 // 0x877
            {
                okFlag = false;
                if (ScanInt(&levelAniIdx) && ScanStr(&scanBuf))
                {
                    curJob = getCharacterJob(scanBuf.c_str());
                    growth.clear();
                    growth.m_level = levelAniIdx;
                    growth.m_type = script->m_field1b4;
                    script->m_69c = 1;
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    info.m_level5c.push_back(std::vector<STEquipmentAniScript>());
                }
            }
            else if (token == "[/level section ani]")                // 0x886
            {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                if (levelLayer.m_field0 != 0)
                {
                    if (!info.m_level5c.empty())
                    {
                        info.m_level5c.back().push_back(levelLayer);
                    }
                    levelLayer.clear();
                }
                info.m_growth50.push_back(growth);
                growth.clear();
                levelAniIdx = -1;
            }
            else if (token == "[level section path]")                // 0x897
            {
                ScanStr(&levelPath);
            }
            else if (token == "[layer variation]")                   // 0x89e
            {
                if (!aniMode)
                {
                    if (levelAniIdx == -1)
                    {
                        normalLayerId = ScanInt((bool*)0);
                        ScanStr(&normalPath);
                    }
                    else
                    {
                        okFlag = false;
                        if (ScanInt(&okFlag) && okFlag)
                        {
                            ScanStr(&scanBuf);
                            levelPath = scanBuf;
                        }
                        else
                        {
                            stopFlag = true;
                        }
                    }
                }
                else
                {
                    jobLayerId = ScanInt((bool*)0);
                    ScanStr(&jobPath);
                }
            }
            else if (token == "[equipment ani script]")              // 0x8bb
            {
                ScanStr(&scanBuf);
                std::map<std::string, STEquipmentAniScript>* table =
                    getMapEquipmentAniScript();
                if (!aniMode)
                {
                    if (levelAniIdx == -1)
                    {
                        setEquipmentAniScriptLay(table, &normalLayer, &scanBuf,
                                                 normalLayerId, &normalPath,
                                                 script,
                                                 (ENUM_CHARACTERJOB)curJob,
                                                 equipmentAniType::T_normal);
                    }
                    else
                    {
                        setEquipmentAniScriptLay(table, &levelLayer, &scanBuf,
                                                 levelLayerId, &levelPath, script,
                                                 (ENUM_CHARACTERJOB)curJob,
                                                 (equipmentAniType::T)2);
                    }
                }
                else
                {
                    setEquipmentAniScriptLay(table, &jobLayer, &scanBuf, jobLayerId,
                                             &jobPath, script,
                                             (ENUM_CHARACTERJOB)curJob,
                                             (equipmentAniType::T)1);
                }
            }
            else if (token == "[LAYER]")                             // 0x8da
            {
                if (!aniMode)
                {
                    if (levelAniIdx == -1)
                    {
                        if (normalLayer.m_field0 != 0)
                        {
                            script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                                .m_ani0.push_back(normalLayer);
                        }
                        normalLayer.clear();
                        if (normalLayerId == 0)
                        {
                            normalLayer.m_field0 = ScanInt((bool*)0);
                        }
                        else
                        {
                            ScanInt((bool*)0);
                            normalLayer.m_field0 = normalLayerId;
                        }
                    }
                    else
                    {
                        if (levelLayer.m_field0 != 0)
                        {
                            EquipmentAniInfoScript& info =
                                script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                            if (!info.m_level5c.empty())
                            {
                                info.m_level5c.back().push_back(levelLayer);
                            }
                        }
                        levelLayer.clear();
                        okFlag = false;
                        int scanned = ScanInt(&okFlag);
                        if (levelLayerId == 0)
                        {
                            levelLayer.m_field0 = scanned;
                        }
                        else
                        {
                            levelLayer.m_field0 = levelLayerId;
                        }
                    }
                }
                else
                {
                    if (jobLayer.m_field0 != 0)
                    {
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                            .m_ani18.push_back(jobLayer);
                    }
                    jobLayer.clear();
                    if (jobLayerId == 0)
                    {
                        jobLayer.m_field0 = ScanInt((bool*)0);
                    }
                    else
                    {
                        ScanInt((bool*)0);
                        jobLayer.m_field0 = jobLayerId;
                    }
                }
            }
            // ---- motion 族（单值格式化后即弃，见 ScanMotionString 注释） ----
            EQUIP_MOTION_BRANCH("[waiting motion]")                  // 0x925
            EQUIP_MOTION_BRANCH("[move motion]")                     // 0x939
            EQUIP_MOTION_BRANCH("[sit motion]")                      // 0x94d
            EQUIP_MOTION_BRANCH("[damage motion 1]")                 // 0x961
            EQUIP_MOTION_BRANCH("[damage motion 2]")                 // 0x975
            EQUIP_MOTION_BRANCH("[down motion]")                     // 0x989
            EQUIP_MOTION_BRANCH("[overturn motion]")                 // 0x99d
            EQUIP_MOTION_BRANCH("[jump motion]")                     // 0x9b1
            EQUIP_MOTION_BRANCH("[jumpattack motion]")               // 0x9c5
            EQUIP_MOTION_BRANCH("[rest motion]")                     // 0x9d9
            EQUIP_MOTION_BRANCH("[throw motion 1-1]")                // 0x9ed
            EQUIP_MOTION_BRANCH("[throw motion 1-2]")                // 0xa01
            EQUIP_MOTION_BRANCH("[throw motion 2-1]")                // 0xa15
            EQUIP_MOTION_BRANCH("[throw motion 2-2]")                // 0xa29
            EQUIP_MOTION_BRANCH("[throw motion 3-1]")                // 0xa3d
            EQUIP_MOTION_BRANCH("[throw motion 3-2]")                // 0xa51
            EQUIP_MOTION_BRANCH("[throw motion 4-1]")                // 0xa65
            EQUIP_MOTION_BRANCH("[throw motion 4-2]")                // 0xa79
            EQUIP_MOTION_BRANCH("[dash motion]")                     // 0xa8d
            EQUIP_MOTION_BRANCH("[dashattack motion]")               // 0xaa1
            EQUIP_MOTION_BRANCH("[getitem motion]")                  // 0xab5
            EQUIP_MOTION_BRANCH("[buff motion]")                     // 0xac9
            EQUIP_MOTION_BRANCH("[back motion]")                     // 0xadd
            else if (token == "[attack motion]" ||                   // 0xaf1/0xb16
                     token == "[etc motion]")
            {
                // 列表版 motion：循环格式化进局部容器（ORIG avStack，无消费点）。
                std::vector<std::string> sink;
                const char* arg = aniMode ? jobPath.c_str()
                                    : (levelAniIdx == -1 ? normalPath.c_str()
                                                         : levelPath.c_str());
                while (ScanStr(&scanBuf))
                {
                    snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), arg);
                    sink.push_back(std::string(printBuf));
                }
            }
            else if (token == "[move wav]")                          // 0xb3d
            {
                ScanStr(&script->m_str84);
            }
            else if (token == "[custom wav]")                        // 0xb40
            {
                if (!aniMode)
                {
                    script->m_vec658.clear();
                    while (ScanStr(&tmp))
                    {
                        script->m_vec658.push_back(tmp);
                    }
                }
                else
                {
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    info.m_wav40.clear();
                    while (ScanStr(&tmp))
                    {
                        info.m_wav40.push_back(tmp);
                    }
                }
            }
            else if (token == "[animation job]")                     // 0xb51
            {
                ScanStr(&scanBuf);
                if (normalLayer.m_field0 != 0)
                {
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                        .m_ani0.push_back(normalLayer);
                    normalLayer.clear();
                }
                if (jobLayer.m_field0 != 0)
                {
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                        .m_ani18.push_back(jobLayer);
                    jobLayer.clear();
                }
                if (levelLayer.m_field0 != 0)
                {
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    if (!info.m_level5c.empty())
                    {
                        info.m_level5c.back().push_back(levelLayer);
                    }
                    levelLayer.clear();
                }
                curJob = getCharacterJob(scanBuf.c_str());
            }
            else if (token == "[variation]")                         // 0xb6d
            {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                if (!aniMode)
                {
                    if (levelAniIdx == -1)
                    {
                        info.m_c = ScanInt((bool*)0);
                        info.m_10 = ScanInt((bool*)0);
                    }
                    else
                    {
                        ScanInt((bool*)0);
                        ScanInt((bool*)0);
                    }
                }
                else
                {
                    info.m_30 = ScanInt((bool*)0);
                    info.m_34 = ScanInt((bool*)0);
                }
            }
            else if (token == "[setback img]")                       // 0xb82
            {
                ScanStr(&script->m_str83c);
                for (;;)
                {
                    stSetItemBackImageInfo info;
                    info.m_a = 0;
                    info.m_b = 0;
                    info.m_c = 0;
                    okFlag = false;
                    info.m_a = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    info.m_b = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    info.m_c = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    script->m_setItem840.push_back(info);
                }
            }
            else if (token == "[/setback img]")                      // 0xb98
            {
                // 空 case（ORIG 仅建表项，无 body）
            }
            else if (token == "[set item]")                          // 0xb9b
            {
                script->m_vec66c.clear();
                for (;;)
                {
                    okFlag = false;
                    int v = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    script->m_vec66c.push_back(v);
                }
            }
            else if (token == "[piece set ability]")                 // 0xba8
            {
                okFlag = false;
                int key = ScanInt(&okFlag);
                std::map<int, EquipmentParameterInfo>& table = script->m_paramMap678;
                if (table.find(key) == table.end())
                {
                    table[key].clear();
                }
                curParam = &table[key];
                curParam->m_414 = 1;
                cnt[1] = 0;
                cnt[0] = 0;
            }
            else if (token == "[set ability]")                       // 0xbb5
            {
                std::map<int, EquipmentParameterInfo>& table = script->m_paramMap678;
                if (table.find(0) == table.end())
                {
                    table[0].clear();
                }
                curParam = &table[0];
                curParam->m_414 = 1;
                cnt[1] = 0;
                cnt[0] = 0;
            }
            else if (token == "[/set ability]")                      // 0xbc1
            {
                setAbilityBackup = curParam->m_vec468;
                curParam = &script->m_param1c0;
            }
            else if (token == "[/piece set ability]")                // 0xbc8
            {
                curParam = &script->m_param1c0;
            }
            else if (token == "[set item master]")                   // 0xbcb
            {
                ScanInt(&script->m_6a0);
            }
            else if (token == "[int data]")                          // 0xbce
            {
                script->m_vec88.clear();
                for (;;)
                {
                    okFlag = false;
                    int v = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    script->m_vec88.push_back(v);
                }
            }
            else if (token == "[string data]")                       // 0xbd9
            {
                script->m_vec94.clear();
                while (ScanStr(&tmp))
                {
                    script->m_vec94.push_back(tmp);
                }
            }
            else if (token == "[packagable]")                        // 0xbe0
            {
                script->m_fieldfc = (char)(ScanInt((bool*)0) != 0);
            }
            else if (token == "[icon mark]")                         // 0xbe3
            {
                ScanStr(&script->m_strc0);
                ScanInt(&script->m_fieldc4);
            }
            else if (token == "[part set index]")                    // 0xbf0
            {
                ScanInt(&script->m_798);
            }
            else if (token == "[effect part set index]")             // 0xbf4
            {
                ScanInt(&script->m_79c);
            }
            else if (token == "[reference effect part set index]")   // 0xbf7
            {
                script->m_vec7a0.push_back(ScanInt((bool*)0));
            }
            else if (token == "[image packs]")                       // 0xbfb
            {
                while (ScanStr(&tmp))
                {
                    script->m_vec7b0.push_back(tmp);
                }
            }
            else if (token == "[fixed select type]")                 // 0xc04
            {
                okFlag = false;
                script->m_6e0 = ScanInt(&okFlag);
            }
            else if (token == "[emblem socket default]")             // 0xc08
            {
                stAvatarTypeSelect_t head;
                head.init();
                head.m_field0 = 7;
                script->m_avatar6c0.push_back(head);
                for (;;)
                {
                    stAvatarTypeSelect_t sel;
                    sel.init();
                    okFlag = false;
                    sel.m_field0 = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    sel.m_field4 = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        return false;
                    }
                    sel.m_field8 = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        return false;
                    }
                    sel.m_fieldc = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        return false;
                    }
                    sel.m_field10 = ScanInt(&okFlag);
                    if (!okFlag || 5 < sel.m_field10)
                    {
                        return false;
                    }
                    if (0 < sel.m_field10)
                    {
                        if (5 <
                            (int)(script->m_vec6cc.size() + sel.m_field10))
                        {
                            return false;
                        }
                        for (int i = 0; i < sel.m_field10; ++i)
                        {
                            if (!ScanStr(&scanBuf))
                            {
                                return false;
                            }
                            sel.m_socketType[i] =
                                (unsigned short)getAvatarEmblemType(scanBuf.c_str());
                            if (sel.m_socketType[i] == 0)
                            {
                                return false;
                            }
                        }
                    }
                    script->m_avatar6c0.push_back(sel);
                }
            }
            else if (token == "[avatar type select]")                // 0xc16
            {
                okFlag = false;
                script->m_6d8 = ScanInt(&okFlag);
                if (!okFlag)
                {
                    return false;
                }
            }
            else if (token == "[avatar func filter]")                // 0xc55
            {
                okFlag = false;
                script->m_6d8 = ScanInt(&okFlag);
            }
            else if (token == "[emancipate]")                        // 0xc5a
            {
                for (;;)
                {
                    std::string key;
                    ScanType(key, true);
                    script->m_field10e = 1;
                    bool cont;
                    if (key == "[input]")
                    {
                        for (;;)
                        {
                            okFlag = false;
                            int first = ScanInt(&okFlag);
                            if (!okFlag)
                            {
                                break;
                            }
                            int second = ScanInt(&okFlag);
                            if (!okFlag)
                            {
                                break;
                            }
                            script->m_vec7d8.push_back(
                                std::make_pair(first, second));
                        }
                        cont = true;
                    }
                    else if (key == "[output]")
                    {
                        for (;;)
                        {
                            okFlag = false;
                            int first = ScanInt(&okFlag);
                            if (!okFlag)
                            {
                                break;
                            }
                            int second = ScanInt(&okFlag);
                            if (!okFlag)
                            {
                                break;
                            }
                            script->m_vec7e4.push_back(
                                std::make_pair(first, second));
                        }
                        cont = true;
                    }
                    else if (key == "[emancipate explain]")
                    {
                        ScanStr(&script->m_str7f0);
                        cont = true;
                    }
                    else
                    {
                        cont = false;
                    }
                    if (!cont)
                    {
                        break;
                    }
                }
            }
            else if (token == "[hardcoding cooltime]")               // 0xc8f
            {
                okFlag = false;
                script->m_6e4 = ScanInt(&okFlag);
            }
            else if (token == "[item group name]")                   // 0xc93
            {
                if (!ScanStr(&scanBuf))
                {
                    unloadRDARScriptFile();
                    return false;
                }
                std::map<std::string, int>& groupTable = getItemGroupNameTable();
                std::map<std::string, int>::iterator git =
                    groupTable.find(scanBuf);
                if (git == groupTable.end())
                {
                    unloadRDARScriptFile();
                    return false;
                }
                script->m_field100 = git->second;
            }
            else if (token == "[npc gift disallowance]")             // 0xc9d
            {
                okFlag = false;
                int v = ScanInt(&okFlag);
                script->m_field104 = (char)(v != 1);
                if (!okFlag)
                {
                    return false;
                }
            }
            else if (token == "[change status on change hp]")        // 0xca2
            {
                for (;;)
                {
                    std::string appendageName;
                    if (!ScanStr(&appendageName))
                    {
                        break;
                    }
                    if (appendageName == "[/change status on change hp]")
                    {
                        break;
                    }
                    int appendage = loadAppendageType(appendageName);
                    if (appendage != -1)
                    {
                        okFlag = true;
                        float r1 = (float)ScanInt(&okFlag);
                        float r2 = (float)ScanInt(&okFlag);
                        float r3 = (float)ScanInt(&okFlag);
                        float r4 = (float)ScanInt(&okFlag);
                        if (okFlag)
                        {
                            STChangeStatusRate rate;
                            rate.m_appendage = appendage;
                            rate.m_rate1 = r1;
                            rate.m_rate2 = r2;
                            rate.m_rate3 = r3;
                            rate.m_rate4 = r4;
                            script->m_rate6e8.push_back(rate);
                        }
                    }
                }
            }
            else if (token == "[epic routing]")                      // 0xcc1
            {
                okFlag = false;
                int v = ScanInt(&okFlag);
                script->m_7bc = (char)(v == 1);
                if (!okFlag)
                {
                    return false;
                }
            }
            else if (token == "[special ability]")                   // 0xcc6
            {
                okFlag = false;
                script->m_7c0 = ScanInt(&okFlag);
                if (!okFlag)
                {
                    return false;
                }
            }
            else if (token == "[chat emoticon index]")               // 0xcc9
            {
                okFlag = false;
                script->m_7c4 = ScanInt(&okFlag);
                if (!okFlag)
                {
                    return false;
                }
            }
            else if (token == "[required full set for chatting emoticon]")  // 0xccc
            {
                okFlag = false;
                int v = ScanInt(&okFlag);
                script->m_7c8 = (char)(v != 0);
                if (!okFlag)
                {
                    return false;
                }
            }
            else if (token == "[guild power war point]")             // 0xccf
            {
                ScanInt(&script->m_fieldc8);
            }
            else if (token == "[user power war point]")              // 0xcd2
            {
                ScanInt(&script->m_fieldcc);
            }
            else if (token == "[special monster drop]" ||            // 0xcd5/0xcd6
                     token == "[difficulty for extra drop]")
            {
                // 空 case（仅建表项）
            }
            else if (token == "[dungeon type for extra drop]")       // 0xcd7
            {
                if (!EquipmentScriptSpecialMonsterDrop(script,
                                                       std::string(token), path))
                {
                    return false;
                }
            }
            else if (token == "[character item check]")              // 0xcdd
            {
                for (;;)
                {
                    STEnchantSystemData entry;
                    entry.m_a = 0;
                    entry.m_b = 0;
                    entry.m_c = 0;
                    okFlag = false;
                    entry.m_a = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    entry.m_b = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    std::string equipTypeName;
                    if (!ScanStr(&equipTypeName))
                    {
                        return false;
                    }
                    entry.m_c = getEquipmentTypeToEnumValue(equipTypeName);
                    script->m_enchant7cc.push_back(entry);
                }
            }
            else if (token == "[hp recovery add rate]")              // 0xcf4
            {
                okFlag = false;
                ScanInt(&script->m_730);
            }
            else if (token == "[mp recovery add rate]")              // 0xcfc
            {
                okFlag = false;
                ScanInt(&script->m_734);
            }
            else if (token == "[impossible contents]")               // 0xd04
            {
                while (ScanStr(&scanBuf))
                {
                    if (scanBuf == "upgrade")
                    {
                        script->m_6f4 = 1;
                    }
                    else if (scanBuf == "amplify upgrade")
                    {
                        script->m_6f5 = 1;
                    }
                    else if (scanBuf == "disjoint")
                    {
                        script->m_6f6 = 1;
                    }
                    else if (scanBuf == "gift")
                    {
                        script->m_fieldd0 = 1;
                    }
                    else if (scanBuf == "trade")
                    {
                        script->m_fieldd1 = 1;
                    }
                    else if (scanBuf == "separate upgrade")
                    {
                        script->m_fieldd4 = 1;
                    }
                    else if (scanBuf == "charac cargo")
                    {
                        script->m_fieldda = 1;
                    }
                    else if (scanBuf == "ban redeemitem")
                    {
                        script->m_fieldd8 = 1;
                    }
                }
            }
            else if (token == "[possible kiri protect]")             // 0xd2d
            {
                script->m_fieldd9 = 1;
            }
            else if (token == "[limit upgradable level]")            // 0xd31
            {
                while (ScanStr(&scanBuf))
                {
                    upgMin = -1;
                    upgMax = -1;
                    ScanInt(&upgMin);
                    ScanInt(&upgMax);
                    upgType = 5;
                    if (scanBuf == "all upgrade")              upgType = 0;
                    else if (scanBuf == "normal upgrade")      upgType = 1;
                    else if (scanBuf == "amplify upgrade")     upgType = 2;
                    else if (scanBuf == "separate upgrade")    upgType = 3;
                    else if (scanBuf == "3rd chronicle upgrade") upgType = 4;
                    if (upgType != 5 && (upgMin != -1 || upgMax != -1))
                    {
                        if (upgMin == -1)
                        {
                            upgMin = 1;
                        }
                        if (upgMax == -1)
                        {
                            upgMax = 0x46;
                        }
                        if (upgType == 0)
                        {
                            for (int t = 1; t < 5; ++t)
                            {
                                std::pair<ItemUpgradeType::T,
                                          std::pair<int, int> > probe;
                                (void)probe;
                                if (script->m_upgrade6fc.find(
                                        (ItemUpgradeType::T)t) ==
                                    script->m_upgrade6fc.end())
                                {
                                    script->m_upgrade6fc.insert(std::make_pair(
                                        (ItemUpgradeType::T)t,
                                        std::make_pair(upgMin, upgMax)));
                                }
                            }
                        }
                        else if (script->m_upgrade6fc.find(
                                     (ItemUpgradeType::T)upgType) ==
                                 script->m_upgrade6fc.end())
                        {
                            script->m_upgrade6fc.insert(std::make_pair(
                                (ItemUpgradeType::T)upgType,
                                std::make_pair(upgMin, upgMax)));
                        }
                    }
                }
            }
            else if (token == "[particle]")                          // 0xd7d
            {
                ScanStr(&scanBuf);
                if (scanBuf == "same position")
                {
                    ScanStr(&script->m_str7f4);
                    ScanInt(&script->m_particle7f8.m_x);
                    ScanInt(&script->m_particle7f8.m_y);
                    ScanInt(&script->m_particle7f8.m_z);
                    ScanType(scanBuf, true);
                }
            }
            else if (token == "[command]")                           // 0xd89
            {
                script->m_keyCommand = new STKeyCommand();
                importCommand(script->m_keyCommand);
            }
            else if (token == "[character name y revision]")         // 0xd8d
            {
                script->m_80e = (unsigned short)ScanInt((bool*)0);
                script->m_80c = (unsigned short)ScanInt((bool*)0);
            }
            else if (token == "[routing priority]")                  // 0xd91
            {
                script->m_field10f = 1;
                std::string jobName;
                for (;;)
                {
                    if (!ScanStr(&jobName))
                    {
                        break;
                    }
                    if (jobName == "[/routing priority]")
                    {
                        break;
                    }
                    int jobId = getCharacterJobToEnumValue(jobName);
                    if (jobId == -1)
                    {
                        break;
                    }
                    okFlag = false;
                    int pri = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        break;
                    }
                    if (pri < 0 || 4 < pri)
                    {
                        return false;
                    }
                    script->m_vec110.push_back(std::make_pair(jobId, pri));
                }
            }
            else if (token == "[quest item drop rate bonus]")        // 0xdac
            {
                ScanInt(&script->m_810);
            }
            else if (token == "[level section ability]")             // 0xdb0
            {
                okFlag = false;
                if (ScanInt(&okFlag) && okFlag)
                {
                    EquipmentParameterInfo section;
                    script->m_paramVec690.push_back(section);
                    curParam = &script->m_paramVec690.back();
                }
                else
                {
                    stopFlag = true;
                }
            }
            else if (token == "[/level section ability]")            // 0xdbf
            {
                curParam = &script->m_param1c0;
            }
            else if (token == "[Force Result Item Rule]")            // 0xdc3
            {
                script->m_814 = (char)ScanInt((bool*)0);
                ScanInt(&script->m_818);
            }
            else if (token == "[effect]")                            // 0xdc7
            {
                effect::STEffect effectData;
                if (importEffect(effectData))
                {
                    script->m_effect84c.push_back(effectData);
                }
            }
            else if (token == "[usable module]")                     // 0xdcc
            {
                script->m_set714.clear();
                int module = scanModuleType();
                while (module != 0x11)
                {
                    script->m_set714.insert(module);
                    module = scanModuleType();
                }
            }
            else if (token == "[usable world]")                      // 0xdd5
            {
                script->m_setdc.clear();
                int world = scanWorldType();
                while (world != 2)
                {
                    script->m_setdc.insert(world);
                    world = scanWorldType();
                }
            }
            else if (token == "[no random]")                         // 0xdde
            {
                script->m_72c = 1;
            }
            else if (token == "[usable period after unsealing]")     // 0xde1
            {
                ScanInt(&script->m_6f8);
            }
            else if (token == "[aurora graphic effects]")            // 0xdeb
            {
                int total = ScanInt((bool*)0);
                for (int i = 0; i < total; ++i)
                {
                    int enabled = ScanInt((bool*)0);
                    script->m_vec864.push_back(enabled == 1);
                    ScanStr(&scanBuf);
                    script->m_vec858.push_back(scanBuf);
                }
            }
            else if (token == "[msg balloon on mucu skill]")         // 0xdf5
            {
                ScanInt(&script->m_87c);
                ScanStr(&script->m_str878);
            }
            else if (token == "[random option]")                     // 0xdf9
            {
                script->m_888 = 1;
                script->m_889 = (char)ScanInt((bool*)0);
            }
            else if (token == "[enable dye]")                        // 0xdfe
            {
                script->m_88a = (char)(ScanInt((bool*)0) != 0);
                script->m_88b = (char)(ScanInt((bool*)0) != 0);
            }
            else if (token == "[expertjob only]")                    // 0xe0a
            {
                if (!GetExpertJobTypeList(script))
                {
                    return false;
                }
            }
            else if (token == "[prof material variation]")           // 0xe0e
            {
                if (!GetExpertJobValueMaterialVariation(script))
                {
                    return false;
                }
            }
            else if (token == "[prof compound rate]")                // 0xe12
            {
                if (!GetExpertJobValueCompoundRate(script))
                {
                    return false;
                }
            }
            else if (token == "[prof result variation]")             // 0xe16
            {
                if (!GetExpertJobResultVariation(script))
                {
                    return false;
                }
            }
            else if (token == "[prof disjoint big win rate]")        // 0xe1a
            {
                if (!GetExpertJobDisjointBigWinRate(script))
                {
                    return false;
                }
            }
            else if (token == "[prof disjoint result variation]")    // 0xe1e
            {
                if (!GetExpertJobDisjointResultVariation(script))
                {
                    return false;
                }
            }
            else if (token == "[prof additional gain exp]")          // 0xe22
            {
                if (!GetExpertJobAdditionalGainExp(script))
                {
                    return false;
                }
            }
            else if (token == "[war room point bonus]")              // 0xe28
            {
                okFlag = false;
                if (ScanStr(&scanBuf))
                {
                    script->m_wpBonus81c.m_type = getCalculationType(scanBuf.c_str());
                    if (script->m_wpBonus81c.m_type == 2)
                    {
                        return false;
                    }
                    script->m_wpBonus81c.m_value = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        return false;
                    }
                }
            }
            else if (token == "[war room monster exp bonus]")        // 0xe37
            {
                okFlag = false;
                if (ScanStr(&scanBuf))
                {
                    script->m_expBonus824.m_type =
                        getCalculationType(scanBuf.c_str());
                    if (script->m_expBonus824.m_type == 2)
                    {
                        return false;
                    }
                    script->m_expBonus824.m_value = ScanInt(&okFlag);
                    if (!okFlag)
                    {
                        return false;
                    }
                }
            }
            else if (token == "[not amplify]")                       // 0xe48
            {
                ScanInt(&script->m_890);
            }
            else if (token == "[change type ultimateSkillCurScene]")  // 0xe4d
            {
                ScanInt(&script->m_6dc);
            }
            else if (token == "[item category]")                     // 0xe54
            {
                GetItemCategory(script);
            }
            else if (token == "[aura ability]")                      // 0xe57
            {
                GetAuraAvatarOption(script);
            }
            else if (token == "[/aura ability]")                     // 0xe5c
            {
                // 空 case
            }
            else
            {
                // 未命中第二段：模块标签处理（ORIG default LAB_0899c902）。
                int modOut[2];
                modOut[0] = -1;
                modOut[1] = -1;
                bool okMod = processModuleTagSet(modOut[1], modOut[0], &token,
                                                 a, true);
                if (modOut[0] != -1 && okMod)
                {
                    script->m_808 = modOut[0];
                }
            }
#undef EQUIP_MOTION_BRANCH
            }
        }
        if (stopFlag)
        {
            break;
        }
    }

    // ---------------- 收尾（ORIG LAB_0899c9f2） ----------------
    if (!setAbilityBackup.empty())
    {
        curParam->m_vec468 = setAbilityBackup;
    }
    if (normalLayer.m_field0 != 0)
    {
        script->addAniInfo((ENUM_CHARACTERJOB)curJob).m_ani0.push_back(normalLayer);
    }
    if (jobLayer.m_field0 != 0)
    {
        script->addAniInfo((ENUM_CHARACTERJOB)curJob).m_ani18.push_back(jobLayer);
    }
    if (levelLayer.m_field0 != 0)
    {
        EquipmentAniInfoScript& info =
            script->addAniInfo((ENUM_CHARACTERJOB)curJob);
        if (!info.m_level5c.empty())
        {
            info.m_level5c.back().push_back(levelLayer);
        }
    }
    if (script->m_178 == -1)
    {
        script->m_178 = script->m_field14;
        if (script->m_field24 != -1)
        {
            script->m_178 = script->m_field24;
        }
    }
    if (script->m_field24 == -1)
    {
        script->m_field24 = script->m_field14;
    }
    if (idx == -1)
    {
        script->m_field0 =
            (int)g_stEquipmentScriptFileList.FindIndexByFullScan(path);
    }
    else
    {
        script->m_field0 = idx;
    }
    unloadRDARScriptFile();
    return true;
}

bool EquipmentScriptSpecialMonsterDrop(STEquipmentScript* script,
                                       std::string str, const char* path)
{
    // ORIG 0x08991856（与 StackableScriptSpecialMonsterDrop 0x89ae906 同构，
    // 目标为 STEquipmentScript 继承自 STItemScript 的 m_specialDrop +0x105）。
    (void)path;
    STSpecialMonsterDrop& drop = script->m_specialDrop;
    if (str == "[special monster drop]")
    {
        bool value = false;
        bool ok = ScanInt(&value) != 0;
        if (value == false)
        {
            return false;
        }
        drop.m_enable = ok ? 1 : 0;
        return true;
    }
    if (str == "[difficulty for extra drop]")
    {
        std::string token;
        while (ScanStr(&token))
        {
            if (token == "[easy]")
            {
                drop.m_difficulty[0] = 1;
            }
            else if (token == "[medium]")
            {
                drop.m_difficulty[1] = 1;
            }
            else if (token == "[hard]")
            {
                drop.m_difficulty[2] = 1;
            }
            else if (token == "[ultimate]")
            {
                drop.m_difficulty[3] = 1;
            }
            else if (token == "[hero]")
            {
                drop.m_difficulty[4] = 1;
            }
            else if (token == "[all]")
            {
                memset(drop.m_difficulty, 1, sizeof(drop.m_difficulty));
            }
        }
        return true;
    }
    if (str == "[dungeon type for extra drop]")
    {
        std::string token;
        while (ScanStr(&token))
        {
            if (token == "[normal]")
            {
                drop.m_dungeonType[0] = 1;
            }
            else if (token == "[ancient]")
            {
                drop.m_dungeonType[1] = 1;
            }
            else if (token == "[dimension]")
            {
                drop.m_dungeonType[2] = 1;
            }
            else if (token == "[all]")
            {
                memset(drop.m_dungeonType, 1, sizeof(drop.m_dungeonType));
            }
        }
        return true;
    }
    return true;
}

bool importArtifactInfo(STEquipmentScript::artifact_info& info,
                        std::string str)
{
    // ORIG T 0x089bd079（0x7cb，报告全文转写）：函数级静态 lookup 表
    // （map<const char*,int,less_TCHAR> @0x94fdf28 + SpinLock @0x94fdf40 +
    // 就绪字节 @0x94fdf44）双检锁惰性建表；11 个 [creature *] 关键字 → id
    // （0x2ee9..0x2f07），未命中返回 0，命中按 id 分派 ScanInt/ScanFloat 写回。
    // 扫描参数：3 个 int 字段 ScanInt(0)；7 个 rate 字段 ScanFloat(scale,0)
    // ——mp/charge/overcharge/exp 用 scale=2，crit×2/stuck 用 scale=1。
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;

    if (!__inited)
    {
        __init_lock.enter();
        if (!__inited)
        {
            __lookup.insert(std::make_pair("[creature minimum level]", 0x2ee9));
            __lookup.insert(std::make_pair("[creature physical attack]", 0x2eec));
            __lookup.insert(std::make_pair("[creature magical attack]", 0x2eef));
            __lookup.insert(std::make_pair("[creature skill consume mp rate]", 0x2ef2));
            __lookup.insert(std::make_pair("[creature skill charge time rate]", 0x2ef5));
            __lookup.insert(std::make_pair("[creature skill over charge time rate]", 0x2ef8));
            __lookup.insert(std::make_pair("[creature experience amount rate]", 0x2efb));
            __lookup.insert(std::make_pair("[creature physical critical hit]", 0x2efe));
            __lookup.insert(std::make_pair("[creature magical critical hit]", 0x2f01));
            __lookup.insert(std::make_pair("[creature stuck]", 0x2f04));
            __lookup.insert(std::make_pair("[creature piercing]", 0x2f07));
            __inited = true;
        }
        __init_lock.leave();
    }

    std::map<const char*, int, less_TCHAR>::const_iterator it =
        __lookup.find(str.c_str());
    switch (it != __lookup.end() ? it->second : -1)
    {
    case 0x2ee9:                                       // [creature minimum level]
        info.m_minLevel = ScanInt((bool*)0);
        break;
    case 0x2eec:                                       // [creature physical attack]
        info.m_physAttack = ScanInt((bool*)0);
        break;
    case 0x2eef:                                       // [creature magical attack]
        info.m_magAttack = ScanInt((bool*)0);
        break;
    case 0x2ef2:                                       // [creature skill consume mp rate]
        info.m_mpRate = ScanFloat(2, (bool*)0);
        break;
    case 0x2ef5:                                       // [creature skill charge time rate]
        info.m_chargeRate = ScanFloat(2, (bool*)0);
        break;
    case 0x2ef8:                                       // [creature skill over charge time rate]
        info.m_overChargeRate = ScanFloat(2, (bool*)0);
        break;
    case 0x2efb:                                       // [creature experience amount rate]
        info.m_expRate = ScanFloat(2, (bool*)0);
        break;
    case 0x2efe:                                       // [creature physical critical hit]
        info.m_physCrit = ScanFloat(1, (bool*)0);
        break;
    case 0x2f01:                                       // [creature magical critical hit]
        info.m_magCrit = ScanFloat(1, (bool*)0);
        break;
    case 0x2f04:                                       // [creature stuck]
        info.m_stuck = ScanFloat(1, (bool*)0);
        break;
    case 0x2f07:                                       // [creature piercing]
        info.m_piercing = ScanInt((bool*)0);
        break;
    default:
        return false;                                  // ORIG 未命中 → 0
    }
    return true;
}

// ===================== Wave3Deps：脚本解析依赖组 =====================
// ORIG 同段（ImportEquipmentScript 解析器引用的文件级 helper）：

// ---- ORIG B 0x0949b348：键命令名称→索引表（importCommand 查询；填充方为
//      ORIG TU 内 static importKeyNameIndexTable，随脚本装载批次接入） ----
std::map<std::string, int> g_mapKeyNameIndexTable;

// ---- ORIG W 0x088c414c ----
void STKeyCommandUnit::clear()
{
    m_ids.clear();
    m_fieldC = 0;
    m_field10 = 0x12c;
}

// ---- ORIG W 0x088c4174（vector 自构造后 clear 置默认值） ----
STKeyCommandUnit::STKeyCommandUnit()
{
    clear();
}

// ---- ORIG T 0x089c05dd：惰性初始化的全局动画表指针 ----
std::map<std::string, STEquipmentAniScript>* getMapEquipmentAniScript()
{
    if (g_mapEquipmentAniScript == 0)
        InitMapEquipmentAniScript();
    return g_mapEquipmentAniScript;
}

// ---- ORIG t 0x089bfdc5：[item category] 标签序列 → item->m_set11c。
//      标签表惰性构建（ORIG 函数级静态 map @0x94fde28 + SpinLock @0x94fde40
//      + 就绪字节 @0x94fde44，双检锁形态）；未知标签令整个导入失败。 ----
static bool GetItemCategory(STItemScript* item)
{
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;

    std::string token;
    while (ScanStr(&token))
    {
        if (!__inited)
        {
            __init_lock.enter();
            if (!__inited)
            {
                // ORIG 首轮 case 链依次插入三项（字面量 .rodata 0x8e19dcd 起），
                // 随后重查当前 token；此处建表后直接落入同一 find，行为一致。
                __lookup.insert(std::make_pair("monster card", 0x30ae));
                __lookup.insert(std::make_pair("cannot bind", 0x30b1));
                __lookup.insert(std::make_pair("boss drop", 0x30b4));
                __inited = true;
            }
            __init_lock.leave();
        }

        int state = 0;
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(token.c_str());
        switch (it != __lookup.end() ? it->second : -1)
        {
        case 0x30ae:                                   // "monster card"
            state = STItemScript::ITEM_CATEGORY_1;
            break;
        case 0x30b1:                                   // "cannot bind"
            state = STItemScript::ITEM_CATEGORY_2;
            break;
        case 0x30b4:                                   // "boss drop"
            state = STItemScript::ITEM_CATEGORY_3;
            break;
        default:
            return false;                              // 未命中 → 导入失败
        }
        item->m_set11c.insert(
            static_cast<STItemScript::ENUM_ITEM_CATEGORY>(state));
    }
    return true;
}

// ---- ORIG t 0x089c019b：[aura ability] 单标签分派 → script->m_880 模式 +
//      m_884 参数。同型惰性标签表（map @0x94fde08 / SpinLock @0x94fde20 /
//      就绪字节 @0x94fde24）。ScanStr 返回值不检查——空 token 必然 miss，
static bool GetAuraAvatarOption(STEquipmentScript* script)
{
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;

    if (script == 0)
        return false;

    bool ok = true;
    std::string token;
    ScanStr(&token);

    if (!__inited)
    {
        __init_lock.enter();
        if (!__inited)
        {
            __lookup.insert(
                std::make_pair("[party teleport]", 0x30d0));
            __lookup.insert(std::make_pair("[none]", 0x30d5));
            __lookup.insert(std::make_pair("[solo teleport]", 0x30db));
            __inited = true;
        }
        __init_lock.leave();
    }

    std::map<const char*, int, less_TCHAR>::const_iterator it =
        __lookup.find(token.c_str());
    switch (it != __lookup.end() ? it->second : -1)
    {
    case 0x30d0:                                       // [party teleport]
        script->m_880 = 0;
        script->m_884 = ScanInt(&ok);
        break;
    case 0x30d5:                                       // [none]
        script->m_880 = 1;
        script->m_884 = 0;
        break;
    case 0x30db:                                       // [solo teleport]
        script->m_880 = 2;
        script->m_884 = ScanInt(&ok);
        break;
    default:
        ok = false;                                    // 未命中 → 导入失败
        break;
    }
    return ok;
}

// ---- ORIG T 0x08990f1f：[equipment type] 字符串→枚举（26 项，失配→0x1a；
//      表 0x93f4fc0 的 26 个字面量按序展开） ----
int GetEquipmentType(const char* name)
{
    static const char* const kNames[26] = {
        "[hat avatar]",   "[hair avatar]",  "[face avatar]",  "[coat avatar]",
        "[pants avatar]", "[shoes avatar]", "[breast avatar]", "[waist avatar]",
        "[skin avatar]",  "[aurora avatar]", "[weapon]",      "[title name]",
        "[coat]",         "[shoulder]",     "[pants]",        "[shoes]",
        "[waist]",        "[amulet]",       "[wrist]",        "[ring]",
        "[support]",      "[magic stone]",  "[creature]",     "[artifact red]",
        "[artifact blue]", "[artifact green]",
    };
    for (int i = 0; i < 26; ++i)
    {
        if (strcmp(name, kNames[i]) == 0)
            return i;
    }
    return 26;
}

// ---- ORIG T 0x088c227b：[calculation type]（'+'→0、'%'→1、其余→2） ----
int getCalculationType(const char* name)
{
    std::string tmp(name);
    if (tmp == "+")
        return 0;
    if (tmp == "%")
        return 1;
    return 2;
}

// ---- ORIG T 0x088c1492：world 标签→类型 ----
int getWorldType(const char* name)
{
    std::string tmp(name);
    if (tmp == "[common]")
        return 0;
    if (tmp == "[fair]")
        return 1;
    return 2;
}

// ---- ORIG T 0x088c1430 ----
int scanWorldType()
{
    std::string token;
    ScanStr(&token);
    return getWorldType(token.c_str());
}

// ---- ORIG T 0x088c167a：[command] 键序列解析 ----
// 循环体按 token 分派：5=终止收集；6=键名查表入 m_ids；2=[speed] 十进制；
// 7=[type] DOWN/DOWN ONCE/UP/UP ONCE → m_18=0..3；8=连接符（'&' 把 m_ids 并入
// command->m_vecC，'!'','~' 设超时后把 unit 追加进 m_vec0）。收尾再追加残 unit。
bool importCommand(STKeyCommand* command)
{
    if (command == 0)
        return false;

    STKeyCommandUnit unit;
    command->m_vec0.clear();

    int token = g_dnfLex.getToken();
    while (token != 0)
    {
        if (token == 5)
            break;

        if (token == 6)
        {
            std::string name = toTString(g_dnfLex.getCustomData());
            std::map<std::string, int>::const_iterator it =
                g_mapKeyNameIndexTable.find(name);
            if (it != g_mapKeyNameIndexTable.end())
                unit.m_ids.push_back(it->second);
        }
        else if (token == 2)
        {
            unit.m_fieldC = g_dnfLex.getDecimal();
        }
        else if (token == 7)
        {
            std::string type = toTString(g_dnfLex.getString());
            if (type == "DOWN")
                command->m_18 = 0;
            else if (type == "DOWN ONCE")
                command->m_18 = 1;
            else if (type == "UP")
                command->m_18 = 2;
            else if (type == "UP ONCE")
                command->m_18 = 3;
        }
        else if (token == 8)
        {
            char connector = *g_dnfLex.getConnector();
            if (connector == '&')
            {
                if (!unit.m_ids.empty())
                    command->m_vecC = unit.m_ids;
                unit.clear();
            }
            else
            {
                if (connector == '!')
                    unit.m_field10 = 0x96;    // 150
                else if (connector == ',')
                    unit.m_field10 = 0x12c;   // 300
                else if (connector == '~')
                    unit.m_field10 = 0x3e8;   // 1000
                if (!unit.m_ids.empty())
                    command->m_vec0.push_back(unit);
                unit.clear();
            }
        }
        token = g_dnfLex.getToken();
    }

    if (!unit.m_ids.empty())
        command->m_vec0.push_back(unit);
    return true;
}

// ---- ORIG T 0x088c1a0d：[effect] 条目解析 ----
bool importEffect(effect::STEffect& effect)
{
    std::string tag;
    std::string value;
    while (true)
    {
        ScanType(tag, true);

        if (tag == "[attach target]")
        {
            ScanStr(&value);
            if (value == "delegator")
                effect.m_a = 0;
            else if (value == "target")
                effect.m_a = 1;
        }
        else if (tag == "[type]")
        {
            ScanStr(&value);
            if (value == "animation")
                effect.m_b = 0;
            else if (value == "particle")
                effect.m_b = 1;
        }
        else if (tag == "[attach pos]")
        {
            ScanStr(&value);
            if (value == "bottom")
                effect.m_c = 2;
            else if (value == "middle")
                effect.m_c = 1;
            else if (value == "top")
                effect.m_c = 0;
        }
        else if (tag == "[z revision]")
        {
            effect.m_4 = ScanInt((bool*)0);
        }
        else if (tag == "[index]")
        {
            effect.m_8 = ScanInt((bool*)0);
        }
        else if (tag == "[option]")
        {
            // 位标志子循环：ScanStr 失败即回到外层标签循环（ORIG 88c1c32）。
            while (ScanStr(&value))
            {
                if (value == "keep direction")
                    effect.m_c_ |= 0x01;
                else if (value == "floor")
                    effect.m_c_ |= 0x02;
                else if (value == "auto remove")
                    effect.m_c_ |= 0x04;
                else if (value == "apply custom color")
                    effect.m_c_ |= 0x08;
                else if (value == "sync speed")
                    effect.m_c_ |= 0x10;
                else if (value == "copy horizon")
                    effect.m_c_ |= 0x20;
                else if (value == "copy vertical")
                    effect.m_c_ |= 0x40;
                else if (value == "resize")
                    effect.m_c_ |= 0x80;
            }
        }
        else if (tag == "[module]")
        {
            effect.m_10 = scanModuleType();
        }
        else if (tag == "[/module]")
        {
            break;   // ORIG 落入 [file name] 比较失败路径 → 返回 true
        }
        else if (tag == "[file name]")
        {
            ScanStr(&effect.m_str14);
        }
    }
    return true;
}

// ---- ORIG T 0x089c1cb9：动画层提交（normal/level/job 三层 + def 模板逐槽拼接） ----
void setEquipmentAniScriptLay(
    const std::map<std::string, STEquipmentAniScript>* table,
    STEquipmentAniScript* ani, std::string* key, int layerId,
    std::string* path, STEquipmentScript* script,
    ENUM_CHARACTERJOB job, equipmentAniType::T aniType)
{
    char buf[0x200];
    memset(buf, 0, sizeof(buf));
    const STEquipmentAniScript& def = table->find(*key)->second;

    EquipmentAniInfoScript& infoRef = script->addAniInfo(job);
    EquipmentAniInfoScript* info = &infoRef;
    if (aniType == equipmentAniType::T_job)
    {
        if (!info->m_level5c.empty())
            info->m_level5c.push_back(*info->m_level5c.rbegin());
    }
    else if (aniType == equipmentAniType::T_level)
    {
        info->m_ani18.push_back(*ani);
    }
    else
    {
        info->m_ani0.push_back(*ani);
    }

    ani->clear();
    ani->m_field0 = (layerId != 0) ? layerId : def.m_field0;
    ani->m_str60 = path->c_str();

#define SETANI_SLOT(member)                                                    \
    do                                                                         \
    {                                                                          \
        snprintf(buf, sizeof(buf), "%s%s", path->c_str(),                      \
                 def.member.c_str());                                          \
        ani->member = buf;                                                     \
    } while (0)

    // ORIG 拼接顺序（88c1e63..25c2）：…2c、3c、30、40、34、44、38、48 交错。
    SETANI_SLOT(m_str04);
    SETANI_SLOT(m_str08);
    SETANI_SLOT(m_str0c);
    SETANI_SLOT(m_str10);
    SETANI_SLOT(m_str14);
    SETANI_SLOT(m_str18);
    SETANI_SLOT(m_str1c);
    SETANI_SLOT(m_str20);
    SETANI_SLOT(m_str24);
    SETANI_SLOT(m_str28);
    SETANI_SLOT(m_str2c);
    SETANI_SLOT(m_str3c);
    SETANI_SLOT(m_str30);
    SETANI_SLOT(m_str40);
    SETANI_SLOT(m_str34);
    SETANI_SLOT(m_str44);
    SETANI_SLOT(m_str38);
    SETANI_SLOT(m_str48);
    SETANI_SLOT(m_str4c);
    SETANI_SLOT(m_str50);
    SETANI_SLOT(m_str54);
    SETANI_SLOT(m_str58);
    SETANI_SLOT(m_str5c);
#undef SETANI_SLOT

    for (std::vector<std::string>::const_iterator it = def.m_vec64.begin();
         it != def.m_vec64.end(); ++it)
    {
        snprintf(buf, sizeof(buf), "%s%s", path->c_str(), it->c_str());
        ani->m_vec64.push_back(std::string(buf));
    }
    for (std::vector<std::string>::const_iterator it = def.m_vec70.begin();
         it != def.m_vec70.end(); ++it)
    {
        snprintf(buf, sizeof(buf), "%s%s", path->c_str(), it->c_str());
        ani->m_vec70.push_back(std::string(buf));
    }
}

// ---- 专家职业解析组（ORIG t 局部静态；关键字映射表/SpinLock/inited 三件套） ----

// ORIG t 0x089bf6ca：[usable list] —— 关键字→typeSel(1..5) + 每项等级下限。
static bool GetExpertJobTypeList(STItemScript* item)
{
    if (item == 0)
        return false;
    item->m_expertJob.m_usable.clear();

    static std::map<const char*, int, less_TCHAR> __lookup;  // 0x94fde48
    static SpinLock __init_lock;                                  // 0x94fde60
    static bool __inited = false;                            // 0x94fde64

    std::string token;
    bool scanning = true;
    while (scanning)
    {
        int typeSel = 0;
        if (!ScanStr(&token))
            break;

        // 首个 token 惰性登记全部关键字映射（锁内连续 insert），随后重查本 token。
        if (!__inited)
        {
            __init_lock.enter();
            if (!__inited)
            {
                __lookup.insert(std::make_pair("enchanter", 0x303a));
                __lookup.insert(std::make_pair("alchemist", 0x303d));
                __lookup.insert(std::make_pair("disjointer", 0x3040));
                __lookup.insert(std::make_pair("doll_controller", 0x3043));
                __lookup.insert(std::make_pair("all", 0x3046));
                __inited = true;
            }
            __init_lock.leave();

            typeSel = 0;
            std::map<const char*, int, less_TCHAR>::const_iterator found =
                __lookup.find(token.c_str());
            if (found != __lookup.end())
                typeSel = found->second;
        }
        else
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            if (it == __lookup.end())
                return false;
            switch (it->second)
            {
            case 0x303a: typeSel = 1; break;   // enchanter
            case 0x303d: typeSel = 2; break;   // alchemist
            case 0x3040: typeSel = 3; break;   // disjointer
            case 0x3043: typeSel = 4; break;   // doll_controller
            case 0x3046: typeSel = 5; break;   // all
            default: return false;
            }
        }

        if (typeSel != 0)
        {
            int level = 0;
            if (!ScanInt(&level))
                return false;
            STItemScript::SEXPERTJOB::SUSABLE entry;
            entry.m_field0 = static_cast<ENUM_EXPERT_JOB_TYPE>(typeSel);
            entry.m_field4 = level;
            item->m_expertJob.m_usable.push_back(entry);
        }
    }
    return !item->m_expertJob.m_usable.empty();
}

// ORIG t 0x089bfc2f：-100.0 哨兵（0x8e1a574）视为未设置。
bool GetExpertJobValueMaterialVariation(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field18 = ScanFloat(&ok);
    if (item->m_expertJob.m_field18 == -100.0f)
        return false;
    return ok;
}

// ORIG t 0x089bfc84
bool GetExpertJobValueCompoundRate(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field1c = ScanFloat(&ok);
    return ok;
}

// ORIG t 0x089bfcb5：第一 float 成功即返回；否则读第二槽。
static bool GetExpertJobResultVariation(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_variation.m_field0 = ScanFloat(&ok);
    if (ok)
        return true;
    item->m_expertJob.m_variation.m_field4 = ScanFloat(&ok);
    return ok;
}

// ORIG t 0x089bfd0b
static bool GetExpertJobDisjointBigWinRate(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field10 = ScanFloat(&ok);
    return ok;
}

// ORIG t 0x089bfd3c
static bool GetExpertJobDisjointResultVariation(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field14 = ScanFloat(&ok);
    return ok;
}

// ORIG t 0x089bfd6d：float(+0x140) 失败时回退读 int(+0x144)。
static bool GetExpertJobAdditionalGainExp(STItemScript* item)
{
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_additionalExp.m_field0 = ScanFloat(&ok);
    if (ok)
        return true;
    item->m_expertJob.m_additionalExp.m_field4 = ScanInt(&ok);
    return ok;
}

// ---- ORIG T 0x08990f66：纹章槽前缀→位标志（strncmp 前缀匹配；[All socket]
//      长度 12；其余 10；失配→0） ----
int getAvatarEmblemType(const char* name)
{
    if (strncmp(name, "[A socket]", 10) == 0)
        return 1;
    if (strncmp(name, "[B socket]", 10) == 0)
        return 2;
    if (strncmp(name, "[C socket]", 10) == 0)
        return 4;
    if (strncmp(name, "[D socket]", 10) == 0)
        return 8;
    if (strncmp(name, "[S socket]", 10) == 0)
        return 0x10;
    if (strncmp(name, "[M socket]", 10) == 0)
        return 0xffef;    // ORIG mov $0xffef (65519)
    if (strncmp(name, "[All socket]", 12) == 0)
        return 0xffff;     // ORIG mov $0xffff (65535)
    return 0;
}
