// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CtorProducers_1/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// AvatarRoulette/Server 权威类经 DataManagerInitDepends.h（含 LevelUpSkill 前置依赖）
#include "CDataManager.h"
#include "CCharacList.h"
#include "CWorldMap.h"
#include "WorldMapScript.h"
#include "NPCScript.h"
#include "STCharacterScript.h"   // STCharacterScript 权威布局（CCharacList::init 真实 C1/D1）
#include "NPCDynamicInfo.h"      // CNPCDynamicInfo 权威布局/ctor（CNPCDynamicInfoManager::add 真实 C1）
#include "CChattingEmoticonList.h"
#include "CGuildServerProxy.h"   // DeliveryMsg 权威声明（迁入定义 NL6）

// 【A 档 GI 顺序对齐】下列两头原本在文件中途 include（87 行起）；AvatarRouletteServer
// ctor 前置到 TU 最前后需在此可见。含 include guard ⇒ 中途处重复 include 无副作用；
// 相对顺序保持 sync_script.h 先于 deps 头（原注释要求）。
#include "sync_script.h"             // STAICharacterCommonScript 占位/STEquipmentPartSetPiece（须先于 deps 头）
#include "DataManagerInitDepends.h"  // TowerOfDespair/CLevelRewardTable 权威类（Ctor_Group1）

#include "CtorProducers_1/AvatarRouletteServer.inc"


// ---- 迁入定义（NL6，outgoing 帧塌缩规避；NL4 unit-at-a-time 机制实证）----

#include "CtorProducers_1/clear.inc"


#include "CtorProducers_1/DeliveryMsg.inc"

#include "EquipmentParameterInfo.h"
#include "STEquipmentScript.h"       // ImportEquipmentScript 声明（5 参）
#include "DNFLexWrapper.h"           // loadRDARScriptFile/ScanType/ScanInt/loadRDARScriptListFile
#include "LogManager.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "RDARScriptStringManager.h" // g_scriptStringManager_/findString
#include "sync_script.h"             // STAICharacterCommonScript 占位/STEquipmentPartSetPiece（须先于 deps 头）
#include "DataManagerInitDepends.h"  // TowerOfDespair/CLevelRewardTable 权威类（Ctor_Group1）
#include "CDungeon.h"                // get_index/get_min_level/get_standard_level/getDungeonKind
#include "GameTypes.h"               // cMyTrace
#include "TimerDispatcher.h"         // TimerNPCMoodChange::registNextTimer（权威声明）

// ---- STScriptFileList 镜像（map 视图；NpcScript.h 的 pad 版仅作不透明传递，
//      需要 map 成员访问的三全局改用本镜像类型声明）----
class STFileListMapMirror
{
public:
    std::map<int, std::string> m_fileMap;                // +0x00
    int FindIndexByFullScan(const char* path) const;     // ORIG T 0x088bae56
};

// ---- ORIG B 全局脚本文件列表（对象模型，map<int,string>@+0；定义归属
//      InitScriptProducers_4.cpp / InitScriptProducers_5.cpp，此处仅引用）----
extern STFileListMapMirror g_stCharacterScriptFileList;   // B 0x94fe940
extern STScriptFileList g_worldMapScriptFileList;         // 0x9501000（NpcScript.h 类型；方法解析 ORIG 符号）
extern STFileListMapMirror g_aiCharacterScriptFileList;    // 0x94f6a20
extern STScriptFileList g_stEquipmentScriptFileList;       // 0x94fac00（ORIG 类型，GetFileName 解析 0x088bad4e）
extern char g_worldMapScriptBaseDirectory[0x104];          // B（InitScripts_5 定义域）

#include "CtorProducers_1/ImportCharacterScript.inc"

