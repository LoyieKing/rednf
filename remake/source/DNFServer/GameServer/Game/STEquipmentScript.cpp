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
// [Lane23 2026-09-10 收口] 本轮 8 件 DIFF/NEAR → AE（四数 710/1439/2/7/464；
//   基线 710/1431/3/14/464 ⇒ ae+8 / near−1 / diff−7，零回退）：
//   EquipmentScriptSpecialMonsterDrop 176→172（共享 `return true`：if→else-if 链）；
//   GetAuraAvatarOption 259→256、GetExpertJobTypeList 329→327（P333 static 三件套
//   移入重查循环体 + P164 内层 for 替 goto + 独立 int v 判别值）；
//   importArtifactInfo 450→448（find 内联 c_str 实参 + break 出循环 + 循环外 return）；
//   importEffect 314→307（[/module]/[file name] 改落链尾，消 4.4 的 nop+jmp 反向块）；
//   importCommand 259→254（while((token=getToken())!=0) 条件段单点取 token +
//   name 内联为 find 实参）；
//   GetExpertJobValueMaterialVariation 29→27（x87 比较写 `-100.0f >= field` 交换
//   操作数方向，复现 4.1 `test $0x5,%ah; sete`）；
//   setEquipmentAniScriptLay NEAR→AE（rbegin() 具名 reverse_iterator 复位深槽）。
//   余 2 件游戏域：ImportEquipmentScript（帧 0x19fc vs 0x1a3c，N82 槽复用族）、
//   GetItemCategory（state 占 4.1 深槽 -0x78 vs 4.4 浅槽 -0x20）。
//   [补 2026-09-10] ImportEquipmentScript prologue 顺序修正：stopFlag/curParam/
//   setAbilityBackup 的声明点移到 m_str664 赋值之后（ORIG idx 132..138 同序，
//   消旧形 prologue 多发的 12 条），upgMin/upgMax/upgType 删声明处初值
//   （ORIG 初值在 [upgrade] 段 idx 7992..8000 每次进入时重置，本文件同序已有）。
//   四数不变（710/1439/2/7/464）；该件余量仍为全帧 0x40 槽差（1451 差异块/5579 行）。
// [SweepSTEquip 2026-09-09 单文件泳道收口判词] 本轮 13 件逐件读数：
//   ■ 真缺口修复 8 件（gap 降序）：
//     setEquipmentAniScriptLay 44→2（find 传 key->c_str() 临时 string /
//       rbegin()->push_back 语义纠错 / 迭代器先声明后赋值 / 两循环复用 it）；
//     GetItemCategory 32→5、GetAuraAvatarOption 32→5、GetExpertJobTypeList
//       18→10（三件同为 P333「首次建表后查表」switch 状态机还原：哨兵 case 0
//       与首 key 同体 + 每 case `movzbl __inited; xor $1` + fallthrough 链 +
//       尾块 leave→__inited→goto retry；余量为 4.4 EH landing（EXTRA
//       _Unwind_Resume×2，N104）与帧差（N115/N126））；
//     STEquipmentAniScript::C1Ev 22→0（375=375）、C1ERKS_ 433=433、
//       D1Ev 332=332、aSERKS_ 180=180、clear ?→0（118=118）——P151 数组成员
//       建模：+0x10/+0x14→m_arr10[2]、+0x2c..+0x38→m_arr2c[4]、
//       +0x3c..+0x48→m_arr3c[4]（.h 改动已广播；sizeof 0x7c/偏移零变化）。
//   ■ 余量为结构性 N（p167 调用点差集已 CLEAR，仅剩 C1↔C2/D1↔D2 同址别名命名差）：
//     importEffect gap 4、setEquipmentAniScriptLay gap 2（帧 0x25c vs 0x26c，
//     N126/N115）、GetExpertJobTypeList gap 10（guard 位置跨版本 + 帧 0xac vs 0xbc）、
//     importCommand gap −4（4.4 多 4 条槽/声明序）、EquipmentScriptSpecialMonsterDrop
//     gap −4、GetExpertJobValueMaterialVariation gap −2（N85 x87 lowering）。
//   ■ 前泳道已修、本泳道未重开：importArtifactInfo（P333 已落）、ImportEquipmentScript。
//   TU：check_tu_routed.sh COMPILE OK 0 错误，identical=710 ae=1431 near=2
//     diff=15 missing_in_game=464（基线 709/1426/2/20/468 ⇒ ae+5、diff−5、miss−4，零回退）。
//   P0：本文件零 `register asm` 绑定、零裸 register（清绑数 0）。
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
    // [b28d-solo-STEquipmentScript] 旧 N81/P30 判词作废：ORIG 三处循环体发
    // `mov i,%edx; mov this,%eax; add $K,%edx; movl $0,disp(%eax,%edx,4)`，
    // 由头内 +0x08 起的二维成员数组视角 m_view[9][4] 复现（P311；探针
    // /tmp/b28d_s219/probe1.cpp T::h1/h2/h3 与 ORIG 逐条同形）。字段偏移不变。
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
        m_view[2][i] = 0;           // +0x28..+0x34 元素耐性（字索引 8+i）
    }
    m_refAll = 0;                   // +0x38
    for (int j = 0; j <= 16; ++j)
    {
        m_view2[3][j] = 0;          // +0x3c..+0x7c 状态耐性（视图基址 +0x0c，索引 12+j）
    }
    m_refAllState = 0;              // +0x80
    m_84 = 0;                       // +0x84
    for (int k = 0; k <= 3; ++k)
    {
        m_view[8][k] = 0;           // +0x88..+0x94（字索引 32+k）
    }
}

STEquipmentAniScript::STEquipmentAniScript()
{
    // [SweepSTEquip 2026-09-09 旧判词 N4/P235 作废] C1 0x0898ca36 375 条：
    // ORIG 对 +0x10/+0x2c/+0x3c 三个连续 string 区发**倒计数构造循环**
    // （0x0898ca42 `lea 0x10(%eax),%esi; mov $0x1,%ebx; … sub $1,%ebx;
    // cmp $-1,%ebx; jne`，P151）。正解不是 union 视图而是**真数组成员**：
    // .h 已把 +0x10/+0x14→m_arr10[2]、+0x2c..0x38→m_arr2c[4]、
    // +0x3c..0x48→m_arr3c[4]（sizeof 0x7c 与全部偏移不变，隐式拷贝/赋值/
    // 析构仍由编译器合成）。修后 375=375 逐条同形；同族 C1ERKS_ 433=433、
    // D1Ev 332=332、aSERKS_ 180=180（原 192）、clear 118=118 全部归零。
    clear();
}

