// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_5/<Func>.inc（不参与编译、内部不含 #include）。
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
#include <stdlib.h>    // atof
#include <string.h>    // strncpy / strlen / __builtin_strcat

#include "DNFLexWrapper.h"    // loadRDARScriptFile / ScanType / ScanInt
#include "LogManager.h"      // ScanAbsOrRel 告警日志
#include "CreatureScript.h"  // CreatureScript::ExpTable_E 静态成员
#include "sync_script.h"     // STEquipmentPartSetPiece（DataManagerInitDepends.h 依赖序，同 CDataManager.cpp）
#include "DataManagerInitDepends.h"  // 本批 12 函数唯一声明点
                                     // （须在 CDataManager.h 后包含；本 TU 不使用
                                     //   其余重类型声明，仅借签名一致性）

#include "InitScriptProducers_5/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);

// ---- ORIG 全局（定义于各自权威 TU；本 TU extern 引用）----
extern char g_expertJobScriptBaseDirectory[0x100];   // B 0x094f7660  CExpertJobList.cpp
extern STScriptFileList g_expertJobScriptFileList;   // B 0x094f7760  CExpertJobList.cpp（本批正为对象模型）
extern char g_itemShopScriptBaseDirectory[0x100];    // B 0x094fe160  CItemShop.cpp（本批由 g_itemShopScriptDir 正名 ORIG nm 名）
extern STScriptFileList g_itemShopScriptFileList;    // B 0x094fe260  CItemShop.cpp
extern char g_PvpMissionScriptBaseDirectory[0x100];  // B 0x09500100  PvP_MissionSystem.cpp
extern STScriptFileList g_PvpMissionScriptFileList;  // B 0x09500200  PvP_MissionSystem.cpp
extern char g_creatureScriptBaseDirectory[0x100];    // B 0x094ff4e0 size 0x100  CreatureScript.cpp
// g_creatureScriptFileList 权威型 STScriptFileList（CreatureScript.h:285；B 0x094ff5e0）
extern char g_ChattingEmoticonScriptBaseDirectory[0x100];    // B 0x094f6bc0  CChattingEmoticonList.cpp
extern STScriptFileList g_ChattingEmoticonScriptFileList;    // B 0x094f6cc0  CChattingEmoticonList.cpp（本批正为对象模型）

// ---- 本 TU 提供定义的 ORIG 全局（nm 真实存在；此前 source 无任何定义，
//      已与 Ctor_Group1 协调归属）----
// [A31-GI · P327] g_eventCharacterScriptBaseDirectory 声明前移为本 TU 首个强外部符号
// ⇒ GI 键名 `_GLOBAL__I_g_eventCharacterScriptBaseDirectory`（ORIG 0x895b874）。
// 纯 POD char[0x100]（零动态初始化）⇒ 移动定义不改变任何构造序。
char g_eventCharacterScriptBaseDirectory[0x100];     // B 0x094f73c0 (initEventCharacterScript)
extern char g_aiCharacterScriptBaseDirectory[0x100];   // [SplitC batch5] 定义已迁出至 RDARScriptAICharacter.cpp
STScriptFileList g_aiCharacterScriptFileList;        // B 0x094f6a20 (0x18)
STScriptFileList g_eventCharacterScriptFileList;     // B 0x094f74c0 (0x18)
extern char g_pvpSkillTreeScriptBaseDirectory[0x100];   // [SplitC batch5] 定义已迁出至 RDARScriptPvPSkillTree.cpp
STScriptFileList g_pvpSkillTreeScriptFileList;       // B 0x09500340 (0x18)
extern char g_independentDropScriptBaseDirectory[0x100];   // [SplitC batch5] 定义已迁出至 RDARScriptIndependentDrop.cpp
STScriptFileList g_independentDropScriptFileList;   // B 0x094f7940 (0x18)
extern char g_stageMapScriptBaseDirectory[0x100];   // [SplitC batch5] 定义已迁出至 global_RDARScriptStageMap.cpp
STScriptFileList g_stageMapScriptFileList;           // B 0x09494e20 (0x18)
extern char g_worldMapScriptBaseDirectory[0x100];   // [SplitC batch5] 定义已迁出至 RDARScriptWorldMap.cpp
STScriptFileList g_worldMapScriptFileList;           // B 0x09501000 (0x18)

