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

int scanModuleType();                                       // T 0x088c0b9a
int getModuleType(const char* name);                        // T 0x088c0bfc
int getCalculationType(const char* name);                   // T 0x088c227b
int scanWorldType();                                        // T 0x088c1430
int GetEquipmentType(const char* name);                     // T 0x08990f1f
int getAvatarEmblemType(const char* name);                  // T 0x08990f66
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
        return &(*it).second;   // ORIG 用 operator*（de）
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
    // 声明顺序 = ORIG prologue 构造/初始化执行顺序（帧槽位由声明序决定，
    // 注释内为 ORIG %ebp 槽位，报告 0x08991b00-0x08991d8b）：
    std::string tmp;                       // ORIG -0x106c（各 case 第一读取暂存）
    std::string token;                     // ORIG -0x1070（ScanType 目标）
    std::string unusedStr;                 // ORIG -0x1074（ORIG 死变量：仅 ctor/dtor）
    std::string scanBuf;                   // ORIG -0x1078（getDirectoryPath 输出与 motion 共用）
    STEquipmentAniScript normalLayer;      // ORIG -0x1258
    int intScratch = 0;                    // ORIG -0x107c（ScanInt(int*)/值暂存）
    bool boolScratch = false;              // ORIG -0x107d（ScanInt(bool*) 出参）
    int normalLayerId = 0;                 // ORIG -0xb0
    std::string normalPath("");            // ORIG -0x1084（string("") 带 allocator 形参）
    int curJob = 0xb;                      // ORIG -0xac（0xb = 全职业缺省）
    int cnt[4];                            // ORIG -0x1088..-0x1094（参数解析器计数输出）
    cnt[0] = 0;
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0;
    bool aniMode = false;                  // ORIG -0xa5（[expand ani]..[/expand ani] 区）
    STEquipmentAniScript jobLayer;         // ORIG -0x12d4
    int jobLayerId = 0;                    // ORIG -0xa4
    std::string jobPath("");               // ORIG -0x1098
    int levelAniIdx = -1;                  // ORIG -0x109c（level section 状态）
    STEquipmentAniScript levelLayer;       // ORIG -0x1350
    GrowthEquipmentInfo growth;            // ORIG -0x10b4
    int levelLayerId = 0;                  // ORIG -0xa0
    std::string levelPath("");             // ORIG -0x10b8
    bool okFlag = false;                   // 各 case 的 ScanInt(bool*) 成功标志
    bool stopFlag = false;                 // ORIG -0x96
    EquipmentParameterInfo* curParam = &script->m_param1c0;  // ORIG -0x9c
    std::vector<std::pair<int, int> > setAbilityBackup;      // ORIG -0x10c8
    int upgMin = -1;                       // ORIG -0x1164
    int upgMax = -1;                       // ORIG -0x1168
    int upgType = 5;                       // ORIG -0x116c
    char printBuf[0x200];                  // ORIG -0x1550

    if (!loadRDARScriptFile(kEquipmentScriptBaseDir, path))
    {
        return false;
    }
    script->Clear();
    getDirectoryPath(scanBuf, "%s%s", kEquipmentScriptBaseDir, path);
    script->m_str664 = scanBuf;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                       // EOF → 收尾（ORIG jne 0x0899c9f1）
        }

        // ---------------- 第一段分派（主属性区，ORIG switch(local_98)） ----------
        // ORIG 0x08991daa-0x08994d8b：静态关键字表（std::map<char const*,int,
        // less_TCHAR>）+ SpinLock 双检锁；case 值为整数 switch（GCC 二分降形态），
        // 每个 case 体首查 __inited：首轮（!__inited）idA=0 命中 case 0/0x67d 别名，
        // 沿 case 布局逐例 __lookup.insert 惰性建表（74 项，ORIG 0x08992431 起），
        // 链尾填完置 __inited 并回循环头对本 token 重新 find + 真实分派。
        bool matched = true;                        // ORIG local_99（+0x2b8 置 1）
        static std::map<char const*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int idA;
        for (;;)
        {
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                idA = 0;
            }
            else
            {
                char const* key = token.c_str();
                std::map<char const*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(key);
                if (it != __lookup.end())
                {
                    idA = it->second;
                }
                else
                {
                    idA = -1;
                }
            }
            switch (idA)
            {
        case 0:
        case 0x67d:                              // [name]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[name]", 0x67d));
        }
        else
        {
            ScanStr(&script->m_strac);
            if (flag)
            {
                stopFlag = true;
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x682:                              // [name2]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[name2]", 0x682));
        }
        else
        {
            ScanStr(&script->m_strb0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x685:                              // [explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain]", 0x685));
        }
        else
        {
            ScanStr(&script->m_strb4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x689:                              // [explain2]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain2]", 0x689));
        }
        else
        {
            ScanStr(&script->m_strb4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x68d:                              // [basic explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[basic explain]", 0x68d));
        }
        else
        {
            ScanStr(&script->m_str6a4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x690:                              // [detail explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[detail explain]", 0x690));
        }
        else
        {
            ScanStr(&script->m_str6a8);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x693:                              // [parameter basic explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[parameter basic explain]", 0x693));
        }
        else
        {
            ScanStr(&curParam->m_str278);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x696:                              // [parameter detail explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[parameter detail explain]", 0x696));
        }
        else
        {
            ScanStr(&curParam->m_str27c);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x699:                              // [flavor text]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[flavor text]", 0x699));
        }
        else
        {
            ScanStr(&script->m_strb8);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x69c:                              // [attack info]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[attack info]", 0x69c));
        }
        else
        {
            ScanStr(&script->m_str6b8);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x69f:                              // [fullset explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[fullset explain]", 0x69f));
        }
        else
        {
            ScanStr(&script->m_str6ac);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6a2:                              // [fullset basic explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[fullset basic explain]", 0x6a2));
        }
        else
        {
            ScanStr(&script->m_str6b0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6a5:                              // [fullset detail explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[fullset detail explain]", 0x6a5));
        }
        else
        {
            ScanStr(&script->m_str6b4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6a8:                              // [set name]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[set name]", 0x6a8));
        }
        else
        {
            ScanStr(&script->m_str668);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ab:                              // [grade]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[grade]", 0x6ab));
        }
        else
        {
            script->m_field4 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ae:                              // [spectrum]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[spectrum]", 0x6ae));
        }
        else
        {
            unsigned int v1 = (unsigned int)ScanInt((bool*)0);
            unsigned int v2 = (unsigned int)ScanInt((bool*)0);
            unsigned int v3 = (unsigned int)ScanInt((bool*)0);
            int v4 = ScanInt((bool*)0);
            script->m_790 = (int)((v3 & 0xff) << 0x10 | v1 & 0xff |
                                  (v2 & 0xff) << 8 | v4 << 0x18);
            script->m_794 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6b6:                              // [clear avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[clear avatar]", 0x6b6));
        }
        else
        {
            if (ScanInt((bool*)0) == 1)
            {
                script->m_7ac = 1;
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6bb:                              // [attach type]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[attach type]", 0x6bb));
        }
        else
        {
            ScanStr(&tmp);
            if (tmp == "[free]")               script->m_field8 = 0;
            else if (tmp == "[trade]")         script->m_field8 = 1;
            else if (tmp == "[trade delete]")  script->m_field8 = 2;
            else if (tmp == "[sealing]")       script->m_field8 = 3;
            else if (tmp == "[sealing trade]") script->m_field8 = 4;
            else if (tmp == "[account]")       script->m_field8 = 5;
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6cc:                              // [creation rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creation rate]", 0x6cc));
        }
        else
        {
            script->m_fieldc = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6cf:                              // [rarity]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[rarity]", 0x6cf));
        }
        else
        {
            script->m_field10 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6d2:                              // [weight]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[weight]", 0x6d2));
        }
        else
        {
            script->m_field34 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6d5:                              // [icon]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[icon]", 0x6d5));
        }
        else
        {
            ScanStr(&script->m_str6c);
            script->m_field70 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6da:                              // [aura hud icon]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[aura hud icon]", 0x6da));
        }
        else
        {
            script->m_188 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6df:                              // [tarotcard view]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[tarotcard view]", 0x6df));
        }
        else
        {
            ScanStr(&script->m_str7c);
            script->m_field80 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6e5:                              // [extra icon]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[extra icon]", 0x6e5));
        }
        else
        {
            ScanStr(&tmp);
            script->m_vec18c.push_back(tmp);
            script->m_vec198.push_back(ScanInt((bool*)0));
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ea:                              // [field image]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[field image]", 0x6ea));
        }
        else
        {
            ScanStr(&script->m_str74);
            script->m_field78 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ee:                              // [field animation]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[field animation]", 0x6ee));
        }
        else
        {
            ScanStr(&script->m_str1a4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6f1:                              // [custom animation]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[custom animation]", 0x6f1));
        }
        else
        {
            script->m_vec1a8.clear();
            while (ScanStr(&tmp))
            {
                script->m_vec1a8.push_back(tmp);
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x71c:                              // [usable job]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable job]", 0x71c));
        }
        else
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
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x72a:                              // [suitable job]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[suitable job]", 0x72a));
        }
        else
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
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x739:                              // [max having count]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[max having count]", 0x739));
        }
        else
        {
            script->m_field60 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x73e:                              // [minimum level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[minimum level]", 0x73e));
        }
        else
        {
            script->m_field5c = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x741:                              // [maximum level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[maximum level]", 0x741));
        }
        else
        {
            script->m_17c = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x744:                              // [minimum rank]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[minimum rank]", 0x744));
        }
        else
        {
            script->m_field64 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x747:                              // [control index]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[control index]", 0x747));
        }
        else
        {
            script->m_180 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x74a:                              // [required skill]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[required skill]", 0x74a));
        }
        else
        {
            script->m_184 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x74d:                              // [cool time]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cool time]", 0x74d));
        }
        else
        {
            script->m_field68 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x750:                              // [equipment type]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[equipment type]", 0x750));
        }
        else
        {
            ScanStr(&tmp);
            script->m_strbc = tmp;
            script->m_field1b4 = GetEquipmentType(tmp.c_str());
            script->m_fielda4 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x75a:                              // [avatar task num]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar task num]", 0x75a));
        }
        else
        {
            ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x762:                              // [weapon full avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[weapon full avatar]", 0x762));
        }
        else
        {
            script->m_72d = 1;
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x766:                              // [hide equipment]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hide equipment]", 0x766));
        }
        else
        {
            script->m_equipTypes768.clear();
            while (ScanStr(&tmp))
            {
                if (tmp == "[/hide equipment]")
                {
                    break;
                }
                ENUM_EQUIPMENTTYPE eqType = (ENUM_EQUIPMENTTYPE)0x16;
                if (tmp == "[hat avatar]")          eqType = (ENUM_EQUIPMENTTYPE)0;
                else if (tmp == "[hair avatar]")    eqType = (ENUM_EQUIPMENTTYPE)1;
                else if (tmp == "[face avatar]")    eqType = (ENUM_EQUIPMENTTYPE)2;
                else if (tmp == "[coat avatar]")    eqType = (ENUM_EQUIPMENTTYPE)3;
                else if (tmp == "[pants avatar]")   eqType = (ENUM_EQUIPMENTTYPE)4;
                else if (tmp == "[shoes avatar]")   eqType = (ENUM_EQUIPMENTTYPE)5;
                else if (tmp == "[breast avatar]")  eqType = (ENUM_EQUIPMENTTYPE)6;
                else if (tmp == "[waist avatar]")   eqType = (ENUM_EQUIPMENTTYPE)7;
                else if (tmp == "[skin avatar]")    eqType = (ENUM_EQUIPMENTTYPE)8;
                else if (tmp == "[aurora avatar]")  eqType = (ENUM_EQUIPMENTTYPE)9;
                else if (tmp == "[weapon]")         eqType = (ENUM_EQUIPMENTTYPE)10;
                else if (tmp == "[title name]")     eqType = (ENUM_EQUIPMENTTYPE)0xb;
                if ((int)eqType != 0x16)
                {
                    script->m_equipTypes768.push_back(eqType);
                }
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x792:                              // [hide layer]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hide layer]", 0x792));
        }
        else
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
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x79e:                              // [hide growtype avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hide growtype avatar]", 0x79e));
        }
        else
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
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7aa:                              // [usable period]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable period]", 0x7aa));
        }
        else
        {
            script->m_fieldf4 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7ad:                              // [expiration date]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[expiration date]", 0x7ad));
        }
        else
        {
            script->m_fieldf8 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7bf:                              // [usable even peace]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable even peace]", 0x7bf));
        }
        else
        {
            script->m_78c = (char)(ScanInt((bool*)0) == 1);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7c3:                              // [sub type]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[sub type]", 0x7c3));
        }
        else
        {
            if (aniMode)
            {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                info.m_3c = ScanInt((bool*)0);
            }
            else if (levelAniIdx != -1)
            {
                ScanInt((bool*)0);
            }
            else
            {
                script->m_1b8 = ScanInt((bool*)0);
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7d3:                              // [durability]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[durability]", 0x7d3));
        }
        else
        {
            script->m_1bc = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7d6:                              // [creature species]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature species]", 0x7d6));
        }
        else
        {
            script->m_6bc = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7d9:                              // [creature minimum level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature minimum level]", 0x7d9));
        }
        else
        {
            script->m_artifact738.m_minLevel = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7dc:                              // [creature physical attack]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature physical attack]", 0x7dc));
        }
        else
        {
            script->m_artifact738.m_physAttack = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7df:                              // [creature magical attack]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature magical attack]", 0x7df));
        }
        else
        {
            script->m_artifact738.m_magAttack = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7e2:                              // [creature skill consume mp rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature skill consume mp rate]", 0x7e2));
        }
        else
        {
            script->m_artifact738.m_mpRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7e5:                              // [creature skill charge time rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature skill charge time rate]", 0x7e5));
        }
        else
        {
            script->m_artifact738.m_chargeRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7e8:                              // [creature skill over charge time rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature skill over charge time rate]", 0x7e8));
        }
        else
        {
            script->m_artifact738.m_overChargeRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7eb:                              // [creature experience amount rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature experience amount rate]", 0x7eb));
        }
        else
        {
            script->m_artifact738.m_expRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7ee:                              // [creature physical critical hit]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature physical critical hit]", 0x7ee));
        }
        else
        {
            script->m_artifact738.m_physCrit = ScanFloat(1, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7f1:                              // [creature magical critical hit]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature magical critical hit]", 0x7f1));
        }
        else
        {
            script->m_artifact738.m_magCrit = ScanFloat(1, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7f4:                              // [creature stuck]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature stuck]", 0x7f4));
        }
        else
        {
            script->m_artifact738.m_stuck = ScanFloat(1, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7f7:                              // [creature piercing]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature piercing]", 0x7f7));
        }
        else
        {
            script->m_artifact738.m_piercing = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7ff:                              // [output index]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[output index]", 0x7ff));
        }
        else
        {
            script->m_764 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x802:                              // [price]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[price]", 0x802));
        }
        else
        {
            script->m_field14 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x805:                              // [value]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[value]", 0x805));
        }
        else
        {
            script->m_field24 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x808:                              // [repair price]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[repair price]", 0x808));
        }
        else
        {
            script->m_178 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x80b:                              // [finish point price]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[finish point price]", 0x80b));
        }
        else
        {
            script->m_field20 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x80e:                              // [cash]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cash]", 0x80e));
        }
        else
        {
            script->m_field18 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x811:                              // [medal]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[medal]", 0x811));
        }
        else
        {
            script->m_field1c = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x81a:                              // [usable upgrade chracter level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable upgrade chracter level]", 0x81a));
        }
        else
        {
            script->m_field134 = (unsigned short)ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x81e:                              // [need material]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[need material]", 0x81e));
        }
        else
        {
            script->m_pair28.first = ScanInt((bool*)0);
            script->m_pair28.second = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x823:                              // [recieve postal message]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[recieve postal message]", 0x823));
        }
        else
        {
            ScanStr(&script->m_str174);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x828:                              // [Uncompoundable]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[Uncompoundable]", 0x828));
        }
        else
        {
            okFlag = false;
            int v = ScanInt(&okFlag);
            if (okFlag && v == 1)
            {
                script->m_82c = 1;
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x83d:                              // [remove avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[remove avatar]", 0x83d));
        }
        else
        {
            // ORIG LAB_08994bb2：块作用域临时 string（ctor/dtor 各 1 次）。
            std::string moduleName;
            while (ScanStr(&moduleName))
            {
                // ORIG 08994c54：实参为 prvalue → push_back(int&&)。
                script->m_vec830.push_back(getModuleType(moduleName.c_str()));
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x845:                              // [auction free]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[auction free]", 0x845));
        }
        else
        {
            script->m_field30 = 1;
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x849:                              // [hidden option]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hidden option]", 0x849));
        }
        else
        {
            script->m_field31 = 1;
            goto stage1_switch_end;
        }
        default:
            if (__inited)
            {
                matched = false;
            }
            }
        stage1_switch_end:
            if (__inited)
            {
                break;                 // ORIG LAB_08994d8b → code_r0x08994db2
            }
            __init_lock.leave();       // ORIG 首轮沿 case 链填完表后置位并重新分派
            __inited = true;
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
                // ---------------- 第二段分派（动画/扩展区，ORIG switch(local_70)） --
                matched = true;
                // ORIG 0x08994e85-0x0899c9bb：第二级静态表 __lookup_0 + __inited_0
                // 双检锁（独立于第一段）；整数 switch 二分降形态，每个 case 体首查
                // __inited_0：首轮 idB=0 命中 case 0/0x867 别名，沿 case 布局逐例
                // 惰性 insert（109 项，ORIG 0x089956c4 起），填完置位重新分派。
                static std::map<char const*, int, less_TCHAR> __lookup_0;
                static SpinLock __init_lock_0;
                static bool __inited_0 = false;
                int idB;
                for (;;)
                {
                    if (!__inited_0)
                    {
                        __init_lock_0.enter();
                        if (__inited_0)
                        {
                            __init_lock_0.leave();
                            continue;
                        }
                        idB = 0;
                    }
                    else
                    {
                        char const* keyB = token.c_str();
                        std::map<char const*, int, less_TCHAR>::const_iterator itB =
                            __lookup_0.find(keyB);
                        if (itB != __lookup_0.end())
                        {
                            idB = itB->second;
                        }
                        else
                        {
                            idB = -1;
                        }
                    }
                    switch (idB)
                    {

                case 0:
                    /*FALLTHRU*/
                case 0x867:                          // [expand ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[expand ani]", 0x867));
                }
                else
                {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                aniMode = true;
                info.m_24 = ScanInt((bool*)0);
                info.m_28 = ScanInt((bool*)0);
                info.m_2c = ScanInt((bool*)0);
                info.m_38 = ScanInt((bool*)0);
                info.m_expand14 = 1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x870:                          // [/expand ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/expand ani]", 0x870));
                }
                else
                {
                aniMode = false;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x873:                          // [expand path]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[expand path]", 0x873));
                }
                else
                {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                ScanStr(&info.m_path4c);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x877:                          // [level section ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[level section ani]", 0x877));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x886:                          // [/level section ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/level section ani]", 0x886));
                }
                else
                {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                if (levelLayer.m_field0 != 0)
                {
                    if (!info.m_level5c.empty())
                    {
                        info.m_level5c.rbegin()->push_back(levelLayer);
                    }
                    levelLayer.clear();
                }
                info.m_growth50.push_back(growth);
                growth.clear();
                levelAniIdx = -1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x897:                          // [level section path]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[level section path]", 0x897));
                }
                else
                {
                // ORIG LAB_08995ae3：读入暂存串后赋 growth.m_str14（非 levelPath）。
                ScanStr(&scanBuf);
                growth.m_str14 = scanBuf;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x89e:                          // [layer variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[layer variation]", 0x89e));
                }
                else
                {
                // ORIG 08995b6e 区：aniMode 分支在前；层变体走 ScanInt(int*)，
                // 0 是合法层编号（失败标志才置 stopFlag）；先赋 levelLayerId 再 levelPath。
                if (aniMode)
                {
                    jobLayerId = ScanInt((bool*)0);
                    ScanStr(&jobPath);
                }
                else if (levelAniIdx != -1)
                {
                    int layerVariation = 0;
                    if (ScanInt(&layerVariation))
                    {
                        stopFlag = true;
                    }
                    else
                    {
                        ScanStr(&scanBuf);
                        levelLayerId = layerVariation;
                        levelPath = scanBuf;
                    }
                }
                else
                {
                    normalLayerId = ScanInt((bool*)0);
                    ScanStr(&normalPath);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x8bb:                          // [equipment ani script]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[equipment ani script]", 0x8bb));
                }
                else
                {
                ScanStr(&scanBuf);
                std::map<std::string, STEquipmentAniScript>* table =
                    getMapEquipmentAniScript();
                if (aniMode)
                {
                    setEquipmentAniScriptLay(table, &jobLayer, &scanBuf, jobLayerId,
                                             &jobPath, script,
                                             (ENUM_CHARACTERJOB)curJob,
                                             (equipmentAniType::T)1);
                }
                else if (levelAniIdx != -1)
                {
                    setEquipmentAniScriptLay(table, &levelLayer, &scanBuf,
                                             levelLayerId, &levelPath, script,
                                             (ENUM_CHARACTERJOB)curJob,
                                             (equipmentAniType::T)2);
                }
                else
                {
                    setEquipmentAniScriptLay(table, &normalLayer, &scanBuf,
                                             normalLayerId, &normalPath,
                                             script,
                                             (ENUM_CHARACTERJOB)curJob,
                                             equipmentAniType::T_normal);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x8da:                          // [LAYER]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[LAYER]", 0x8da));
                }
                else
                {
                if (aniMode)
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
                        jobLayer.m_str60 = jobPath;
                    }
                    else
                    {
                        ScanInt((bool*)0);
                        jobLayer.m_field0 = jobLayerId;
                        jobLayer.m_str60 = jobPath;
                    }
                }
                else if (levelAniIdx != -1)
                {
                    if (levelLayer.m_field0 != 0)
                    {
                        EquipmentAniInfoScript& info =
                            script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                        if (!info.m_level5c.empty())
                        {
                            info.m_level5c.rbegin()->push_back(levelLayer);
                        }
                    }
                    levelLayer.clear();
                    // ORIG 08995fb2：层号走 ScanInt(int*) 形式。
                    int scanned = 0;
                    ScanInt(&scanned);
                    if (levelLayerId == 0)
                    {
                        levelLayer.m_field0 = scanned;
                        levelLayer.m_str60 = levelPath;
                    }
                    else
                    {
                        levelLayer.m_field0 = levelLayerId;
                        levelLayer.m_str60 = levelPath;
                    }
                }
                else
                {
                    if (normalLayer.m_field0 != 0)
                    {
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                            .m_ani0.push_back(normalLayer);
                    }
                    normalLayer.clear();
                    // ORIG LAB_08995e01：两分支各自求值一次 m_str60 = path。
                    if (normalLayerId == 0)
                    {
                        normalLayer.m_field0 = ScanInt((bool*)0);
                        normalLayer.m_str60 = normalPath;
                    }
                    else
                    {
                        ScanInt((bool*)0);
                        normalLayer.m_field0 = normalLayerId;
                        normalLayer.m_str60 = normalPath;
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x925:                          // [waiting motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[waiting motion]", 0x925));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str04 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str04 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str04 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x939:                          // [move motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[move motion]", 0x939));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str08 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str08 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str08 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x94d:                          // [sit motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[sit motion]", 0x94d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str0c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str0c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str0c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x961:                          // [damage motion 1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[damage motion 1]", 0x961));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str10 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str10 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str10 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x975:                          // [damage motion 2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[damage motion 2]", 0x975));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str14 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str14 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str14 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x989:                          // [down motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[down motion]", 0x989));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str18 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str18 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str18 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x99d:                          // [overturn motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[overturn motion]", 0x99d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str1c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str1c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str1c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9b1:                          // [jump motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[jump motion]", 0x9b1));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str20 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str20 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str20 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9c5:                          // [jumpattack motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[jumpattack motion]", 0x9c5));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str24 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str24 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str24 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9d9:                          // [rest motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[rest motion]", 0x9d9));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str28 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str28 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str28 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9ed:                          // [throw motion 1-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 1-1]", 0x9ed));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str2c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str2c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str2c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa01:                          // [throw motion 1-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 1-2]", 0xa01));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str3c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str3c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str3c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa15:                          // [throw motion 2-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 2-1]", 0xa15));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str30 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str30 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str30 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa29:                          // [throw motion 2-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 2-2]", 0xa29));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str40 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str40 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str40 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa3d:                          // [throw motion 3-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 3-1]", 0xa3d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str34 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str34 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str34 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa51:                          // [throw motion 3-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 3-2]", 0xa51));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str44 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str44 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str44 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa65:                          // [throw motion 4-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 4-1]", 0xa65));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str38 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str38 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str38 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa79:                          // [throw motion 4-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 4-2]", 0xa79));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str48 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str48 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str48 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa8d:                          // [dash motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[dash motion]", 0xa8d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str4c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str4c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str4c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xaa1:                          // [dashattack motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[dashattack motion]", 0xaa1));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str50 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str50 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str50 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xab5:                          // [getitem motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[getitem motion]", 0xab5));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str54 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str54 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str54 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xac9:                          // [buff motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[buff motion]", 0xac9));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str58 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str58 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str58 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xadd:                          // [back motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[back motion]", 0xadd));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str5c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str5c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str5c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xaf1:                          // [attack motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[attack motion]", 0xaf1));
                }
                else
                {
                    if (aniMode)
                    {
                        jobLayer.m_vec64.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                            tmp = printBuf;
                            jobLayer.m_vec64.push_back(tmp);
                        }
                    }
                    else if (levelAniIdx != -1)
                    {
                        levelLayer.m_vec64.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                            tmp = printBuf;
                            levelLayer.m_vec64.push_back(tmp);
                        }
                    }
                    else
                    {
                        normalLayer.m_vec64.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                            tmp = printBuf;
                            normalLayer.m_vec64.push_back(tmp);
                        }
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb16:                          // [etc motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[etc motion]", 0xb16));
                }
                else
                {
                    if (aniMode)
                    {
                        jobLayer.m_vec70.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                            tmp = printBuf;
                            jobLayer.m_vec70.push_back(tmp);
                        }
                    }
                    else if (levelAniIdx != -1)
                    {
                        levelLayer.m_vec70.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                            tmp = printBuf;
                            levelLayer.m_vec70.push_back(tmp);
                        }
                    }
                    else
                    {
                        normalLayer.m_vec70.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                            tmp = printBuf;
                            normalLayer.m_vec70.push_back(tmp);
                        }
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb3d:                          // [move wav]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[move wav]", 0xb3d));
                }
                else
                {
                ScanStr(&script->m_str84);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb40:                          // [custom wav]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[custom wav]", 0xb40));
                }
                else
                {
                if (aniMode)
                {
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    info.m_wav40.clear();
                    while (ScanStr(&tmp))
                    {
                        info.m_wav40.push_back(tmp);
                    }
                }
                else
                {
                    script->m_vec658.clear();
                    while (ScanStr(&tmp))
                    {
                        script->m_vec658.push_back(tmp);
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb51:                          // [animation job]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[animation job]", 0xb51));
                }
                else
                {
                ScanStr(&scanBuf);
                // ORIG LAB_08998b69：提交后仅复位 m_field0（不调 clear()）。
                if (normalLayer.m_field0 != 0)
                {
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                        .m_ani0.push_back(normalLayer);
                    normalLayer.m_field0 = 0;
                }
                if (jobLayer.m_field0 != 0)
                {
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                        .m_ani18.push_back(jobLayer);
                    jobLayer.m_field0 = 0;
                }
                if (levelLayer.m_field0 != 0)
                {
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    if (!info.m_level5c.empty())
                    {
                        info.m_level5c.rbegin()->push_back(levelLayer);
                    }
                    levelLayer.m_field0 = 0;
                }
                curJob = getCharacterJob(scanBuf.c_str());
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb6d:                          // [variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[variation]", 0xb6d));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb82:                          // [setback img]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[setback img]", 0xb82));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb98:                          // [/setback img]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/setback img]", 0xb98));
                }
                else
                {
                // 空 case（ORIG 仅建表项，无 body）
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb9b:                          // [set item]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[set item]", 0xb9b));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xba8:                          // [piece set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[piece set ability]", 0xba8));
                }
                else
                {
                // ORIG LAB_08999014：分档号由 ScanInt 读出（bool* 出参形式）；
                // find==end 时先 [key].clear()，else 分支仍走一次 operator[]（保证
                // 存在默认元素），最后再取 &table[key] 作为当前参数区。
                okFlag = false;
                int key = ScanInt(&okFlag);
                std::map<int, EquipmentParameterInfo>& table = script->m_paramMap678;
                if (table.find(key) == table.end())
                {
                    table[key].clear();
                }
                else
                {
                    table[key];
                }
                curParam = &table[key];
                curParam->m_414 = 1;
                cnt[1] = 0;
                cnt[0] = 0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbb5:                          // [set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[set ability]", 0xbb5));
                }
                else
                {
                // ORIG LAB_08999174：进入“分档 0”参数区。find(0)==end() 时先
                // table[0].clear()，否则仍求值一次 table[0]（保证默认元素存在），
                // 最后再取 &table[0] 作为当前参数区并置 m_414 = 1、清计数。
                std::map<int, EquipmentParameterInfo>& table = script->m_paramMap678;
                int probeKey = 0;
                if (table.find(probeKey) == table.end())
                {
                    int clearKey = 0;
                    table[clearKey].clear();
                }
                else
                {
                    int existKey = 0;
                    table[existKey];
                }
                int layKey = 0;
                curParam = &table[layKey];
                curParam->m_414 = 1;
                cnt[1] = 0;
                cnt[0] = 0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbc1:                          // [/set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/set ability]", 0xbc1));
                }
                else
                {
                // ORIG LAB_089992e8：保存当前分档的 [set ability] 向量，
                // 然后切回主参数区继续解析。
                setAbilityBackup = curParam->m_vec468;
                curParam = &script->m_param1c0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbc8:                          // [/piece set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/piece set ability]", 0xbc8));
                }
                else
                {
                curParam = &script->m_param1c0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbcb:                          // [set item master]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[set item master]", 0xbcb));
                }
                else
                {
                script->m_6a0 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbce:                          // [int data]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[int data]", 0xbce));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbd9:                          // [string data]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[string data]", 0xbd9));
                }
                else
                {
                script->m_vec94.clear();
                while (ScanStr(&tmp))
                {
                    script->m_vec94.push_back(tmp);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbe0:                          // [packagable]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[packagable]", 0xbe0));
                }
                else
                {
                script->m_fieldfc = (char)(ScanInt((bool*)0) != 0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbe3:                          // [icon mark]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[icon mark]", 0xbe3));
                }
                else
                {
                // ORIG LAB_0899962b 系：先读暂存再赋 string（operator=(const string&)）。
                ScanStr(&scanBuf);
                script->m_strc0 = scanBuf;
                ScanInt(&script->m_fieldc4);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbf0:                          // [part set index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[part set index]", 0xbf0));
                }
                else
                {
                script->m_798 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbf4:                          // [effect part set index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[effect part set index]", 0xbf4));
                }
                else
                {
                script->m_79c = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbf7:                          // [reference effect part set index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[reference effect part set index]", 0xbf7));
                }
                else
                {
                script->m_vec7a0.push_back(ScanInt((bool*)0));
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbfb:                          // [image packs]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[image packs]", 0xbfb));
                }
                else
                {
                while (ScanStr(&tmp))
                {
                    script->m_vec7b0.push_back(tmp);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc04:                          // [fixed select type]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[fixed select type]", 0xc04));
                }
                else
                {
                okFlag = false;
                script->m_6e0 = ScanInt(&okFlag);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc08:                          // [emblem socket default]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[emblem socket default]", 0xc08));
                }
                else
                {
                for (;;)
                {
                    if (!ScanStr(&scanBuf))
                    {
                        break;
                    }
                    int emblemType = getAvatarEmblemType(scanBuf.c_str());
                    if (emblemType == 0)
                    {
                        return false;
                    }
                    script->m_vec6cc.push_back((unsigned short)emblemType);
                }
                if (5 < (int)script->m_vec6cc.size())
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc16:                          // [avatar type select]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar type select]", 0xc16));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc55:                          // [avatar func filter]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar func filter]", 0xc55));
                }
                else
                {
                okFlag = false;
                script->m_6d8 = ScanInt(&okFlag);
                if (!okFlag)
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc5a:                          // [emancipate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[emancipate]", 0xc5a));
                }
                else
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
                                std::pair<int, int>(first, second));
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
                                std::pair<int, int>(first, second));
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc8f:                          // [hardcoding cooltime]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[hardcoding cooltime]", 0xc8f));
                }
                else
                {
                script->m_6e4 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
            // ORIG 0xc93（汇编 LAB_0899a04f __inited==1 分支）：ScanStr/表查找失败
            // 均直接 return 0——失败路径不调 unloadRDARScriptFile（全函数唯一
            // unloadRDARScriptFile 在成功收尾 0xb084）。
                    /*FALLTHRU*/
                case 0xc93:                          // [item group name]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[item group name]", 0xc93));
                }
                else
                {
                if (!ScanStr(&scanBuf))
                {
                    return false;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(scanBuf);
                if (git == getItemGroupNameTable().end())
                {
                    return false;
                }
                script->m_field100 = git->second;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc9d:                          // [npc gift disallowance]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[npc gift disallowance]", 0xc9d));
                }
                else
                {
                okFlag = false;
                int v = ScanInt(&okFlag);
                script->m_field104 = (char)(v != 1);
                if (!okFlag)
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xca2:                          // [change status on change hp]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[change status on change hp]", 0xca2));
                }
                else
                {
                // ORIG 结构：块作用域 string（ctor/dtor 各 1）+ do-while 标志位；
                // 循环体内还有一个未被消费的 std::vector<float> 临时（ORIG 0899a24b
                // 构造、两条退出路径各析构一次），照其形态保留。
                std::string appendageName;
                bool cont;
                do
                {
                    if (!ScanStr(&appendageName))
                    {
                        break;
                    }
                    std::vector<float> rateBuf;
                    (void)rateBuf;
                    if (appendageName == "[/change status on change hp]")
                    {
                        cont = false;
                    }
                    else
                    {
                        int appendage = loadAppendageType(appendageName);
                        if (appendage != -1)
                        {
                            STChangeStatusRate rate;
                            rate.m_appendage = appendage;
                            okFlag = true;
                            rate.m_rate1 = (float)ScanInt(&okFlag);
                            rate.m_rate3 = (float)ScanInt(&okFlag);
                            rate.m_rate2 = (float)ScanInt(&okFlag);
                            rate.m_rate4 = (float)ScanInt(&okFlag);
                            if (okFlag)
                            {
                                script->m_rate6e8.push_back(rate);
                            }
                        }
                        cont = true;
                    }
                } while (cont);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcc1:                          // [epic routing]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[epic routing]", 0xcc1));
                }
                else
                {
                okFlag = false;
                int v = ScanInt(&okFlag);
                script->m_7bc = (char)(v == 1);
                if (!okFlag)
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcc6:                          // [special ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[special ability]", 0xcc6));
                }
                else
                {
                okFlag = false;
                script->m_7c0 = ScanInt(&okFlag);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcc9:                          // [chat emoticon index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[chat emoticon index]", 0xcc9));
                }
                else
                {
                okFlag = false;
                script->m_7c4 = ScanInt(&okFlag);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xccc:                          // [required full set for chatting emoticon]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[required full set for chatting emoticon]", 0xccc));
                }
                else
                {
                okFlag = false;
                int v = ScanInt(&okFlag);
                script->m_7c8 = (char)(v != 0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xccf:                          // [guild power war point]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[guild power war point]", 0xccf));
                }
                else
                {
                script->m_fieldc8 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcd2:                          // [user power war point]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[user power war point]", 0xcd2));
                }
                else
                {
                script->m_fieldcc = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcd5:                          // [special monster drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[special monster drop]", 0xcd5));
                }
                else
                {
                if (!EquipmentScriptSpecialMonsterDrop(script,
                                                       std::string(token), path))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                case 0xcd6:                          // [difficulty for extra drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[difficulty for extra drop]", 0xcd6));
                }
                else
                {
                if (!EquipmentScriptSpecialMonsterDrop(script,
                                                       std::string(token), path))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                case 0xcd7:                          // [dungeon type for extra drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[dungeon type for extra drop]", 0xcd7));
                }
                else
                {
                if (!EquipmentScriptSpecialMonsterDrop(script,
                                                       std::string(token), path))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcdd:                          // [character item check]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[character item check]", 0xcdd));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcf4:                          // [hp recovery add rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[hp recovery add rate]", 0xcf4));
                }
                else
                {
                ScanInt(&script->m_730);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcfc:                          // [mp recovery add rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[mp recovery add rate]", 0xcfc));
                }
                else
                {
                ScanInt(&script->m_734);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd04:                          // [impossible contents]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[impossible contents]", 0xd04));
                }
                else
                {
                // ORIG LAB_0899ab13：块作用域 string（ctor 1 / dtor 2 路径）。
                std::string contentsName;
                while (ScanStr(&contentsName))
                {
                    if (contentsName == "upgrade")
                    {
                        script->m_6f4 = 1;
                    }
                    else if (contentsName == "amplify upgrade")
                    {
                        script->m_6f5 = 1;
                    }
                    else if (contentsName == "disjoint")
                    {
                        script->m_6f6 = 1;
                    }
                    else if (contentsName == "gift")
                    {
                        script->m_fieldd0 = 1;
                    }
                    else if (contentsName == "trade")
                    {
                        script->m_fieldd1 = 1;
                    }
                    else if (contentsName == "separate upgrade")
                    {
                        script->m_fieldd4 = 1;
                    }
                    else if (contentsName == "charac cargo")
                    {
                        script->m_fieldda = 1;
                    }
                    else if (contentsName == "ban redeemitem")
                    {
                        script->m_fieldd8 = 1;
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd2d:                          // [possible kiri protect]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[possible kiri protect]", 0xd2d));
                }
                else
                {
                script->m_fieldd9 = 1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd31:                          // [limit upgradable level]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[limit upgradable level]", 0xd31));
                }
                else
                {
                while (ScanStr(&scanBuf))
                {
                    upgMin = -1;
                    upgMax = -1;
                    ScanInt(&upgMin);
                    ScanInt(&upgMax);
                    upgType = 5;
                     // ORIG 0x0899ae13-0x0899b289：第三级独立静态表 __lookup_1 + __inited_1
                     // 双检锁；整数 switch 五路降形态，case 体首查 __inited_1，首轮 idC=0 命中
                     // case 0/0xd3d 别名进入逐例惰性 insert 链（5 项）。
                     static std::map<char const*, int, less_TCHAR> __lookup_1;
                     static SpinLock __init_lock_1;
                     static bool __inited_1 = false;
                     int idC;
                     for (;;)
                     {
                         if (!__inited_1)
                         {
                             __init_lock_1.enter();
                             if (__inited_1)
                             {
                                 __init_lock_1.leave();
                                 continue;
                             }
                             idC = 0;
                         }
                         else
                         {
                             char const* key = scanBuf.c_str();
                             std::map<char const*, int, less_TCHAR>::const_iterator it =
                                 __lookup_1.find(key);
                             if (it != __lookup_1.end())
                             {
                                 idC = it->second;
                             }
                             else
                             {
                                 idC = -1;
                             }
                         }
                         switch (idC)
                         {
                         case 0:
                         case 0xd3d:                          // all upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("all upgrade", 0xd3d));
                         }
                         else
                         {
                             upgType = 0;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd40:                          // normal upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("normal upgrade", 0xd40));
                         }
                         else
                         {
                             upgType = 1;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd43:                          // amplify upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("amplify upgrade", 0xd43));
                         }
                         else
                         {
                             upgType = 2;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd46:                          // separate upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("separate upgrade", 0xd46));
                         }
                         else
                         {
                             upgType = 3;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd49:                          // 3rd chronicle upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("3rd chronicle upgrade", 0xd49));
                         }
                         else
                         {
                             upgType = 4;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         default:
                             break;
                         }
                     stage3_switch_end:
                         if (__inited_1)
                         {
                             break;                     // ORIG LAB_0899b262 → code_r0x0899b289
                         }
                         __init_lock_1.leave();     // ORIG 首轮沿链填表后置位重新分派
                         __inited_1 = true;
                     }
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
                                if (script->m_upgrade6fc.find(
                                        (ItemUpgradeType::T)t) ==
                                    script->m_upgrade6fc.end())
                                {
                                    std::pair<ItemUpgradeType::T,
                                              std::pair<int, int> > kv;
                                    kv.first = (ItemUpgradeType::T)t;
                                    kv.second = std::pair<int, int>(upgMin, upgMax);
                                    script->m_upgrade6fc.insert(kv);
                                }
                            }
                        }
                        else if (script->m_upgrade6fc.find(
                                     (ItemUpgradeType::T)upgType) ==
                                 script->m_upgrade6fc.end())
                        {
                        std::pair<ItemUpgradeType::T,
                                  std::pair<int, int> > kv;
                        kv.first = (ItemUpgradeType::T)upgType;
                        kv.second = std::pair<int, int>(upgMin, upgMax);
                        script->m_upgrade6fc.insert(kv);
                        }
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd7d:                          // [particle same position]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[particle same position]", 0xd7d));
                }
                else
                {
                ScanStr(&scanBuf);
                if (scanBuf == "same position")
                {
                    ScanStr(&script->m_str7f4);
                    script->m_particle7f8.m_x = ScanInt((bool*)0);
                    script->m_particle7f8.m_y = ScanInt((bool*)0);
                    script->m_particle7f8.m_z = ScanInt((bool*)0);
                    ScanType(scanBuf, true);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd89:                          // [command script]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[command script]", 0xd89));
                }
                else
                {
                script->m_keyCommand = new STKeyCommand();
                importCommand(script->m_keyCommand);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd8d:                          // [character name y revision]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[character name y revision]", 0xd8d));
                }
                else
                {
                script->m_80e = (unsigned short)ScanInt((bool*)0);
                script->m_80c = (unsigned short)ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd91:                          // [routing priority]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[routing priority]", 0xd91));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdac:                          // [quest item drop rate bonus]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[quest item drop rate bonus]", 0xdac));
                }
                else
                {
                script->m_810 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdb0:                          // [level section ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[level section ability]", 0xdb0));
                }
                else
                {
                int sectionMarker = 0;
                // ORIG 0899b9c0 传 int*：合法值 0 也必须创建新段；
                // 不能用 ScanInt(bool*) 的“数值非零”结果代替成功标志。
                if (ScanInt(&sectionMarker))
                {
                    EquipmentParameterInfo section;
                    script->m_paramVec690.push_back(section);
                    curParam = &script->m_paramVec690[
                        script->m_paramVec690.size() - 1];
                }
                else
                {
                    stopFlag = true;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdbf:                          // [/level section ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/level section ability]", 0xdbf));
                }
                else
                {
                curParam = &script->m_param1c0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdc3:                          // [Force Result Item Rule]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[Force Result Item Rule]", 0xdc3));
                }
                else
                {
                script->m_814 = (char)ScanInt((bool*)0);
                script->m_818 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdc7:                          // [effect]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[effect]", 0xdc7));
                }
                else
                {
                effect::STEffect effectData;
                if (importEffect(effectData))
                {
                    script->m_effect84c.push_back(effectData);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdcc:                          // [usable module]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable module]", 0xdcc));
                }
                else
                {
                script->m_set714.clear();
                int module = scanModuleType();
                while (module != 0x11)
                {
                    script->m_set714.insert(module);
                    module = scanModuleType();
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdd5:                          // [usable world]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable world]", 0xdd5));
                }
                else
                {
                script->m_setdc.clear();
                int world = scanWorldType();
                while (world != 2)
                {
                    script->m_setdc.insert(world);
                    world = scanWorldType();
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdde:                          // [no random]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[no random]", 0xdde));
                }
                else
                {
                script->m_72c = 1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xde1:                          // [usable period after unsealing]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable period after unsealing]", 0xde1));
                }
                else
                {
                script->m_6f8 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdeb:                          // [aurora graphic effects]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[aurora graphic effects]", 0xdeb));
                }
                else
                {
                int total = ScanInt((bool*)0);
                for (int i = 0; i < total; ++i)
                {
                    int enabled = ScanInt((bool*)0);
                    script->m_vec864.push_back(enabled == 1);
                    ScanStr(&scanBuf);
                    script->m_vec858.push_back(scanBuf);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdf5:                          // [msg balloon on mucu skill]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[msg balloon on mucu skill]", 0xdf5));
                }
                else
                {
                script->m_87c = ScanInt((bool*)0);
                ScanStr(&script->m_str878);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdf9:                          // [random option]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[random option]", 0xdf9));
                }
                else
                {
                script->m_888 = 1;
                script->m_889 = (char)ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdfe:                          // [enable dye]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[enable dye]", 0xdfe));
                }
                else
                {
                script->m_88a = (char)(ScanInt((bool*)0) != 0);
                script->m_88b = (char)(ScanInt((bool*)0) != 0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe0a:                          // [expertjob only]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[expertjob only]", 0xe0a));
                }
                else
                {
                if (!GetExpertJobTypeList(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe0e:                          // [prof material variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof material variation]", 0xe0e));
                }
                else
                {
                if (!GetExpertJobValueMaterialVariation(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe12:                          // [prof compound rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof compound rate]", 0xe12));
                }
                else
                {
                if (!GetExpertJobValueCompoundRate(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe16:                          // [prof result variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof result variation]", 0xe16));
                }
                else
                {
                if (!GetExpertJobResultVariation(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe1a:                          // [prof disjoint big win rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof disjoint big win rate]", 0xe1a));
                }
                else
                {
                if (!GetExpertJobDisjointBigWinRate(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe1e:                          // [prof disjoint result variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof disjoint result variation]", 0xe1e));
                }
                else
                {
                if (!GetExpertJobDisjointResultVariation(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe22:                          // [prof additional gain exp]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof additional gain exp]", 0xe22));
                }
                else
                {
                if (!GetExpertJobAdditionalGainExp(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe28:                          // [war room point bonus]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[war room point bonus]", 0xe28));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe37:                          // [war room monster exp bonus]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[war room monster exp bonus]", 0xe37));
                }
                else
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
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe48:                          // [not amplify]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[not amplify]", 0xe48));
                }
                else
                {
                script->m_890 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe4d:                          // [change type ultimateSkillCurScene]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[change type ultimateSkillCurScene]", 0xe4d));
                }
                else
                {
                script->m_6dc = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe54:                          // [item category]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[item category]", 0xe54));
                }
                else
                {
                GetItemCategory(script);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe57:                          // [aura ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[aura ability]", 0xe57));
                }
                else
                {
                GetAuraAvatarOption(script);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe5c:                          // [/aura ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/aura ability]", 0xe5c));
                }
                else
                {
                // 空 case
                    goto stage2_switch_end;
                }
                default:
                if (__inited_0)
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
                    }
                stage2_switch_end:
                    if (__inited_0)
                    {
                        break;              // ORIG LAB_0899c9bb → 退出分派循环
                    }
                    __init_lock_0.leave();  // ORIG 首轮沿链填表后置位重新分派
                    __inited_0 = true;
                }
            }
        }
        if (stopFlag)
        {
            break;
        }
    }

    // ---------------- 收尾（ORIG LAB_0899c9f2） ----------------
    if (!setAbilityBackup.empty())
        script->m_param1c0.m_vec468 = setAbilityBackup;
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
            info.m_level5c.rbegin()->push_back(levelLayer);
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
            __lookup.insert(std::pair<char const* const, int>("[creature minimum level]", 0x2ee9));
            __lookup.insert(std::pair<char const* const, int>("[creature physical attack]", 0x2eec));
            __lookup.insert(std::pair<char const* const, int>("[creature magical attack]", 0x2eef));
            __lookup.insert(std::pair<char const* const, int>("[creature skill consume mp rate]", 0x2ef2));
            __lookup.insert(std::pair<char const* const, int>("[creature skill charge time rate]", 0x2ef5));
            __lookup.insert(std::pair<char const* const, int>("[creature skill over charge time rate]", 0x2ef8));
            __lookup.insert(std::pair<char const* const, int>("[creature experience amount rate]", 0x2efb));
            __lookup.insert(std::pair<char const* const, int>("[creature physical critical hit]", 0x2efe));
            __lookup.insert(std::pair<char const* const, int>("[creature magical critical hit]", 0x2f01));
            __lookup.insert(std::pair<char const* const, int>("[creature stuck]", 0x2f04));
            __lookup.insert(std::pair<char const* const, int>("[creature piercing]", 0x2f07));
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
                __lookup.insert(std::pair<char const* const, int>("monster card", 0x30ae));
                __lookup.insert(std::pair<char const* const, int>("cannot bind", 0x30b1));
                __lookup.insert(std::pair<char const* const, int>("boss drop", 0x30b4));
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
            __lookup.insert(std::pair<char const* const, int>("[none]", 0x30d5));
            __lookup.insert(std::pair<char const* const, int>("[solo teleport]", 0x30db));
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
        /* ORIG 0x89c1e63..：格式串 = def.member（脚本模板），参数 = path */     \
        snprintf(buf, sizeof(buf), def.member.c_str(), path->c_str());         \
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
        // ORIG 0x89c2601..0x89c2634：格式串 = it 元素，参数 = path
        snprintf(buf, sizeof(buf), it->c_str(), path->c_str());
        ani->m_vec64.push_back(std::string(buf));
    }
    for (std::vector<std::string>::const_iterator it = def.m_vec70.begin();
         it != def.m_vec70.end(); ++it)
    {
        // ORIG 0x89c272b..0x89c275e：同上
        snprintf(buf, sizeof(buf), it->c_str(), path->c_str());
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
                __lookup.insert(std::pair<char const* const, int>("enchanter", 0x303a));
                __lookup.insert(std::pair<char const* const, int>("alchemist", 0x303d));
                __lookup.insert(std::pair<char const* const, int>("disjointer", 0x3040));
                __lookup.insert(std::pair<char const* const, int>("doll_controller", 0x3043));
                __lookup.insert(std::pair<char const* const, int>("all", 0x3046));
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