// ---- STAICharacterCommonScript 权威布局视图（sizeof 0x1010 —— init 栈帧
//      -0x104c..-0x3c 实证；CAICharacter::set 访问最远字段 +0x100c。
//      sync_script.h 现存 8 字节占位与本视图冲突，故本 TU 不含该头。
//      CAICharacter::set 符号由 sub_CAISet asm 别名提供（见下））----
struct STAICharacterCommonScriptView
{
    int m_field0;                                   // +0x000
    std::string m_str4;                             // +0x004
    int m_level;                                    // +0x008（set 读）
    char m_padC[0xd80 - 0xc];                       // +0x00c..
    int m_d80;                                      // +0xd80（set 读）
    int m_d84;                                      // +0xd84（set 读）
    std::vector<std::pair<int, int> > m_d88;        // +0xd88（set assign）
    char m_padD90[0xda4 - 0xd94];                   // +0xd90..
    int m_da4;                                      // +0xda4（set 读）
    int m_da8;                                      // +0xda8（set 读）
    char m_padDac[0xf2c - 0xdac];                   // +0xdac..
    std::vector<stMonsterIndependentDrop_t> m_f2c;  // +0xf2c（set assign）
    char m_padF38[0xf50 - 0xf38];                   // +0xf3c..
    char m_avatarDrops[0x100c - 0xf50];             // +0xf50（stEnamyDropAvatarInfo_t 区，本 TU 不访问成员）
    unsigned char m_rarityByte;                     // +0x100c（set 读）
    char m_pad100d[3];
    int m_animationJob;                             // +0x1010（ORIG clear 置 0xb）
};  // sizeof 0x1014（ORIG STAICharacterCommonScript 布局视图）


namespace DNFFLib { void Make_Dir(char* dir); }   // T 0x08108ad7

#include "CtorProducers_1/loadRDARScriptListFile.inc"
                            const char* listPath, std::map<int, int>* unused);
   // ORIG T 0x088bbd77 _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_
   //   _PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE；定义 STScriptFileList.cpp。
   // 旧本地声明 bool(void*,...) mangled 不匹配（Pv ≠ P16STScriptFileList），
   // 曾产生未解析引用——本批次统一为 ORIG 签名（ScriptFileAndGlobalsB2）。
   // 返回类型按 bool 声明（mangled 不含返回类型；StrikerScript.cpp 先例；
   // ORIG 体 return 0/1，调用侧 xor$1/test %al 岛证实 1 字节语义）。

// ---- STAICharacterCommonScript 权威视图（sizeof 0x1010；ORIG 同名类型，
//      sync_script.h 的 8 字节占位与本视图冲突故本 TU 不含该头）----
// +0x08 字段声明为枚举：P254 判据（ORIG `mov 0x8(%eax),%eax; mov %eax,%edx`
// 两段装载中转 ⇒ 源字段类型是 enum；int 字段 4.4 恒折单段）。
enum AICharLevelEnum { AI_CHAR_LEVEL_MIN = 0 };

struct STAICharScriptView
{
    int m_field0;                                   // +0x000
    std::string m_str4;                             // +0x004
    AICharLevelEnum m_level;                        // +0x008（set 读；enum 见上）
    char m_padC[0xd80 - 0xc];
    int m_d80;                                      // +0xd80
    int m_d84;                                      // +0xd84
    std::vector<std::pair<int, int> > m_d88;        // +0xd88
    char m_padD90[0xda4 - 0xd94];
    int m_da4;                                      // +0xda4
    int m_da8;                                      // +0xda8
    char m_padDac[0xf2c - 0xdac];
    std::vector<stMonsterIndependentDrop_t> m_f2c;  // +0xf2c
    char m_padF38[0xf50 - 0xf38];
    stEnamyDropAvatarInfo_t m_f50;                  // +0xf50（sizeof 0x3c；P288 成员取址 lea）
    char m_padF8c[0x100c - 0xf8c];                  // +0xf8c..+0x100b
    unsigned char m_rarityByte;                     // +0x100c
    char m_pad100d[3];
};  // sizeof 0x1010

// ============================================================================

// （报告 AvatarRoulette/{AvatarRoulette,clear,swap}.md）

// ============================================================================

// （报告 AvatarRoulette/{AvatarRoulette,clear,swap}.md）
// ============================================================================
// AvatarRouletteServer（报告 AvatarRouletteServer/*.md）
// ============================================================================

#include "CtorProducers_1/dtor_AvatarRouletteServer.inc"


