// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"             // 脚本类型权威定义（须先于 deps 头）
#include "sync_script.h"              // STEquipmentPartSetPiece 完整类型
#include "STEquipmentScript.h"        // STEquipmentScript/m_paramMap678/ImportEquipmentScript
#include "EquipmentParameterInfo.h"   // EquipmentParameterInfo copy/operator=
#include "DataManagerInitDepends.h"   // 本批声明唯一汇总点
#include "DNFLexWrapper.h"            // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"               // LogManager::logFormat

// PVP_NORMAL_BATTLE_TIME / PVP_DM_BATTLE_TIME —— ORIG D 0x093f4ee0 / 0x093f4ee8
// （各 [2] int）。初值取自 ORIG .data 逐字节实读：
//   0x093f4ee0: 20 bf 02 00 20 bf 02 00  c0 27 09 00 c0 27 09 00
//   ⇒ PVP_NORMAL_BATTLE_TIME = {180000, 180000}（0x2bf20，3 分钟毫秒）
//      PVP_DM_BATTLE_TIME     = {600000, 600000}（0x927c0，10 分钟毫秒）
// 非零静态初值同时决定段归属为 .data（D，与 ORIG 一致；零初始化会落 .bss）。
// ORIG 中与本 TU 的 g_equipmentPartSetMap_ / characterEquipmentTypeTags 同源文件且为
// 文件内首个具名对象，决定 `_GLOBAL__I_PVP_NORMAL_BATTLE_TIME` 静态初始化键名
// （c6root gcc4.4 探针验证）；因此定义必须置于本文件首个具名对象位置，
// PvP_Room.cpp / CDataManager.cpp 仅 extern 引用（脚本加载时覆盖 [0]/[1]）。
// ============================================================================
int PVP_NORMAL_BATTLE_TIME[2] = {180000, 180000};
int PVP_DM_BATTLE_TIME[2] = {600000, 600000};

// ============================================================================
// characterEquipmentTypeTags —— ORIG D 0x093f4fc0（26 项 const char*）
// 内容逐项提取自 ORIG .rodata 指针表（0x8e15e81..0x8e15fad）。
// ============================================================================
// characterEquipmentTypeTags —— ORIG D 0x093f4fc0（26 项 const char*）
// 内容逐项提取自 ORIG .rodata 指针表（0x8e15e81..0x8e15fad）。
// ============================================================================
const char* characterEquipmentTypeTags[26] = {
    "[hat avatar]",    "[hair avatar]",   "[face avatar]",    "[coat avatar]",
    "[pants avatar]",  "[shoes avatar]",  "[breast avatar]",  "[waist avatar]",
    "[skin avatar]",   "[aurora avatar]", "[weapon]",         "[title name]",
    "[coat]",          "[shoulder]",      "[pants]",          "[shoes]",
    "[waist]",         "[amulet]",        "[wrist]",          "[ring]",
    "[support]",       "[magic stone]",   "[creature]",
    "[artifact red]",  "[artifact blue]", "[artifact green]",
};

// ORIG B 0x094f7130（BSS，sizeof(std::map)=0x18）；init 尾段遍历回灌 DB。
std::map<int, STEquipmentPartSetData> g_equipmentPartSetMap_;

#include "InitScriptProducers_2/STEquipmentPartSetData.inc"


#include "InitScriptProducers_2/dtor_STEquipmentPartSetData.inc"


#include "InitScriptProducers_2/makeFairPVPEquipmentPackageKey.inc"
{
    // ORIG 0x088945ad：单 int 槽 -0x4，`mov %ax,-0x4`（job 低16）→
    // `mov %ax,-0x2`（idx 高16）→ `mov -0x4,%eax` 整体回读。
    // 两次 16-bit store 到同一 32-bit 槽 ⇒ P180 union 视图逐半字写（移位/or 形
    // 被 4.4 生 movzwl+shl+or，int 局部赋 ushort 会走整 dword store 均不可）。
    union { int u; unsigned short v[2]; } key;
    key.v[0] = (unsigned short)job;
    key.v[1] = (unsigned short)idx;
    return key.u;
}

#include "InitScriptProducers_2/getSameStringIndex.inc"


// deleteTagBracketString（ORIG T 0x08917db3）定义归 InitScriptProducers_1.cpp
// （InitScripts_1 认领），本 TU 经 DataManagerInitDepends.h 声明引用。

#include "InitScriptProducers_2/LoadRewardDataByGrowType.inc"


#include "InitScriptProducers_2/ImportReturnUserRewardScript.inc"


#include "InitScriptProducers_2/ImportBingoScript.inc"


#include "InitScriptProducers_2/ImportItemMakingSkill.inc"


#include "InitScriptProducers_2/importGrowthPower.inc"


#include "InitScriptProducers_2/ImportWarAreaCounter.inc"


#include "InitScriptProducers_2/importPairPvPStatScript.inc"


#include "InitScriptProducers_2/ImportSecurityReward.inc"


#include "InitScriptProducers_2/importEquipmentPartSetInfo.inc"

