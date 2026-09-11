// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_8/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"        // 目标类型：reseal_table_t/stDeathTower_t/
                                 // stRankSystemInfo/stUnlimitChallengeInfo/
                                 // seriaRoomDeco*/useCharacterJobFatigue/
                                 // stVendingMachine_t/PVPMAP_DATA/AvatarColorInfo
#include "sync_script.h"        // STEquipmentPartSetPiece（DataManagerInitDepends.h 需要）
#include "LogManager.h"          // LogManager::logFormat
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr
#include "CEnvironment.h"        // less_TCHAR/SpinLock（SpinLock.cpp 提供）

#include "InitScriptProducers_8/getCharacterJobToEnumValue.inc"
#include "InitScriptProducers_8/getDrawLayer.inc"
#include "InitScriptProducers_8/importDeathTowerScript.inc"


#include "InitScriptProducers_8/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);
// _Z22loadRDARScriptListFile.. T 0x088bbd77（STScriptFileList.cpp，定义侧已 bool；
// P292：ORIG 调用点 `test %al,%al` 直判 ⇒ 本 TU 旧 int 声明产 test/sete 物化）。

// ---- PvP 全局表：定义于 PvP_Room.cpp 的三个，此处仅 extern 引用 ----
// [b28d-Main 代落·S165] 元素 unsigned char→bool（定义侧 PvP_Room.cpp:65 已改；
// 消 ODR 不一致；sizeof/地址 0x94f70e0 不变，写侧值域 0/1）。
extern bool PVPMAP_DEATHMATCH_MODE[0x14];                     // ORIG B 0x94f70e0
// [b28d-Main 代落·S215] 元素 char→bool（定义侧 PvP_Room.cpp 已改，P248 档②；
// 同符号同宽，读侧已按 bool 读 0/1）。
extern bool PVPMAP_ONLY_NORMAL_EXPOSURE[0x14];       // ORIG B 0x94f7108
extern bool PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14];     // ORIG B 0x94f711c

// ---- PvP map 全局表（ORIG 数据符号；外部链接）----
// D 0x93f4f00 [0x50]：int[20]，仅 [0]=-1 其余 0（.data 字节实证）。
int PVPMAP_BIG_IMAGE[0x14] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// D 0x93f4f60 [0x50]：同上。
int PVPMAP_SMALL_IMAGE[0x14] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// B 0x94f70f4 [0x14]：char[20] 零初始化 BSS。
char PVPMAP_ONLY_TOURNAMENT_EXPOSURE[0x14] = {0};

#include "InitScriptProducers_8/importResealTable.inc"


#include "InitScriptProducers_8/importUltimateDungeonList.inc"


#include "InitScriptProducers_8/importVillageAttackedScript.inc"


#include "InitScriptProducers_8/loadPvpMapBasicDataScript.inc"


// ============================================================================
// importVendingMachineInfoCore —— ORIG 0x0890b2a3
// 单台售货机脚本解析。[item group]…[/item group] 为一栏：
//   [group num]/[material a b] 直写；[output item rate count flag] 与
//   [bonus output …] 分别累计 m_giveCount/m_bonusCount 并 push
//   {itemId, 新累计值, count}；flag==1 时 (itemId,count) 追加 m_lottery。
// 错误码：0x1e0b 打开失败、0x1e27/0x1e2d/0x1e30 组头、0x1e3d/0x1e40/0x1e44
// output、0x1e55/0x1e58/0x1e5c bonus；组间未知标签记日志继续；EOF 返回 0。
// 注意：ORIG clear() 不清 m_lottery（仅 vecGive/vecBonus），忠实保留。
// ============================================================================
// [GiantA-R8 only-NEW 清理] 原匿名命名空间两 const（`_ZN12_GLOBAL__N_1L13kVendBonusTagE` /
// `L14kVendOutputTagE`）ORIG 全库无同名；ORIG 0x890b444/0x890b572 实证直接用 .rodata 字面量
// （0x8e10db6 = "[output]" / 0x8e10dbf = "[bonus output]"）⇒ 删除常量、使用点内联字面量。

#include "InitScriptProducers_8/importVendingMachineInfoCore.inc"


#include "InitScriptProducers_8/importVendingMachineInfo.inc"


#include "InitScriptProducers_8/importTowerInfo.inc"


#include "InitScriptProducers_8/importRankSystemScript.inc"


#include "InitScriptProducers_8/importUnlimitChallengeScript.inc"


#include "InitScriptProducers_8/importSeriaRoomDecoEvent.inc"


#include "InitScriptProducers_8/importUseJobFatigueGiftScript.inc"


#include "InitScriptProducers_8/importSeriaRoomAnimationDecoEvent.inc"


// ============================================================================
// AvatarVariation::importAvatarColorVariation —— ORIG 0x088d444f
// 固定路径 "Etc/AvatarColorVariation.etc"；清 m_colorMap 后 [avatar color map]
// 段每行 “key r g b name”（RGB 字节宽度 mov %al 实证）；name 扫描成功时同时
// insert colorMap/nameMap，失败退出该段；主循环 EOF 返回成功。
// ============================================================================
namespace AvatarVariation
{

#include "InitScriptProducers_8/importAvatarColorVariation.inc"


}  // namespace AvatarVariation

// ============================================================================
// 域内 POD 生命周期定义（ORIG W 符号转写；声明见 CDataManager.h）
// ============================================================================

#include "InitScriptProducers_8/seriaRoomDecoEventSoundInfo.inc"


#include "InitScriptProducers_8/dtor_seriaRoomDecoEventSoundInfo.inc"

#include "InitScriptProducers_8/clear.inc"


#include "InitScriptProducers_8/fatigueMatchGift.inc"


#include "InitScriptProducers_8/seriaRoomDecoAniInfo.inc"


#include "InitScriptProducers_8/dtor_seriaRoomDecoAniInfo.inc"

#include "InitScriptProducers_8/clear_1.inc"


#include "InitScriptProducers_8/stVendingMachineInfo_t.inc"


#include "InitScriptProducers_8/dtor_stVendingMachineInfo_t.inc"

#include "InitScriptProducers_8/clear_2.inc"


#include "InitScriptProducers_8/stVendingMachine_t.inc"

#include "InitScriptProducers_8/dtor_stVendingMachine_t.inc"