#include "CtorProducers_1/Init.inc"


#include "CtorProducers_1/isAvatarPottery.inc"


#include "CtorProducers_1/InitAddCharacSkill.inc"


#include "CtorProducers_1/Pickup.inc"


// ============================================================================
// CChattingEmoticonList ctor（ORIG 0x0837e418W；声明见 CDataManager.h：
// map@0/mapVec@0x18/mapInt@0x30 三 map 默认构造，编译器生成）
// ============================================================================

// ============================================================================
// NPC 装载小件（NpcScript.h 提供全局声明）
// ============================================================================

#include "CtorProducers_1/initNpcScript.inc"


#include "CtorProducers_1/isExistNPCScript.inc"


// ============================================================================
// CNPCDynamicInfoManager（ctor/dtor 0x083760f2W/0x08376106W；
// getNPCInfo 0x08581948W；add 0x08580668 报告 CNPCDynamicInfoManager/add.md）
// sizeof 0x18（ctor 0x083760f2W 仅 map 构造）。
class CNPCDynamicInfoManager
{
public:
    CNPCDynamicInfoManager();                       // ORIG 0x083760f2W
    ~CNPCDynamicInfoManager();                      // ORIG 0x08376106W
    CNPCDynamicInfo* getNPCInfo(unsigned int idx);  // ORIG W 0x08581948
    bool add(const CNPCScript* script);             // ORIG T 0x08580668

    std::map<unsigned int, CNPCDynamicInfo*> m_map;   // +0x00
};

#include "CtorProducers_1/CNPCDynamicInfoManager.inc"


#include "CtorProducers_1/getNPCInfo.inc"


#include "CtorProducers_1/add.inc"


// ============================================================================
// TowerOfDespair（TowerOfDespair_APC_Mgr ctor 0x086444e6W 仅 map 构造；
// TowerOfDespairMgr 报告 TowerOfDespairMgr/*.md）
// ============================================================================

#include "CtorProducers_1/TowerOfDespair_APC_Mgr.inc"


#include "CtorProducers_1/TowerOfDespairMgr.inc"


#include "CtorProducers_1/dtor_TowerOfDespairMgr.inc"


#include "CtorProducers_1/_Set_LayerByDungeonIndex.inc"


#include "CtorProducers_1/_Set_LayerByDungeonIndex_1.inc"


#include "CtorProducers_1/_Set_APCIndexByLayer.inc"


#include "CtorProducers_1/LoadScript.inc"


// ============================================================================
// CLevelRewardTable（ctor 0x086975feW / SetJobAffection 0x08697766W /
//   GetProperJobIndex 0x08687e78 / GetProperLevelIndex 0x086876c8）
// ============================================================================

#include "CtorProducers_1/CLevelRewardTable.inc"


#include "CtorProducers_1/SetJobAffection.inc"


#include "CtorProducers_1/GetProperJobIndex.inc"


#include "CtorProducers_1/GetProperLevelIndex.inc"


// ============================================================================
// CLevelRewardTableMgr（单例指针 B 0x09473978；ctor 0x08687710 /
// D1 0x0868777c / GetInstance 0x0837505bW / SetChallengeInfo 0x08687828）
// ============================================================================

CLevelRewardTableMgr* CLevelRewardTableMgr::levelRewardTableMgr = 0;

#include "CtorProducers_1/CLevelRewardTableMgr.inc"


#include "CtorProducers_1/dtor_CLevelRewardTableMgr.inc"


#include "CtorProducers_1/GetInstance.inc"


#include "CtorProducers_1/SetChallengeInfo.inc"


// ============================================================================
// WorldMapScript（ctor 0x08369f08W / dtor 0x0837f5aeW / clear；
// importWorldMapScript 0x08ab0388 报告 _lt_global_gt_/importWorldMapScript.md）
// ============================================================================

#include "CtorProducers_1/WorldMapScript.inc"


#include "CtorProducers_1/dtor_WorldMapScript.inc"


#include "CtorProducers_1/clear_1.inc"


#include "CtorProducers_1/importWorldMapScript.inc"