void STEquipmentAniScript::clear()  // ORIG 0x0898ce62（m_field0=0，各 string 置 ""）
{
    // [SweepSTEquip 2026-09-09] ORIG 形：`.clear()` 实为 `s = ""`（operator=(const
    // char*)，实参 "" 落 0x4(%esp)）；第一个循环 `i<=1` 只清 m_arr10，第二个循环
    // `i<=3` 同体清 m_arr2c[i]+m_arr3c[i]（0x0898cf65/0x0898cf84 两个赋值）；
    // +0x60 在 vector clear 之后（0x0898d026）。修后 118=118 逐条同形。
    m_field0 = 0;
    m_str04 = "";
    m_str08 = "";
    m_str0c = "";
    // ORIG 0x0898ceb3/0x0898ceba 两次 `movl $0x0,-0xc(%ebp)` ⇒ 具名 `int i = 0;`
    // 后接 `for (i = 0; …)`；第二个循环（0x0898cf5c）只清一次 ⇒ 复用同一 i。
    // 第二个循环体含**两个**数组赋值（0x0898cf65 m_arr2c[i] + 0x0898cf84
    // m_arr3c[i]），同一 i 同一循环。
    int i = 0;
    for (i = 0; i <= 1; ++i)
    {
        m_arr10[i] = "";
    }
    m_str18 = "";
    m_str1c = "";
    m_str20 = "";
    m_str24 = "";
    m_str28 = "";
    for (i = 0; i <= 3; ++i)
    {
        m_arr2c[i] = "";
        m_arr3c[i] = "";
    }
    m_str4c = "";
    m_str50 = "";
    m_str54 = "";
    m_str58 = "";
    m_vec64.clear();
    m_vec70.clear();
    // ORIG 0x0898ce62 未清 +0x5c（拷贝构造/operator= 均拷贝 +0x5c，故 +0x5c 是 string
    // 成员，仅 clear 漏清——按 ORIG 实证保留不清）
    m_str60 = "";
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
    // [b28c-27] AE（fresh 复验）：562/562 逐条同，.o 重定位归一后与 ORIG 全等
    // （成员构造/清零/EH 链序一致）。
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
    // [b28d-solo-STEquipmentScript] ORIG 形 = 具名 bool 局部 + `||` 短路链
    // （0x0898f0fe：逐条件 jne 共享 mov$1 块 → mov $0 岛 → 尾 `mov %al,-0x1(%ebp);
    // movzbl`，sub $0x10）。float 项必须写 `!(x == 0.0f)`（否定式）：4.4 才发
    // `fucompp; fnstsw; sahf; sete; setnp; and; xor $1; test; jne` 与 ORIG 逐条同形；
    // 写 `x != 0.0f` 则折成 `fxch; jne; jp` 直跳（探针 /tmp/b28d_s219/probe5.cpp c1 vs c2）。
    bool usable = (m_minLevel != 0) || (m_physAttack != 0) || (m_magAttack != 0) ||
                  !(m_mpRate == 0.0f) || !(m_chargeRate == 0.0f) ||
                  !(m_overChargeRate == 0.0f) || !(m_expRate == 0.0f) ||
                  !(m_physCrit == 0.0f) || !(m_magCrit == 0.0f) ||
                  !(m_stuck == 0.0f) || !(m_piercing == 0);
    return usable;
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
    // [b28d-solo-STEquipmentScript 判词·N] §6.2 三件套（0x08991aeb T）：
    // ①gap：ORIG 9771 条 vs 我们 9838 条（帧 0x19fc vs 0x1a2c，巨件全帧槽号平移）；
    // ②p167：ORIG calls=1292 / 我们 1300，MISSING 仅 2 个 pair converting ctor 实例
    //   （`pair<char const(&)[10],int>` / `[11]`，模板实例化差非语义缺口），EXTRA
    //   含 EquipmentScriptSpecialMonsterDrop ×2（三 case 各自调用点 vs ORIG 单共享块）
    //   与 STL 析构/拷贝 ctor 计数差；③槽位集/帧差属 N82/N89。语义完成，byte 层封顶。
    // [BigEquip pass-1 修订 2026-09-09] ③中 N89 判错：两侧 call 面实测对齐
    //   （1292 vs 1291，差集=snprintf 名/模板实例名/ABI guard 等无害项；9 个
    //   _ZL Get* helper 两侧均外调）。真实残差=N82 帧槽平移（+0x24/+0x1c/+0x54
    //   非恒定；ORIG 浅区 23 个独立 return-temp 槽，我侧 13=4.4 lifetime 复用）。
    //   本 pass 语义/形差修正：删 26 处 ORIG 无的 okFlag 预清零；[emancipate]/
    //   [war room point bonus]/[war room monster exp bonus] 回写 ScanType/ScanStr
    //   结果到 boolScratch；[hide layer]/[hide growtype avatar]/[change status]
    //   拆独立出参 bool（ORIG -0x10d1/-0x10d9/-0x1145）；[spectrum] v4 补 &0xff
    //   （ORIG movzbl 实证）；[Uncompoundable]/[setback img]/[set item]/[int data]/
    //   [fixed select type]/[avatar type select]/[avatar func filter]/[special
    //   ability]/[chat emoticon index]/[required full set]/[character item check]/
    //   [/routing priority]/[epic routing] 实参统一 boolScratch。台账
    //   ndiff 13237→13134、gap -67→-42。
    //   ⚠ 实测三→一 MonsterDrop 共享块合并（-71 insn、gap +23→-23、calls 1=ORIG）
    //   反使 ndiff 13134→13593（difflib 对齐窗口恶化，§6.2 失真实证）⇒ 已回滚；
    //   若要结构保真优先请 Main 裁定改台账口径后重做。
    // [BigEquipScript pass-2 2026-09-09] 按 §6.2 三件套重做 MonsterDrop 合并
    //   （指令数收敛优先，N127 台账 ndiff 不作成败判据）：
    //   ①三 case [special monster drop]/[difficulty for extra drop]/[dungeon type
    //     for extra drop] 的 else 分支 `goto special_monster_drop;` 汇入 0xcd7
    //     else 内共享块（ORIG 0899a83f 共享块、0899a86e 唯一调用点实证）⇒
    //     calls 1300→1292=ORIG（p167 修正版；余差集仅 C1↔C2/D1↔D2 同址别名镜像
    //     对，P93 豁免）；our_insns 9865→9799、gap −44→+22。
    //   ②[emblem socket default] 0x08999a3e / [avatar type select] 0x08999c00 两处
    //     `5 < size()` 删冗余 (int) cast ⇒ seta（无符号，与 ORIG 同；原 setg）。
    //   ③[spectrum] 08992ba9 位组合 `&0xff`→(unsigned char) cast ⇒ 复现 ORIG 逐项
    //     movzbl（消 3 and、movzbl 差 6→2）。
    //   残余 +22 = N82 帧槽族（+0x24/+0x1c/+0x54 非恒定）+ 分散 movb(+6)/add(+10)
    //   形态差；difflib 对本件失真（N127），逐 case 段对齐不可用（字符串段界
    //   ORIG 225 vs 我方重定位段 520）⇒ 封 N。
    // [SweepSTEquipScript 2026-09-09 复核] gap 22 保持（9821/9799）；p167 差集仅
    //   C2/D2 ↔ C1/D1 同址别名镜像（P93）；三个惰性表符号 __lookup/__lookup_0/
    //   __lookup_1 + __init_lock×3 + __inited×3 与 ORIG 名字/数量逐一对应（P333
    //   状态机结构完整）；帧 0x1a3c vs ORIG 0x19fc（N82 槽复用）。13 个 missing
    //   段抽查（08991f60/0899925d/0899b26e/0899b3ed 等）均为 4.1 switch 决策树与
    //   叶子块布局差，非语义缺口 ⇒ 维持封 N。
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
    bool hideFlag774;                      // ORIG -0x10d1（[hide layer] 循环出参，ORIG 无显式初始化）
    bool hideFlag780;                      // ORIG -0x10d9（[hide growtype avatar] 循环出参）
    bool rateFlag;                         // ORIG -0x1145（[change status on change hp] 循环出参）
    // [2026-09-10 顺序修正] stopFlag/curParam/setAbilityBackup 的声明点见下方
    // m_str664 赋值之后（ORIG idx 132..138）；upgMin/upgMax/upgType 初值不在
    // 声明点：ORIG prologue 无 `movl $-1/-1/5`，
    // 初值由 [upgrade] 段每次进入时重置（ORIG idx 7992..8000：`upgMin=-1;
    // upgMax=-1; ScanInt(&upgMin); ScanInt(&upgMax); upgType=5;`，本文件同序已存在）。
    int upgMin;                            // ORIG -0x1160
    int upgMax;                            // ORIG -0x1164
    int upgType;                           // ORIG -0x1168
    char printBuf[0x200];                  // ORIG -0x1550

    if (!loadRDARScriptFile(kEquipmentScriptBaseDir, path))
    {
        return false;
    }
    script->Clear();
    getDirectoryPath(scanBuf, "%s%s", kEquipmentScriptBaseDir, path);
    script->m_str664 = scanBuf;

    // [2026-09-10 顺序修正] ORIG 的声明点在此（idx 132..138，for 之前）；初始化
    // 指令序 = curParam → setAbilityBackup(构造) → stopFlag（见上注释）。
    EquipmentParameterInfo* curParam = &script->m_param1c0;
    std::vector<std::pair<int, int> > setAbilityBackup;
    bool stopFlag = false;

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
            unsigned int v4 = (unsigned int)ScanInt((bool*)0);
            // ORIG 08999c... 逐项 movzbl %al,%eax（零扩展）⇒ 用 (unsigned char)
            // cast 而非 &0xff（后者生 and，助记符不符）。
            script->m_790 = (int)((unsigned char)v1 |
                                  ((unsigned char)v2 << 8) |
                                  ((unsigned char)v3 << 0x10) |
                                  ((unsigned char)v4 << 0x18));
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
                int v = ScanInt(&hideFlag774);
                if (!hideFlag774)
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
                int v = ScanInt(&hideFlag780);
                if (!hideFlag780)
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
            int v = ScanInt(&boolScratch);
            if (boolScratch && v == 1)
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
                        jobLayer.m_arr10[0] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr10[0] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr10[0] = printBuf;
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
                        jobLayer.m_arr10[1] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr10[1] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr10[1] = printBuf;
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
                        jobLayer.m_arr2c[0] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[0] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[0] = printBuf;
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
                        jobLayer.m_arr3c[0] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[0] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[0] = printBuf;
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
                        jobLayer.m_arr2c[1] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[1] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[1] = printBuf;
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
                        jobLayer.m_arr3c[1] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[1] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[1] = printBuf;
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
                        jobLayer.m_arr2c[2] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[2] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[2] = printBuf;
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
                        jobLayer.m_arr3c[2] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[2] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[2] = printBuf;
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
                        jobLayer.m_arr2c[3] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[3] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[3] = printBuf;
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
                        jobLayer.m_arr3c[3] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[3] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[3] = printBuf;
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
                    info.m_a = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    info.m_b = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    info.m_c = ScanInt(&boolScratch);
                    if (!boolScratch)
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
                    int v = ScanInt(&boolScratch);
                    if (!boolScratch)
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
                int key = ScanInt(&boolScratch);
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
                    int v = ScanInt(&boolScratch);
                    if (!boolScratch)
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
                script->m_6e0 = ScanInt(&boolScratch);
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
                if (5 < script->m_vec6cc.size())
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
                    sel.m_field0 = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    sel.m_field4 = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                    sel.m_field8 = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                    sel.m_fieldc = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                    sel.m_field10 = ScanInt(&boolScratch);
                    if (!boolScratch || 5 < sel.m_field10)
                    {
                        return false;
                    }
                    if (0 < sel.m_field10)
                    {
                        if (5 < script->m_vec6cc.size() + sel.m_field10)
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
                script->m_6d8 = ScanInt(&boolScratch);
                if (!boolScratch)
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
                    boolScratch = ScanType(key, true);
                    script->m_field10e = 1;
                    bool cont;
                    if (key == "[input]")
                    {
                        for (;;)
                        {
                            int first = ScanInt(&boolScratch);
                            if (!boolScratch)
                            {
                                break;
                            }
                            int second = ScanInt(&boolScratch);
                            if (!boolScratch)
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
                            int first = ScanInt(&boolScratch);
                            if (!boolScratch)
                            {
                                break;
                            }
                            int second = ScanInt(&boolScratch);
                            if (!boolScratch)
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
                int v = ScanInt(&boolScratch);
                script->m_field104 = (char)(v != 1);
                if (!boolScratch)
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
                            rateFlag = true;
                            rate.m_rate1 = (float)ScanInt(&rateFlag);
                            rate.m_rate3 = (float)ScanInt(&rateFlag);
                            rate.m_rate2 = (float)ScanInt(&rateFlag);
                            rate.m_rate4 = (float)ScanInt(&rateFlag);
                            if (rateFlag)
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
                script->m_7bc = (ScanInt(&boolScratch) == 1);
                if (!boolScratch)
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
                script->m_7c0 = ScanInt(&boolScratch);
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
                script->m_7c4 = ScanInt(&boolScratch);
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
                int v = ScanInt(&boolScratch);
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
                case 0xcd5:                          // [special monster drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[special monster drop]", 0xcd5));
                }
                else
                {
                    goto special_monster_drop;
                }
                    /*FALLTHRU*/
                case 0xcd6:                          // [difficulty for extra drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[difficulty for extra drop]", 0xcd6));
                }
                else
                {
                    goto special_monster_drop;
                }
                    /*FALLTHRU*/
                case 0xcd7:                          // [dungeon type for extra drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[dungeon type for extra drop]", 0xcd7));
                }
                else
                {
                special_monster_drop:;
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
                    entry.m_a = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    entry.m_b = ScanInt(&boolScratch);
                    if (!boolScratch)
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
                case 0xd7d:                          // [particle]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[particle]", 0xd7d));
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
                case 0xd89:                          // [command]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[command]", 0xd89));
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
                    int pri = ScanInt(&boolScratch);
                    if (!boolScratch)
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
                if (boolScratch = ScanStr(&scanBuf))
                {
                    script->m_wpBonus81c.m_type = getCalculationType(scanBuf.c_str());
                    if (script->m_wpBonus81c.m_type == 2)
                    {
                        return false;
                    }
                    script->m_wpBonus81c.m_value = ScanInt(&boolScratch);
                    if (!boolScratch)
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
                if (boolScratch = ScanStr(&scanBuf))
                {
                    script->m_expBonus824.m_type =
                        getCalculationType(scanBuf.c_str());
                    if (script->m_expBonus824.m_type == 2)
                    {
                        return false;
                    }
                    script->m_expBonus824.m_value = ScanInt(&boolScratch);
                    if (!boolScratch)
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
    // [b28d-solo-STEquipmentScript] ORIG 0x08991856（与 StackableScriptSpecialMonsterDrop
    // 0x89ae906 同构，目标为 STItemScript::m_specialDrop +0x105）：
    // ①函数头只构造一个 string 局部（-0x10），各分支复用（P28/P212 全头部声明）；
    // ②禁 `STSpecialMonsterDrop& drop` 引用缓存（P183）——每个使用点直写
    //   `script->m_specialDrop.xxx`，才复现 ORIG `add $0x105,%eax` + `mov %dl,0x105(%eax)`。
    (void)path;
    std::string token;
    if (str == "[special monster drop]")
    {
        bool value = false;
        bool ok = ScanInt(&value) != 0;
        if (value == false)
        {
            return false;
        }
        script->m_specialDrop.m_enable = ok;   // P136/P32：bool 字段直存（ORIG movzbl→mov %dl）
    }
    else if (str == "[difficulty for extra drop]")
    {
        while (ScanStr(&token))
        {
            if (token == "[easy]")
            {
                script->m_specialDrop.m_difficulty[0] = 1;
            }
            else if (token == "[medium]")
            {
                script->m_specialDrop.m_difficulty[1] = 1;
            }
            else if (token == "[hard]")
            {
                script->m_specialDrop.m_difficulty[2] = 1;
            }
            else if (token == "[ultimate]")
            {
                script->m_specialDrop.m_difficulty[3] = 1;
            }
            else if (token == "[hero]")
            {
                script->m_specialDrop.m_difficulty[4] = 1;
            }
            else if (token == "[all]")
            {
                memset(script->m_specialDrop.m_difficulty, 1,
                       sizeof(script->m_specialDrop.m_difficulty));
            }
        }
    }
    else if (str == "[dungeon type for extra drop]")
    {
        while (ScanStr(&token))
        {
            if (token == "[normal]")
            {
                script->m_specialDrop.m_dungeonType[0] = 1;
            }
            else if (token == "[ancient]")
            {
                script->m_specialDrop.m_dungeonType[1] = 1;
            }
            else if (token == "[dimension]")
            {
                script->m_specialDrop.m_dungeonType[2] = 1;
            }
            else if (token == "[all]")
            {
                memset(script->m_specialDrop.m_dungeonType, 1,
                       sizeof(script->m_specialDrop.m_dungeonType));
            }
        }
    }
    return true;
}

bool importArtifactInfo(STEquipmentScript::artifact_info& info,
                        std::string str)
{
    // [SweepC 2026-09-09 P333 首次建表状态机还原] 0x089bd079：ORIG 450 / 我方 354
    // （gap 96；p167 双向 0/0 ⇒ 纯控制流形态差）。ORIG 真形 = 静态表在**循环体内**
    // 声明（guard 检查随 continue 回跳重放，0x089bd085）+ 11 个 case 链式 fallthrough
    // 注册：`if (__inited) { 读值; break; } insert(...); // fallthrough`；
    // 尾块 0x089bd80d：`if (!__inited) { leave(0x089bd822); __inited=true; continue; }`。
    // 表：map<const char*,int,less_TCHAR> @0x94fdf28 + SpinLock @0x94fdf40 +
    // 就绪字节 @0x94fdf44；11 个 [creature *] 关键字 → id（0x2ee9..0x2f07），
    // 未命中返回 0，命中按 id 分派 ScanInt/ScanFloat 写回。
    // 扫描参数：3 个 int 字段 ScanInt(0)；7 个 rate 字段 ScanFloat(scale,0)
    // ——mp/charge/overcharge/exp 用 scale=2，crit×2/stuck 用 scale=1。
    do
    {
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int id;
        if (__inited)
        {
            // [P236 2026-09-10] ORIG 0x089bd0? 的 c_str 结果槽（-0x104）在 find 的
            // sret 槽（-0x108）**之上**：原码是内联实参 `find(str.c_str())`
            // （临时槽序 = key 浅 / sret 深），具名 key 局部反装槽序。
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(str.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                              // 回循环头（重放 guard）
            }
            id = 0;
        }

        switch (id)
        {
        case 0:
        case 0x2ee9:                                   // [creature minimum level]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature minimum level]", 0x2ee9));
            }
            else
            {
                info.m_minLevel = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        case 0x2eec:                                   // [creature physical attack]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature physical attack]", 0x2eec));
            }
            else
            {
                info.m_physAttack = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        case 0x2eef:                                   // [creature magical attack]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature magical attack]", 0x2eef));
            }
            else
            {
                info.m_magAttack = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        case 0x2ef2:                                   // [creature skill consume mp rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature skill consume mp rate]", 0x2ef2));
            }
            else
            {
                info.m_mpRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2ef5:                                   // [creature skill charge time rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature skill charge time rate]", 0x2ef5));
            }
            else
            {
                info.m_chargeRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2ef8:                                   // [creature skill over charge time rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature skill over charge time rate]", 0x2ef8));
            }
            else
            {
                info.m_overChargeRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2efb:                                   // [creature experience amount rate]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature experience amount rate]", 0x2efb));
            }
            else
            {
                info.m_expRate = ScanFloat(2, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2efe:                                   // [creature physical critical hit]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature physical critical hit]", 0x2efe));
            }
            else
            {
                info.m_physCrit = ScanFloat(1, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2f01:                                   // [creature magical critical hit]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature magical critical hit]", 0x2f01));
            }
            else
            {
                info.m_magCrit = ScanFloat(1, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2f04:                                   // [creature stuck]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature stuck]", 0x2f04));
            }
            else
            {
                info.m_stuck = ScanFloat(1, (bool*)0);
                break;
            }
            // fallthrough
        case 0x2f07:                                   // [creature piercing]
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>(
                    "[creature piercing]", 0x2f07));
            }
            else
            {
                info.m_piercing = ScanInt((bool*)0);
                break;
            }
            // fallthrough
        default:
            if (__inited)
            {
                return false;                          // ORIG 未命中 → 0
            }
            // fallthrough 到尾块（ORIG 0x089bd7fb）
        }

        if (!__inited)
        {
            // ORIG 0x089bd822/0x089bd827：leave → __inited=true → 回循环头
            __init_lock.leave();
            __inited = true;
            continue;
        }
        break;                                         // 已建表 → 退出并返回 true
    } while (true);
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
    // [SweepSTEquip 2026-09-09 P333 状态机还原] ORIG 0x089bfdc5 是「首次建表、
    // 之后查表」的 switch 状态机，不是直线双检锁：
    //   ① 循环体首 `state(-0x78)=0`（0x089bfde1），retry 锚 0x089bfde8；
    //   ② 已初始化走 find → `state = it!=end ? it->second : -1`（0x089bff7e
    //      汇入 switch）；未初始化 enter 后若已被他线程建表则 leave+回跳
    //      （0x089bffa3 nop / 0x089bffa4 jmp retry），否则 `state = 0` 哨兵；
    //   ③ switch(state)：case 0 与 case 0x30ae 同体，每 case 体头
    //      `movzbl __inited; xor $1; test; je INSERT`；已初始化则
    //      `state = 1/2/3` 后 break，否则 insert 并 fallthrough 下一 case
    //      （0x089c002d/0x089c0087/0x089c00de 的 jmp 下一 case 体头）；
    //   ④ default：`if (__inited) return false`（0x089c00e9），未初始化则落尾；
    //   ⑤ 尾块 `if (!__inited) { leave; __inited = true; goto retry; }`
    //      （0x089c00fe..0x089c011f），随后 `if (state != 0) insert`。
    // 旧译文把建表写成一次性三连 insert，缺整棵 switch 与回跳 ⇒ gap 32。
    // [SweepSTEquipScript 2026-09-09] 三处 case 体极性反转（同 GetExpertJobTypeList）：
    // gap 5 → -1（254/255）。残差 1 条 = ORIG `mov slot,%eax; test %eax,%eax; je`
    // 被 4.4 折成 `cmpl $0,slot; je`（P209/N18b 跨版本折叠，无源码解）。
    // [P28/P212 全头部声明 2026-09-10] ORIG 的 state 占深槽 -0x78（与 token -0x74
    // 同区），声明在函数级；循环体首只做 `state = 0`（ORIG 0x089bfde1）。
    // [E06 2026-09-10 实验·翻案失败] state 标量形恒落浅区 -0x20、POD 聚合包装版
    // 254（劣 1 条且槽未回深区）、unsigned 形 253 与 int 形同值 ⇒ 按 P321-S1
    // （赌输恢复直证形）恢复 int，N82附/N93（4.4 聚合优先低槽+标量分区）封顶。
    int state;
    std::string token;
    int v;                      // switch 判别值（ORIG -0x1c）
    while (ScanStr(&token))
    {
        state = 0;
        // [P164 真解 2026-09-10，Main 广播] `retry: … goto retry` 会令 4.4 在循环头
        // 前插 entry jmp+nop；改为内层 `for (;;)`（guard 竞争 `continue`、重派发块
        // `if(!__inited){…} else {break;} continue;`，continue 置于内层体末）。
        for (;;)
        {
        // [P333 修正 2026-09-10] ORIG 的 static 三件套声明在**循环体内**（retry 锚
        // 处）：guard 检查随 `goto retry` 逐轮重放（ORIG 0x089bfe62/0x089bfedc 的
        // `mov $__lookup_guard`/`movzbl; test; jne` 位于 retry 锚 +0x023），
        // 函数入口只有 token 构造 + `jmp 循环条件`（0x089bfdd1..0x089bfddc）。
        // 旧形把三件套声明在函数头 ⇒ guard 段被提到函数入口（token 构造之前），
        // 与 ORIG 顺序相反。声明位置不影响静态初始化语义（仍只初始化一次）。
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        // [P333 修正 2026-09-10] ORIG 有两个 int：`state`（-0x78，case 体里写
        // ITEM_CATEGORY_1/2/3）与 switch 判别值 `v`（-0x1c），两者同为函数级。
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            v = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                              // 自旋重试（P333：重放 guard）
            }
            v = 0;                                     // 建表哨兵
        }
        switch (v)
        {
        case 0:                                        // 与首 key 同体（P163）
        case 0x30ae:                                   // "monster card"
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("monster card", 0x30ae));
            }
            else
            {
                state = STItemScript::ITEM_CATEGORY_1;
                break;
            }
            /* FALLTHRU */
        case 0x30b1:                                   // "cannot bind"
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("cannot bind", 0x30b1));
            }
            else
            {
                state = STItemScript::ITEM_CATEGORY_2;
                break;
            }
            /* FALLTHRU */
        case 0x30b4:                                   // "boss drop"
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("boss drop", 0x30b4));
            }
            else
            {
                state = STItemScript::ITEM_CATEGORY_3;
                break;
            }
            /* FALLTHRU */
        default:
            if (__inited)
                return false;                          // 未命中 → 导入失败
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
        }
        else
        {
            break;                                     // 已建表 → 退出内层重查循环
        }
        continue;                                      // 建表后重查本 token
        }
        if (state != 0)
        {
            item->m_set11c.insert(
                static_cast<STItemScript::ENUM_ITEM_CATEGORY>(state));
        }
    }
    return true;
}