#include "InitScriptProducers_5/ImportEquipmentAniScript.inc"
std::map<std::string, STEquipmentAniScript>* g_mapEquipmentAniScript = 0;  // B 0x094f7bf8

// ORIG 静态成员定义（声明 CreatureScript.h）：B 0x094fefa0，396 字节。
int CreatureScript::ExpTable_[0x63];

#include "InitScriptProducers_5/initExpertJobScript.inc"


#include "InitScriptProducers_5/initAICharacterScript.inc"


#include "InitScriptProducers_5/initEventCharacterScript.inc"


#include "InitScriptProducers_5/initItemShopScript.inc"


#include "InitScriptProducers_5/initPvpMissionSystem.inc"


#include "InitScriptProducers_5/initPvPSkillTreeScript.inc"


#include "InitScriptProducers_5/initChattingEmoticonScriptList.inc"


#include "InitScriptProducers_5/initIndependentDropScript.inc"


#include "InitScriptProducers_5/initStageMapScript.inc"


#include "InitScriptProducers_5/initCreatureScript.inc"


#include "InitScriptProducers_5/InitMapEquipmentAniScript.inc"


#include "InitScriptProducers_5/loadWorldMapCoordPattern.inc"
#include "InitScriptProducers_5/ScanAbsOrRel.inc"
                                       // _Z12ScanAbsOrRelRbRSsS_b 0x088bcbef

#include "WorldMapScript.h"

// 静态表定义（权威类型见 WorldMapScript.h；ORIG B 0x09501020 / 0x095014e0）。
STWorldmapPattern WorldMapScript::aWorldMapPatternTable_[3][10];
STWorldmapPattern2ND WorldMapScript::aWorldMapPatternTable2ND_[20];

#include "InitScriptProducers_5/initWorldMapScript.inc"


#include "InitScriptProducers_5/loadWorldMapCoordPattern_1.inc"


// ============================================================================
// ScanAbsOrRel —— ORIG 0x088bcbef（0x2c0 字节；lex 扫描族，当前 source 无
// 定义，本 TU 补齐以支撑 loadWorldMapCoordPattern）。
// float ScanAbsOrRel(bool& isAbs, string& str, bool& gotValue, bool consume)
//   isAbs=true; result=0;
//   loop: tok=g_dnfLex.getToken();
//     tok==2 → result=(float)getDecimal(); isAbs=true; got=true;
//     tok==3 → result=(float)getHexDecimal(); isAbs=true; got=true;
//     tok==4 → result=getFloat();            isAbs=true; got=true;
//     tok==5 → str=toTString(getType()); 去掉首 '[' 前缀与尾 ']' 及其后
//              （find("["+1) erase 头部 / find("]") erase 尾部，npos 回绕语义
//              与 asm 一致）；result=atof(str.c_str()); isAbs=false; got=true;
//     其余  → isAbs=true; got=false;
//   尾判：tok∈{0,2,3,4,5} → return result；tok==1 时 consume 则告警日志后
//   继续循环（LogManager::logFormat file="../RDARScript.cpp"(0x8e0851c),
//   func=__FUNCTION__, line=1496, fmt="ERROR! %s - TYPE mismatch. %s:%d,
//   token = '%s'"(0x8e08658)，实参 func/streamName/lineNo/tokenData）。
// [b28d-solo AE 189/189] 三处源形修正（旧 DIFF 判词作废）：
//   ① `pos = str.find("[",0) + 1;` 后再 erase（ORIG 把 find+1 存槽 -0xc 再回读）；
//   ② 尾判改 P223 五枚独立正向 goto + 连续尾标签（标签序 2,3,4,5,0，ORIG
//      0x088bce8f..0x088bce9c 五岛 nop;jmp；`||` 链在 4.4 被折成单落点）；
//   ③ 告警实参用**全表达式临时链**（无具名 string 局部）：ORIG -0x18/-0x1c 为两个
//      按值返回临时，右起求值 lastData→%esi / lineNo→%ebx / streamName→%eax
//      （探针 /tmp/probe55h.cpp p5 连 EH 块序逐条同形；具名局部因 N93 槽向反）。
// ============================================================================
const char* toTString(const char* pszSrc);   // SyncScriptDeps.h 权威声明（避免拖入其依赖）

#include "InitScriptProducers_5/ScanAbsOrRel_1.inc"