// ============================================================================
// CWorldMap（ctor 0x0837f6eaW / set_world_map 0x0836421a）
// ============================================================================

#include "CtorProducers_1/CWorldMap.inc"


#include "CtorProducers_1/set_world_map.inc"


// ============================================================================
// CWorldMapList（insert_world_map 0x08364694 / load_list 0x08364506；
// find_world_map/destroy/C1/D1 定义于 CDataManagerScripts.cpp）
// ============================================================================

#include "CtorProducers_1/insert_world_map.inc"


#include "CtorProducers_1/load_list.inc"


// ============================================================================
// CCharacList（ctor 0x083493ea / dtor 0x0834945e / destroy 0x083494da /
// init 0x08349514；声明见 CCharacList.h）
// ============================================================================

#include "CtorProducers_1/CCharacList.inc"


#include "CtorProducers_1/dtor_CCharacList.inc"


#include "CtorProducers_1/destroy.inc"


#include "CtorProducers_1/init.inc"


// ============================================================================
// CAICharacter::set（0x0834a066 报告 CAICharacter/set.md）
// 本 TU 局部完整视图（与 MonsterIndependentDrop.h 镜像共享前缀 0x30，
// 补齐 +0x30..+0x74 尾段至 ORIG 尺寸；跨 TU 同前缀保证既有实现不变）。
// ============================================================================

class CAICharacterFull   // ORIG 0x74；前缀 0x30 与 MonsterIndependentDrop.h 镜像一致
{
public:
    // [b28d-Main·检查点21] 删 ctor/dtor：ORIG 无 CAICharacterFull 符号（该名是
    //   本 TU 局部视图；ORIG 的 0x08373a9a/0x08373b32 是 CAICharacter::C1/D1，
    //   已由 CAICharacterList.cpp 发射）。本类仅作指针类型用于 sub_CAISet，
    //   从不实例化 ⇒ 无 ctor/dtor 需求。

    // [b28d-solo-CtorProducers_1] +0x00 声明 unsigned：P211 异型化旋钮——ORIG
    // `mov 0xc(%ebp),%edx; mov 0x8(%ebp),%eax; mov %edx,(%eax)` 是 RHS-first
    // 直存；同型 int 字段在 4.4 恒 LHS-first（`mov 0x8(%ebp),%eax` 先）。
    unsigned int m_idx;                                 // +0x00
    int m_level;                                        // +0x04
    int m_field8;                                       // +0x08
    int m_appearancePoint;                              // +0x0c
    std::vector<std::pair<int, int> > m_dropItems;      // +0x10
    int m_pvpWinLow;                                    // +0x1c
    int m_pvpWinHigh;                                   // +0x20
    std::vector<stMonsterIndependentDrop_t> m_independentDrops;  // +0x24
    char m_avatarDrops[0x6c - 0x30];                    // +0x30（stEnamyDropAvatarInfo_t 区）
    std::string m_name;                                 // +0x6c
    unsigned char m_rarity;                             // +0x70
    char m_pad71[3];                                    // → sizeof 0x74
};

// [b28d-Main·检查点21] CAICharacterFull ctor/dtor 已删（见类内注释）。

#include "CtorProducers_1/sub_CAISet.inc"
    asm("_ZN12CAICharacter3setEiRK25STAICharacterCommonScript");

#include "CtorProducers_1/sub_CAISet_1.inc"


// ============================================================================
// CNPCScriptList ctor/initScript（ctor 0x08376262W；initScript 0x08581314；
// find/_destroy/~ 定义于 CDataManagerScripts.cpp）
// ============================================================================

#include "CtorProducers_1/CNPCScriptList.inc"


#include "CtorProducers_1/initScript.inc"


// ============================================================================
// CChattingEmoticonList ctor / CNPCDynamicInfoManager dtor /
// TowerOfDespair_APC_Mgr dtor（编译器生成等价体）
// ============================================================================

#include "CtorProducers_1/CChattingEmoticonList.inc"

#include "CtorProducers_1/dtor_CNPCDynamicInfoManager.inc"

#include "CtorProducers_1/dtor_TowerOfDespair_APC_Mgr.inc"