// ---- ORIG t 0x089c019b：[aura ability] 单标签分派 → script->m_880 模式 +
//      m_884 参数。同型惰性标签表（map @0x94fde08 / SpinLock @0x94fde20 /
//      就绪字节 @0x94fde24）。ScanStr 返回值不检查——空 token 必然 miss，
static bool GetAuraAvatarOption(STEquipmentScript* script)
{
    // [SweepSTEquip 2026-09-09 P333 状态机还原] ORIG 0x089c019b 与 GetItemCategory
    // 同型：单次 ScanStr（返回值不检查）→ guard → retry 锚 0x089c01ce →
    // switch(state)：case 0 与 0x30d0 同体；每 case 体头
    // `movzbl __inited; xor $1; test; je INSERT`，已建表分支做业务写入
    // （0x089c0419 m_880=0 + ScanInt / 0x089c048a m_880=1 m_884=0 /
    // 0x089c04f4 m_880=2 + ScanInt）后 break；未建表 insert 并 fallthrough；
    // default `if (__inited) return false`；尾块 `if (!__inited) { leave;
    // __inited = true; goto retry; }`（0x089c0526..0x089c0547）。
    // 旧译文建表三连 insert 直线形，缺整棵 switch 与回跳 ⇒ gap 32。
    // [SweepSTEquipScript 2026-09-09] 三处 case 体极性反转（同 GetExpertJobTypeList）：
    // gap 5 → -1（258/259）。残差 1 条 = P209/N18b 零比较折叠（同上件）。
    if (script == 0)
        return false;

    bool ok = true;
    std::string token;
    ScanStr(&token);

    // [P164/P333 修正 2026-09-10] static 三件套移入重查循环体内（ORIG guard 段
    // 0x089c0248/0x089c02c2 在 retry 锚 +0x033；`goto retry` 改内层 `for (;;)`
    // 消 entry jmp+nop）；switch 判别值用独立无初始化 `int v`（ORIG -0x1c）。
    for (;;)
    {
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited = false;
    int v;
    if (__inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(token.c_str());
        v = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();
        if (__inited)
        {
            __init_lock.leave();
            continue;                              // 自旋重试（P333：重放 guard）
        }
        v = 0;                                     // 建表哨兵
    }
    switch (v)
    {
    case 0:                                        // 与首 key 同体（P163）
    case 0x30d0:                                   // "[party teleport]"
        if (!__inited)
        {
            // P61/P81：ORIG 用 pair 直构（MISSING pair<char const(&)[17],int> converting
            // ctor），make_pair 会实例化另一模板并多一次转换临时。
            __lookup.insert(std::pair<char const* const, int>("[party teleport]", 0x30d0));
        }
        else
        {
            script->m_880 = 0;
            script->m_884 = ScanInt(&ok);
            break;
        }
        /* FALLTHRU */
    case 0x30d5:                                   // "[none]"
        if (!__inited)
        {
            __lookup.insert(std::pair<char const* const, int>("[none]", 0x30d5));
        }
        else
        {
            script->m_880 = 1;
            script->m_884 = 0;
            break;
        }
        /* FALLTHRU */
    case 0x30db:                                   // "[solo teleport]"
        if (!__inited)
        {
            __lookup.insert(std::pair<char const* const, int>("[solo teleport]", 0x30db));
        }
        else
        {
            script->m_880 = 2;
            script->m_884 = ScanInt(&ok);
            break;
        }
        /* FALLTHRU */
    default:
        if (__inited)
            return false;                          // 未命中 → 导入失败
        break;
    }
    if (!__inited)
    {
        __init_lock.leave();
        __inited = true;
    }
    else
    {
        break;                                     // 已建表 → 退出内层重查循环
    }
    continue;                                      // 建表后重查本 token
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
    // [b28d-solo-STEquipmentScript 判词·N] 0x088c167a：ORIG 255 条 vs 我们 259 条
    // （槽 -0x34 vs -0x38、局部 string/vector 声明序差）。p167 已清零（41/42，
    // 0 missing；修复=非 const iterator + `(*it).second`，P96/P167；残余 EXTRA
    // DNFLexWrapper::getToken ×1 = 循环入口求值点差，N82）。语义完成，byte 层封顶。
    if (command == 0)
        return false;

    STKeyCommandUnit unit;
    command->m_vec0.clear();

    // [2026-09-10] ORIG 0x088c1692..0x088c16a8 循环头直接 `jmp +0x315`，getToken
    // 只在**条件段**（0x088c198f..0x088c19a7：`call getToken; mov %eax,-0x10;
    // cmpl $0,-0x10; setne %al; test %al,%al; jne 循环体`）⇒ 源形是
    // `while ((token = g_dnfLex.getToken()) != 0)`（P164/P225 四段物化），
    // 不是「循环前取一次 + 循环尾再取」。旧形多发 3 条（循环前 getToken+store）。
    int token;
    while ((token = g_dnfLex.getToken()) != 0)
    {
        if (token == 5)
            break;

        if (token == 6)
        {
            // P96/P220：ORIG 用非 const iterator（p167 MISSING _Rb_tree_iterator::operator!=
            // / operator*，EXTRA const_iterator 转换构造 + const 版比较）。
            // [2026-09-10] name 内联为实参临时（ORIG 0x088c16c1..0x088c16f1 的 string
            // 构造/-0x20 槽紧贴 find 调用；具名局部使 4.4 把 string 排到深槽 -0x34）。
            std::map<std::string, int>::iterator it =
                g_mapKeyNameIndexTable.find(toTString(g_dnfLex.getCustomData()));
            if (it != g_mapKeyNameIndexTable.end())
                unit.m_ids.push_back((*it).second);   // P167：ORIG 实例化 operator* 非 operator->
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
    }

    if (!unit.m_ids.empty())
        command->m_vec0.push_back(unit);
    return true;
}

// ---- ORIG T 0x088c1a0d：[effect] 条目解析 ----
bool importEffect(effect::STEffect& effect)
{
    // [b28d-solo-STEquipmentScript 判词·N] 0x088c1a0d：帧/槽号平移 + STL 模板内联差，
    // p167 CLEAR（40/40）⇒ 语义闭合，byte 层 N82/N89 封顶。
    // [SweepSTEquipScript 2026-09-09 重判] 本轮两处修正：①`[/module]` 由 break 改
    // `continue`（ORIG 0x088c1de1 `jne 088c1a36` 回循环头）并补末尾 `else break`
    // （ORIG 0x088c1dfc `je 088c1e11` 是循环唯一出口）——旧译文遇到 `[/module]`
    // 即提前返回，跳过其后所有标签（真语义缺口）；②[option] 子循环改
    // `while(true){ if(!ScanStr(&value)) break; ... }` 复现 ORIG 0x088c1c2a
    // `xor $1; test; je` 取反物化（P292）。gap 4 → 0（314/314）。
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
            // ORIG 0x088c1c2a 形 = `call; xor $0x1,%eax; test %al,%al; je <循环体>`
            // （P292 bool 取反物化）⇒ 源形是 `if (!ScanStr(&value)) break;`。
            while (true)
            {
                if (!ScanStr(&value))
                    break;
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
        else
        {
            // [2026-09-10] ORIG 0x088c1de1 `test %al,%al; jne <循环头>`：匹配
            // "[/module]" → 回循环头（continue 语义），不匹配直落 [file name] 检查。
            // 两条 continue 路径都写成**落到链尾**（无显式 continue）⇒ 复现
            // ORIG 的「匹配→跳循环头 / 不匹配→直落」正条件布局（显式 continue
            // 会被 4.4 编成 `je <跳过>; nop; jmp <循环头>`）。
            if (tag == "[/module]")
            {
                // 空体：匹配后落到链尾（= continue）
            }
            else if (tag == "[file name]")
            {
                ScanStr(&effect.m_str14);
            }
            else
            {
                // ORIG 0x088c1dfc `je 088c1e11`：未知标签 → 退出循环（0x404 析构 +
                // `mov $0x1,%ebx` 返回 true）。
                break;
            }
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
    // [SweepSTEquip 2026-09-09] 0x089c1cb9：ORIG 774 → 我方 772（gap 44→2）。
    // 本轮四处修复：① `find(key->c_str())` 隐式构造临时 string（0x089c1cec..d02）；
    // ② `m_level5c.rbegin()->push_back(*ani)` 语义纠错（ORIG ptEv + 内层 push_back，
    //   旧译文 deEv + 外层 push_back 把 ani 追加错层）；③ 迭代器先默认构造再
    //   `it = begin()`（P50/P314 复活，0x089c25c7 C1Ev）；④ 两循环复用同一 it 槽
    //   （P160）。p167 147=147 CLEAR（仅余 C1↔C2/D1↔D2 同址别名命名差）；残余
    //   2 条为帧差（0x25c vs ORIG 0x26c，N126/N115）。
    // [SweepSTEquipScript 2026-09-09 重判] 两处修正：①补 `if (ani->m_field0 != 0)`
    //   守卫（ORIG 0x089c1d7c..d83 `mov 0xc(%ebp),%eax; mov (%eax),%eax; test; je`，
    //   仅层号非 0 才提交；旧译文无条件 push_back ⇒ 真语义缺口，Ghidra C 同形）；
    //   ②T_level/else 删具名 `info` 引用、直用 addAniInfo 返回值（ORIG 0x089c1e09
    //   `lea 0x18(%eax),%edx` / 0x089c1e2a `mov %eax,(%esp)`，P0）。
    //   ③`ani->m_field0` 赋值由三目改 if/else（ORIG 0x089c1e49 `cmpl $0; je 0x1a0`
    //   是 layerId 分支在前、def 后置，P47 块排布镜像）。读数 ORIG 774 / 我方 774
    //   （gap 2→0；TU near 2→3 / diff 15→14，DIFF→NEAR）。
    // ORIG 用 `lea -0x24c(%ebp),%ebx; mov $0x0,%eax;
    // mov $0x80,%edx; rep stos %eax,%es:(%edi)`（0x80 dwords = 512B）⇒ 原码是
    // `char buf[0x200] = {0};`（显式零初始化触发 4.4 rep stos）；旧 `memset` 生 call。
    char buf[0x200] = {0};
    // ORIG 0x089c1cec..0x089c1d02：`key->c_str()` → `string::C2(const char*, allocator)`
    // 构造**临时 string** 再传 find（隐式转换），非直接传 *key 引用（p167 MISSING
    // string::C2PKcRKSaIcE / ~string / allocator::C2Ev 各 1）。
    const STEquipmentAniScript& def = table->find(key->c_str())->second;

    // ORIG 0x089c1d7c..0x089c1d83：`mov 0xc(%ebp),%eax; mov (%eax),%eax; test %eax,%eax;
    // je 0x185` ⇒ 三分支提交整体被 `if (ani->m_field0 != 0)` 守卫（层号 0 = 无效，
    // 不提交；Ghidra C 同形）。旧译文无条件提交 ⇒ 真语义缺口。
    if (ani->m_field0 != 0)
    {
    // [b28d-solo-STEquipmentScript] p167：ORIG 三个分支各有独立 addAniInfo 调用点
    // （MISSING addAniInfo×2），不是分支前统一取一次。
    if (aniType == equipmentAniType::T_job)
    {
        EquipmentAniInfoScript& info = script->addAniInfo(job);
        if (!info.m_level5c.empty())
        {
            // ORIG 0x089c1dcd..0x089c1dea：`rbegin()` → reverse_iterator::operator->
            // （ptEv）取**末个子 vector 的地址**，再对它 `push_back(ani)`；旧译文
            // 写成外层 `m_level5c.push_back(*rbegin())`（deEv）语义错（复制末子表
            // 而非把 ani 追加进末子表）——p167 MISSING ptEv / vector<AniScript>::
            // push_back，EXTRA deEv / vector<vector<>>::push_back。
            // [2026-09-10] ORIG 0x089c1dcd..0x089c1dea 把 rbegin() 的 sret 临时放在
            // 深槽 -0x4c（与 it -0x48 / find sret -0x44 同区）；匿名临时被 4.4 放浅槽
            // -0x3c 并把其余临时整体挤深 4（NEAR 的根因）⇒ 用**具名** reverse_iterator
            // 让 4.4 按具名对象分组分配。
            std::vector<std::vector<STEquipmentAniScript> >::reverse_iterator rit =
                info.m_level5c.rbegin();
            rit->push_back(*ani);
        }
    }
    else if (aniType == equipmentAniType::T_level)
    {
        // ORIG 0x089c1e09：`lea 0x18(%eax),%edx` 直接用 addAniInfo 返回值 ⇒ 无具名
        // 引用局部（P0；具名版多 store+load 两条）。
        script->addAniInfo(job).m_ani18.push_back(*ani);
    }
    else
    {
        // ORIG 0x089c1e2a：同上（`mov %eax,(%esp)` 直传返回值）。
        script->addAniInfo(job).m_ani0.push_back(*ani);
    }
    }

    ani->clear();
    // ORIG 0x089c1e49 `cmpl $0x0,0x14(%ebp); je 0x1a0`（layerId 分支在前、
    // def 分支后置）⇒ 源形是 if/else 而非三目（P47 块排布镜像）。
    if (layerId != 0)
        ani->m_field0 = layerId;
    else
        ani->m_field0 = def.m_field0;
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
    SETANI_SLOT(m_arr10[0]);   // +0x10
    SETANI_SLOT(m_arr10[1]);   // +0x14
    SETANI_SLOT(m_str18);
    SETANI_SLOT(m_str1c);
    SETANI_SLOT(m_str20);
    SETANI_SLOT(m_str24);
    SETANI_SLOT(m_str28);
    SETANI_SLOT(m_arr2c[0]);   // +0x2c
    SETANI_SLOT(m_arr3c[0]);   // +0x3c
    SETANI_SLOT(m_arr2c[1]);   // +0x30
    SETANI_SLOT(m_arr3c[1]);   // +0x40
    SETANI_SLOT(m_arr2c[2]);   // +0x34
    SETANI_SLOT(m_arr3c[2]);   // +0x44
    SETANI_SLOT(m_arr2c[3]);   // +0x38
    SETANI_SLOT(m_arr3c[3]);   // +0x48
    SETANI_SLOT(m_str4c);
    SETANI_SLOT(m_str50);
    SETANI_SLOT(m_str54);
    SETANI_SLOT(m_str58);
    SETANI_SLOT(m_str5c);
#undef SETANI_SLOT

    // ORIG 0x089c25c7：迭代器先默认构造（`__normal_iterator::C1Ev`，P50 复活）
    // 再 `it = begin()`（0x089c25cd C1Ev → 0x089c25e5 beginEv → 0x089c25f3 赋值）；
    // 两个循环**复用同一 it 槽 -0x48**（P160 复用槽律）。
    std::vector<std::string>::const_iterator it;
    it = def.m_vec64.begin();
    for (; it != def.m_vec64.end(); ++it)
    {
        // ORIG 0x89c2601..0x89c2634：格式串 = it 元素，参数 = path
        snprintf(buf, sizeof(buf), it->c_str(), path->c_str());
        ani->m_vec64.push_back(std::string(buf));
    }
    it = def.m_vec70.begin();
    for (; it != def.m_vec70.end(); ++it)
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
    // [SweepSTEquip 2026-09-09 P333 状态机还原] ORIG 0x089bf6ca 是「首次建表、
    // 之后查表」的 switch 状态机（与 GetItemCategory/GetAuraAvatarOption 同族）：
    //   ① 循环条件 0x089bfbd2 为 `scanning && ScanStr(&token)`（scanning 是
    //      0x089bf704 `movb $0x1,-0x21` 的永不修改死变量，P171 照抄）；
    //   ② 循环体首 `typeSel(-0x20)=0`（0x089bf726），retry 锚 0x089bf72d；
    //      已初始化走 const_iterator find（`typeSel = it!=end ? it->second : -1`），
    //      未初始化 enter 后若已被建表则 leave+回跳（0x089bf8??），否则 typeSel=0；
    //   ③ switch(typeSel)：case 0 与 case 0x303a 同体，每 case 体头
    //      `movzbl __inited; xor $1; test; je INSERT`；已初始化则 `typeSel=1..5`
    //      后 break 到 0x089bfb38，否则 insert 并 fallthrough 下一 case；
    //   ④ default：`if (__inited) return false`；
    //   ⑤ 尾块 `if (!__inited) { leave; __inited = true; goto retry; }`；
    //   ⑥ 业务块 0x089bfb5f：`int level=0; if(!ScanInt(&level)) return false;
    //      if (typeSel != 0) { entry...; push_back; }` 后回循环条件；
    //      收尾 `return !item->m_expertJob.m_usable.empty();`。
    // 旧译文把 find/switch 拆到两个分支（EXTRA 非 const find/end/c_str ×1），
    // 且建表三连 insert 直线形，缺整棵 switch 与回跳 ⇒ gap 18。
    // [SweepSTEquipScript 2026-09-09] 五处 case 体极性反转（ORIG 每 case
    // `movzbl __inited; xor $1; test; je INSERT` ⇒ 源形 `if(!__inited){insert}else{...}`）：
    // gap 10 → 0（330/330）。残余 DIFF 为槽号/寄存器差（N82）。
    if (item == 0)
        return false;
    item->m_expertJob.m_usable.clear();

    std::string token;
    bool scanning = true;
    while (scanning)
    {
        // ORIG 0x089bf70d 是循环体开头（ScanStr 在此），0x089bfbd2 的条件只测
        // scanning 后跳回体头 ⇒ 源形是 `while (scanning) { if(!ScanStr) break; }`，
        // 不是 `while (scanning && ScanStr(&token))`（后者把 ScanStr 挪进条件段）。
        if (!ScanStr(&token))
            break;
        int typeSel = 0;
        // [P164/P333 修正 2026-09-10] 同 GetItemCategory：static 三件套移入重查
        // 循环体内（ORIG guard 段 0x089bf72d/0x089bf7a7 在 retry 锚 +0x063），
        // `goto retry` 改内层 `for (;;)`（消 4.4 entry jmp+nop）；switch 判别值
        // 用独立无初始化 `int v`（ORIG -0x1c，业务值 typeSel 在 -0x20）。
        for (;;)
        {
        static std::map<const char*, int, less_TCHAR> __lookup;  // 0x94fde48
        static SpinLock __init_lock;                             // 0x94fde60
        static bool __inited = false;                            // 0x94fde64
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator found =
                __lookup.find(token.c_str());
            v = (found != __lookup.end()) ? found->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                              // 自旋重试（P333：重放 guard）
            }
            v = 0;                                     // 建表哨兵
        }
        switch (v)
        {
        case 0:                                        // 与首 key 同体（P163）
        case 0x303a:                                   // enchanter
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("enchanter", 0x303a));
            }
            else
            {
                typeSel = 1;
                break;
            }
            /* FALLTHRU */
        case 0x303d:                                   // alchemist
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("alchemist", 0x303d));
            }
            else
            {
                typeSel = 2;
                break;
            }
            /* FALLTHRU */
        case 0x3040:                                   // disjointer
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("disjointer", 0x3040));
            }
            else
            {
                typeSel = 3;
                break;
            }
            /* FALLTHRU */
        case 0x3043:                                   // doll_controller
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("doll_controller", 0x3043));
            }
            else
            {
                typeSel = 4;
                break;
            }
            /* FALLTHRU */
        case 0x3046:                                   // all
            if (!__inited)
            {
                __lookup.insert(std::pair<char const* const, int>("all", 0x3046));
            }
            else
            {
                typeSel = 5;
                break;
            }
            /* FALLTHRU */
        default:
            if (__inited)
                return false;                          // 未命中 → 导入失败
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
        }
        else
        {
            break;                                     // 已建表 → 退出内层重查循环
        }
        continue;                                      // 建表后重查本 token
        }

        int level = 0;
        if (!ScanInt(&level))
            return false;
        if (typeSel != 0)
        {
            // ORIG 0x089bfb8e/0x089bfb98 先 `movl $0x0,-0xac/-0xa8` 两条清零，
            // 再写 m_field0/m_field4 ⇒ value-init（POD 聚合零初始化）形。
            STItemScript::SEXPERTJOB::SUSABLE entry = STItemScript::SEXPERTJOB::SUSABLE();
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
    // [b28d-solo-STEquipmentScript 判词·N85] ORIG 0x089bfc2f 27 条 vs 我们 29 条：
    // 已用 P225 while 形复现 `flds; flds; fucompp; fnstsw; sahf; sete; test; je`
    // （消掉 4.4 `if` 形的 fxch + jne/jp 直跳，见下方注释与探针 probe6 d1/d2）；
    // 残余仅 `test $0x5,%ah` vs `sahf`（1 条）+ `setnp/and`（2 条）= 4.1 与 4.4
    // 的 x87 比较 lowering 固有差，源形不可择（N85 封顶）。
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field18 = ScanFloat(&ok);
    // [b28d-solo-STEquipmentScript] P225 while 形复现 4.1 的 `==` 物化
    // （ORIG 0x089bfc5a `fucompp; fnstsw; test $0x5,%ah; sete; test; je`）：
    // 写 `if (== ) return false;` 4.4 恒产 `fxch; jne; jp` 直跳（探针
    // /tmp/b28d_s219/probe6.cpp d1 vs d2）；残余 `test $5,%ah` vs `sahf` + setnp/and
    // 属 N85 跨版本 x87 lowering。
    // [2026-09-10] ORIG 0x089bfc63..0x089bfc7c：`fucompp; fnstsw %ax;
    // test $0x5,%ah; sete %al; test %al,%al; je` ⇒ 4.1 把 `field <= -100.0f`
    // 编成 st0(const) >= st1(field) 的 `C0|C2==0` 检查。写**交换操作数的 >=**
    // （`-100.0f >= field`）令 4.4 走同一比较方向（写 `field <= -100.0f`
    // 恒产 sahf/setnp/and 的 NaN 保护形，多 2 条）。
    if (-100.0f >= item->m_expertJob.m_field18)
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
    if (!ok)
        return ok;   // ORIG：xor$1 测试 !ok，!ok 分支 movzbl 返回
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
    if (!ok)              // ORIG：xor$1 测试 !ok，!ok 分支 movzbl 返 ok
        return ok;
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
