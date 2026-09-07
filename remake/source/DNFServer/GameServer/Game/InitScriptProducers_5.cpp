// ============================================================================
// df_game_r 还原 —— CDataManager::init() 脚本装载自由函数（批次 5）。
// 承载 DataManagerInitDepends.h 声明的以下符号（全部 ORIG nm 真实 T 符号，
// objdump 逐指令转写；调用点证据 docs/class_func_reports/CDataManager/init.md）：
//   InitMapEquipmentAniScript       _Z25InitMapEquipmentAniScriptv         0x089c0586  init.md:97
//   initExpertJobScript             _Z19initExpertJobScriptPcS_            0x0896d830  init.md:524
//   initIndependentDropScript       _Z25initIndependentDropScriptPcS_      0x0897c01c  init.md:875
//   initEventCharacterScript        _Z24initEventCharacterScriptPcS_       0x0895ae5c  init.md:1596
//   initAICharacterScript           _Z21initAICharacterScriptPKcS0_        0x088c98d4  init.md:1838
//   initItemShopScript              _Z18initItemShopScriptPcS_             0x089dc4f8  init.md:2178
//   initPvpMissionSystem            _Z20initPvpMissionSystemPcS_           0x08a5aad5  init.md:2219
//   initWorldMapScript              _Z18initWorldMapScriptPcS_             0x08ab000c  init.md:2555
//   initStageMapScript              _Z18initStageMapScriptPcS_             0x088a9e6c  init.md:2565
//   initChattingEmoticonScriptList  _Z30initChattingEmoticonScriptListPcS_ 0x088e30f8  init.md:2740
//   initCreatureScript              _Z18initCreatureScriptPKcS0_S0_        0x08a35d37  init.md:4579
//   initPvPSkillTreeScript          _Z22initPvPSkillTreeScriptPcS_         0x08a5cd6c  init.md:4968
//
// initWorldMapScript 的被调依赖（当前源码缺失，本 TU 一并提供，ORIG nm 真实）：
//   loadWorldMapCoordPattern        _Z24loadWorldMapCoordPatternv          0x08ab0234
//     （报告 docs/class_func_reports/_lt_global_gt_/loadWorldMapCoordPattern.md）
//   ScanAbsOrRel                    _Z12ScanAbsOrRelRbRSsS_b               0x088bcbef
//   WorldMapScript::aWorldMapPatternTable_E    B 0x09501020 (0x4b0)
//   WorldMapScript::aWorldMapPatternTable2ND_E B 0x095014e0
//   g_mapEquipmentAniScript         B 0x094f7bf8（InitMapEquipmentAniScript 域）
//
// 统一模式（各函数 objdump 一致）：
//   strncpy(g_*BaseDirectory, dir, N) → 末字符既非 '\\' 也非 '/' 时
//   memcpy(dst+strlen, "\\", 2)；随后 loadRDARScriptListFile(g_*FileList,
//   baseDir, list, NULL)，返回值 !=0 即 true。
//   注意：strncpy 长度按函数区分（多数 0x100；pvpMission/pvpSkillTree/
//   ChatEmoticon/IndependentDrop 为 0xff），逐函数忠实保留。
// ---------------------------------------------------------------------------
// [b22判词·N族/索引伪差集 P93·P154 同族] __tcf_0（<global>）：本 TU 文件作用域带
// 非平凡析构全局（WorldMapScript::aWorldMapPatternTable_[3][10] 图案表、各
// g_*ScriptFileList STScriptFileList 全局）的 GCC 合成 atexit cleanup，无独立可写
// 源码函数体，指令完全由本 TU 全局对象集合与构造序决定。ORIG 二进制内 `__tcf_0`
// 同名 local 数百枚（每含全局对象 TU 一枚），compare 工具按名取到 0x0828f940
// （WongWork::CDeathTowerRanking 域另一 TU，与本 TU 全局无关）→ 逐条 DIFF 系索引
// 伪差集（§6.3 撞名族），非实现缺口。本 .o __tcf_0 已按本 TU 全局集合正确合成析构
// 循环（0x4b0/0x28 对位图案表元素尺寸）。语义等价，不可源码复现，保留判词。
// ============================================================================

#include <map>
#include <string>
#include <stdlib.h>    // atof
#include <string.h>    // strncpy / strlen / memcpy

#include "DNFLexWrapper.h"    // loadRDARScriptFile / ScanType / ScanInt
#include "LogManager.h"      // ScanAbsOrRel 告警日志
#include "CreatureScript.h"  // CreatureScript::ExpTable_E 静态成员
#include "sync_script.h"     // STEquipmentPartSetPiece（DataManagerInitDepends.h 依赖序，同 CDataManager.cpp）
#include "DataManagerInitDepends.h"  // 本批 12 函数唯一声明点
                                     // （须在 CDataManager.h 后包含；本 TU 不使用
                                     //   其余重类型声明，仅借签名一致性）

// ORIG 0x088bbd77 _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIii..
// 定义 STScriptFileList.cpp（int：恒 0/1；第 4 参未使用）。bool 视图声明：
// P22（ORIG init* 尾 xor $1,%eax;test %al 为 bool 取反形；返回不入 mangled）。
bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);

// ---- ORIG 全局（定义于各自权威 TU；本 TU extern 引用）----
extern char g_expertJobScriptBaseDirectory[0x100];   // B 0x094f7660  CExpertJobList.cpp
extern STScriptFileList g_expertJobScriptFileList;   // B 0x094f7760  CExpertJobList.cpp（本批正为对象模型）
extern char g_itemShopScriptBaseDirectory[0x100];    // B 0x094fe160  CItemShop.cpp（本批由 g_itemShopScriptDir 正名 ORIG nm 名）
extern STScriptFileList g_itemShopScriptFileList;    // B 0x094fe260  CItemShop.cpp
extern char g_PvpMissionScriptBaseDirectory[0x100];  // B 0x09500100  PvP_MissionSystem.cpp
extern STScriptFileList g_PvpMissionScriptFileList;  // B 0x09500200  PvP_MissionSystem.cpp
extern char g_creatureScriptBaseDirectory[0x104];    // B 0x094ff4e0  CreatureScript.cpp
// CreatureScript.cpp 将列表建模为其 +0x00 的 map 视图（见 CreatureScript.h 注释）：
extern std::map<int, std::string> g_creatureScriptFileList;  // B 0x094ff5e0
extern char g_ChattingEmoticonScriptBaseDirectory[0x100];    // B 0x094f6bc0  CChattingEmoticonList.cpp
extern STScriptFileList g_ChattingEmoticonScriptFileList;    // B 0x094f6cc0  CChattingEmoticonList.cpp（本批正为对象模型）

// ---- 本 TU 提供定义的 ORIG 全局（nm 真实存在；此前 source 无任何定义，
//      已与 Ctor_Group1 协调归属）----
char g_aiCharacterScriptBaseDirectory[0x100];        // B 0x094f6920 (initAICharacterScript)
STScriptFileList g_aiCharacterScriptFileList;        // B 0x094f6a20 (0x18)
char g_eventCharacterScriptBaseDirectory[0x100];     // B 0x094f73c0 (initEventCharacterScript)
STScriptFileList g_eventCharacterScriptFileList;     // B 0x094f74c0 (0x18)
char g_pvpSkillTreeScriptBaseDirectory[0x100];       // B 0x09500240 (initPvPSkillTreeScript)
STScriptFileList g_pvpSkillTreeScriptFileList;       // B 0x09500340 (0x18)
char g_independentDropScriptBaseDirectory[0x100];    // B 0x094f7840 (initIndependentDropScript)
STScriptFileList g_independentDropScriptFileList;   // B 0x094f7940 (0x18)
char g_stageMapScriptBaseDirectory[0x100];           // D 0x093f4900 (initStageMapScript)
STScriptFileList g_stageMapScriptFileList;           // B 0x09494e20 (0x18)
char g_worldMapScriptBaseDirectory[0x100];           // B 0x09500f00 (initWorldMapScript/loadWorldMapCoordPattern)
STScriptFileList g_worldMapScriptFileList;           // B 0x09501000 (0x18)

// ---- 装备动画脚本域 ----
// STEquipmentAniScript 权威布局见 docs/class_func_reports/STEquipmentAniScript/；
// ImportEquipmentAniScript 完整解析器（ORIG T 0x89c05f8，0x16c1 字节）报告在
// docs/class_func_reports/_lt_global_gt_/ImportEquipmentAniScript.md，
// 定义 ScriptDepsA.cpp（本批已还原真实语义）。ORIG 尾 `mov %ebx,%eax; ret`
// 返回 0/1（P159 判据：非 void；mangled 不含返回类型，调用点忽略返回值）。
int ImportEquipmentAniScript(std::map<std::string, STEquipmentAniScript>* table); // _Z24..
std::map<std::string, STEquipmentAniScript>* g_mapEquipmentAniScript = 0;  // B 0x094f7bf8

// ORIG 静态成员定义（声明 CreatureScript.h）：B 0x094fefa0，396 字节。
int CreatureScript::ExpTable_[0x63];

// ============================================================================
// initExpertJobScript —— ORIG 0x0896d830（init.md:524 +0x0a44 调用点，
// 失败路径 TRACE "Load expertjob.lst fail" 0x1982 区）。
//   strncpy(g_expertJobScriptBaseDirectory(0x94f7660), dir, 0x100) + 追加 '\\'
//   → loadRDARScriptListFile(&g_expertJobScriptFileList(0x94f7760), baseDir,
//     list, NULL) → 结果非 0 返回 true。
// 消费方：CExpertJobList::init（ORIG 0x896d8ec 直接遍历该对象 m_fileMap）。
// ============================================================================
bool initExpertJobScript(char* dir, char* list)
{
    strncpy(g_expertJobScriptBaseDirectory, dir, 0x100);
    // P49 禁 TU helper（原 CopyScriptBaseDirectory 属发明符号）：内联 strlen×3
    // + memcpy 2B "\\"（字面量 0x8e13d1a）；无空串防御（ORIG len==0 UB，P140）。
    if (g_expertJobScriptBaseDirectory[strlen(g_expertJobScriptBaseDirectory) - 1] != '\\' &&
        g_expertJobScriptBaseDirectory[strlen(g_expertJobScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_expertJobScriptBaseDirectory + strlen(g_expertJobScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_expertJobScriptFileList,
                                g_expertJobScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initAICharacterScript —— ORIG 0x088c98d4（init.md:1838 +0x2483；失败 TRACE
// "- Load AI Character fail" 0x1dd4）。同模式，目录 0x94f6920/列表 0x94f6a20。
// ============================================================================
bool initAICharacterScript(const char* dir, const char* list)
{
    strncpy(g_aiCharacterScriptBaseDirectory, dir, 0x100);
    if (g_aiCharacterScriptBaseDirectory[strlen(g_aiCharacterScriptBaseDirectory) - 1] != '\\' &&
        g_aiCharacterScriptBaseDirectory[strlen(g_aiCharacterScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_aiCharacterScriptBaseDirectory + strlen(g_aiCharacterScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_aiCharacterScriptFileList,
                                g_aiCharacterScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initEventCharacterScript —— ORIG 0x0895ae5c（init.md:1596 +0x1f9b；失败
// TRACE "- initEventCharacterScript Fail" 0x1cd4）。目录 0x94f73c0/列表 0x94f74c0。
// ============================================================================
bool initEventCharacterScript(char* dir, char* list)
{
    strncpy(g_eventCharacterScriptBaseDirectory, dir, 0x100);
    if (g_eventCharacterScriptBaseDirectory[strlen(g_eventCharacterScriptBaseDirectory) - 1] != '\\' &&
        g_eventCharacterScriptBaseDirectory[strlen(g_eventCharacterScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_eventCharacterScriptBaseDirectory + strlen(g_eventCharacterScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_eventCharacterScriptFileList,
                                g_eventCharacterScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initItemShopScript —— ORIG 0x089dc4f8（init.md:2178 +0x2b71；失败 TRACE
// "- Load ItemShop.lst fail" 0x1e94）。目录 0x94fe160/列表 0x94fe260
// （消费方 CItemShop.cpp Load_list / FindIndexByFullScan）。
// ============================================================================
bool initItemShopScript(char* dir, char* list)
{
    strncpy(g_itemShopScriptBaseDirectory, dir, 0x100);
    if (g_itemShopScriptBaseDirectory[strlen(g_itemShopScriptBaseDirectory) - 1] != '\\' &&
        g_itemShopScriptBaseDirectory[strlen(g_itemShopScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_itemShopScriptBaseDirectory + strlen(g_itemShopScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_itemShopScriptFileList,
                                g_itemShopScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initPvpMissionSystem —— ORIG 0x08a5aad5（init.md:2219 +0x2c50；失败 TRACE
// "- Load PVP_Mission.lst fail" 0x1eab）。strncpy 长度 0xff；
// 目录 0x9500100/列表 0x9500200（消费方 PvP_MissionSystem.cpp）。
// ============================================================================
bool initPvpMissionSystem(char* dir, char* list)
{
    strncpy(g_PvpMissionScriptBaseDirectory, dir, 0xff);
    if (g_PvpMissionScriptBaseDirectory[strlen(g_PvpMissionScriptBaseDirectory) - 1] != '\\' &&
        g_PvpMissionScriptBaseDirectory[strlen(g_PvpMissionScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_PvpMissionScriptBaseDirectory + strlen(g_PvpMissionScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_PvpMissionScriptFileList,
                                g_PvpMissionScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initPvPSkillTreeScript —— ORIG 0x08a5cd6c（init.md:4968 +0x6507；失败 TRACE
// "- Load PvPSkillTree.lst fail" 0x21d7）。strncpy 长度 0xff；
// 目录 0x9500240/列表 0x9500340。消费方 PvPSkillTreeParameterScript 域。
// ============================================================================
bool initPvPSkillTreeScript(char* dir, char* list)
{
    strncpy(g_pvpSkillTreeScriptBaseDirectory, dir, 0xff);
    if (g_pvpSkillTreeScriptBaseDirectory[strlen(g_pvpSkillTreeScriptBaseDirectory) - 1] != '\\' &&
        g_pvpSkillTreeScriptBaseDirectory[strlen(g_pvpSkillTreeScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_pvpSkillTreeScriptBaseDirectory + strlen(g_pvpSkillTreeScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_pvpSkillTreeScriptFileList,
                                g_pvpSkillTreeScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initChattingEmoticonScriptList —— ORIG 0x088e30f8（init.md:2740 +0x36be）。
// strncpy 长度 0xff；目录 0x94f6bc0/列表 0x94f6cc0。消费方
// CChattingEmoticonList::Load_list（ORIG 0x80e54d2 直接遍历对象 m_fileMap）。
// ============================================================================
bool initChattingEmoticonScriptList(char* dir, char* list)
{
    strncpy(g_ChattingEmoticonScriptBaseDirectory, dir, 0xff);
    if (g_ChattingEmoticonScriptBaseDirectory[strlen(g_ChattingEmoticonScriptBaseDirectory) - 1] != '\\' &&
        g_ChattingEmoticonScriptBaseDirectory[strlen(g_ChattingEmoticonScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_ChattingEmoticonScriptBaseDirectory + strlen(g_ChattingEmoticonScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_ChattingEmoticonScriptFileList,
                                g_ChattingEmoticonScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initIndependentDropScript —— ORIG 0x0897c01c（init.md:875 +0x1153；失败两步
// 均 TRACE "Load IndependentDrop.lst fail" 0x1b56/0x1b5c）。strncpy 0xff；
// 目录 0x94f7840/列表 0x94f7940。（头文件声明返回 int；asm 仅 eax=0/1。）
// ============================================================================
int initIndependentDropScript(char* dir, char* list)
{
    strncpy(g_independentDropScriptBaseDirectory, dir, 0xff);
    if (g_independentDropScriptBaseDirectory[strlen(g_independentDropScriptBaseDirectory) - 1] != '\\' &&
        g_independentDropScriptBaseDirectory[strlen(g_independentDropScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_independentDropScriptBaseDirectory + strlen(g_independentDropScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_independentDropScriptFileList,
                                g_independentDropScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initStageMapScript —— ORIG 0x088a9e6c（init.md:2565 +0x3345，紧随 worldMap
// 之后、返回值被丢弃）。目录 0x93f4900/列表 0x9494e20。消费方 StageMapList 域。
// ============================================================================
bool initStageMapScript(char* dir, char* list)
{
    strncpy(g_stageMapScriptBaseDirectory, dir, 0x100);
    if (g_stageMapScriptBaseDirectory[strlen(g_stageMapScriptBaseDirectory) - 1] != '\\' &&
        g_stageMapScriptBaseDirectory[strlen(g_stageMapScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_stageMapScriptBaseDirectory + strlen(g_stageMapScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_stageMapScriptFileList,
                                g_stageMapScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// initCreatureScript —— ORIG 0x08a35d37（init.md:4579 +0x5d8a；失败 TRACE
// "- Import Creature Creature.lst fail" 0x215f）。
// ① 目录拷贝（0x94ff4e0，0x100）；② loadRDARScriptFile(baseDir, expTable)
//   失败即 false（ExpTable.tbl 先行装载）；③ 循环 i=0..0x62 共 99 次
//   ScanInt(NULL) 写入 CreatureScript::ExpTable_E[i]
//   （B _ZN14CreatureScript9ExpTable_E 0x94fefa0，readelf size 396=99*4）；
// ④ loadRDARScriptListFile 列表装载（0x94ff5e0）取非 0。
// g_creatureScriptFileList 在权威头建模为 STScriptFileList 的 +0x00 map
// （CreatureScript.h 注释），此处以同布局视图传入装载器。
// ============================================================================
bool initCreatureScript(const char* dir, const char* list, const char* expTable)
{
    strncpy(g_creatureScriptBaseDirectory, dir, 0x100);
    if (g_creatureScriptBaseDirectory[strlen(g_creatureScriptBaseDirectory) - 1] != '\\' &&
        g_creatureScriptBaseDirectory[strlen(g_creatureScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_creatureScriptBaseDirectory + strlen(g_creatureScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, expTable))
        return false;
    for (int i = 0; i <= 0x62; ++i)
        CreatureScript::ExpTable_[i] = ScanInt((bool*)0);
    if (!loadRDARScriptListFile(
            reinterpret_cast<STScriptFileList*>(&g_creatureScriptFileList),
            g_creatureScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}

// ============================================================================
// InitMapEquipmentAniScript —— ORIG 0x089c0586（init.md:97 +0x0176，返回值
// 不检查继续）。new std::map<string,STEquipmentAniScript>(0x18) → 存入
// g_mapEquipmentAniScript(0x94f7bf8) → ImportEquipmentAniScript(map)。
// ============================================================================
void InitMapEquipmentAniScript(void)
{
    g_mapEquipmentAniScript = new std::map<std::string, STEquipmentAniScript>();
    ImportEquipmentAniScript(g_mapEquipmentAniScript);
}

// ============================================================================
// WorldMapScript 静态图案表（ORIG nm 真实静态成员；权威声明见 WorldMapScript.h，
// 本 TU 提供定义）。
//
// 0x28 = STWorldmapPattern（string + int + coord[3][2] 聚合@+0x08 + int + int）。
// 0x28 = STWorldmapPattern（string + 9 int；coord[3][2] 落在 +0x08..+0x24）。
// aWorldMapPatternTable2ND_E（B 0x095014e0）：[20] 条目，每条目 0x1c =
// STWorldmapPattern2ND（{ int base@+0 ; int coord[3][2]@+4 }）。
// ============================================================================
// ---- 本 TU 下方定义的依赖（前置声明）----
bool loadWorldMapCoordPattern(void);   // _Z24loadWorldMapCoordPatternv 0x08ab0234
float ScanAbsOrRel(bool& isAbs, std::string& str, bool& gotValue, bool consume);
                                       // _Z12ScanAbsOrRelRbRSsS_b 0x088bcbef

#include "WorldMapScript.h"

// 静态表定义（权威类型见 WorldMapScript.h；ORIG B 0x09501020 / 0x095014e0）。
STWorldmapPattern WorldMapScript::aWorldMapPatternTable_[3][10];
STWorldmapPattern2ND WorldMapScript::aWorldMapPatternTable2ND_[20];

// initWorldMapScript —— ORIG 0x08ab000c（init.md:2555 +0x3314；失败路径直接
// return false 由 init 终止）。四步：
// ① 目录拷贝（0x9500f00，0x100）+ loadRDARScriptListFile(0x9501000)；
// ② loadRDARScriptFile(baseDir,"WorldMapPatternTable.tbl")（串 0x8e2bd63）；
// ③ 三重循环写 aWorldMapPatternTable_E：外层 i=0..2 每次 ScanType(token,true)
//   失败即跳出（仍继续第④步）；中层 j=0..9 先 name=token，再 k=0..2/l=0..1
//   ScanInt 写 coord[k][l]；
// ④ 调 loadWorldMapCoordPattern() 后返回 true。
// ============================================================================
bool initWorldMapScript(char* dir, char* list)
{
    strncpy(g_worldMapScriptBaseDirectory, dir, 0x100);
    if (g_worldMapScriptBaseDirectory[strlen(g_worldMapScriptBaseDirectory) - 1] != '\\' &&
        g_worldMapScriptBaseDirectory[strlen(g_worldMapScriptBaseDirectory) - 1] != '/')
    {
        memcpy(g_worldMapScriptBaseDirectory + strlen(g_worldMapScriptBaseDirectory), "\\", 2);
    }
    if (!loadRDARScriptListFile(&g_worldMapScriptFileList,
                                g_worldMapScriptBaseDirectory, list, 0))
        return false;
    if (!loadRDARScriptFile(g_worldMapScriptBaseDirectory,
                            "WorldMapPatternTable.tbl"))
        return false;

    std::string token;
    for (int i = 0; i <= 2; ++i)
    {
        if (!ScanType(token, true))
            break;                       // ORIG jne 0x8ab01f7 → 直达 ④
                for (int j = 0; j <= 9; ++j)
        {
            // ORIG 0x8ab013b/0x8ab017a：完整表达式逐次重算 i/j 偏移
            // （(i*50+j*5+k)*2+l 复合索引），不缓存条目引用。
            WorldMapScript::aWorldMapPatternTable_[i][j].m_str0 = token;
            for (int k = 0; k <= 2; ++k)
            {
                for (int l = 0; l <= 1; ++l)
                {
                    WorldMapScript::aWorldMapPatternTable_[i][j]
                        .m_coord.m_coord[k][l] = ScanInt((bool*)0);
                }
            }
        }
    }
    loadWorldMapCoordPattern();
    return true;
}

// ============================================================================
// loadWorldMapCoordPattern —— ORIG 0x08ab0234（0x154 字节；报告
// _lt_global_gt_/loadWorldMapCoordPattern.md）。
// ① loadRDARScriptFile(baseDir,"WorldMapPatternTable2ND.tbl")
//   （串 0x8e2bd7c）失败 return false；
// ② 循环 i=0..0x13（20 条）：ScanAbsOrRel(isAbs,token,got,true) 返回 float，
//   fstcw/fldcw(ah=0xc)+fistp 即 C 截断 (int)→entry[i].m_base；
//   got==false 时跳出；随后 j=0..2/k=0..1 ScanInt 写 entry[i].m_coord[j][k]；
// ③ return gotValue（最后一次扫描结果）。
// ============================================================================
bool loadWorldMapCoordPattern(void)
{
    if (!loadRDARScriptFile(g_worldMapScriptBaseDirectory,
                            "WorldMapPatternTable2ND.tbl"))
        return false;

    bool isAbs = true;                   // ebp-0x29 初值 1
    bool gotValue = false;               // ebp-0x2a 初值 0
    std::string token;
    for (int i = 0; i <= 0x13; ++i)
    {
        int base = (int)ScanAbsOrRel(isAbs, token, gotValue, true);
        if (!gotValue)
            break;                       // test al,al; jne 0x8ab0351
        WorldMapScript::aWorldMapPatternTable2ND_[i].m_base = base;
        for (int j = 0; j <= 2; ++j)
        {
            for (int k = 0; k <= 1; ++k)
            {
                WorldMapScript::aWorldMapPatternTable2ND_[i].m_coord[j][k] =
                    ScanInt((bool*)0);
            }
        }
    }
    return gotValue;                     // movzx ebx,[ebp-0x2a]
}

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
// ============================================================================
const char* toTString(const char* pszSrc);   // SyncScriptDeps.h 权威声明（避免拖入其依赖）

float ScanAbsOrRel(bool& isAbs, std::string& str, bool& gotValue, bool consume)
{
    float result = 0;
    isAbs = true;
    for (;;)
    {
        int tok = g_dnfLex.getToken();
        if (tok == 2)
        {
            result = (float)g_dnfLex.getDecimal();
            isAbs = true;
            gotValue = true;
        }
        else if (tok == 3)
        {
            result = (float)g_dnfLex.getHexDecimal();
            isAbs = true;
            gotValue = true;
        }
        else if (tok == 4)
        {
            result = g_dnfLex.getFloat();
            isAbs = true;
            gotValue = true;
        }
        else if (tok == 5)
        {
            str = toTString(g_dnfLex.getType());
            std::string::size_type pos = str.find("[", 0);
            str.erase(0, pos + 1);
            pos = str.find("]", 0);
            str.erase(pos, str.size());
            result = (float)atof(str.c_str());
            isAbs = false;
            gotValue = true;
        }
        else
        {
            isAbs = true;
            gotValue = false;
        }

        if (tok == 0 || tok == 2 || tok == 3 || tok == 4 || tok == 5)
            return result;

        if (consume)
        {
            std::string streamName = g_dnfLex.getStreamName();
            std::string lastData = g_dnfLex.getLastTokenDataAsString();
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 1496,
                "ERROR! %s - TYPE mismatch. %s:%d, token = '%s'",
                "ScanAbsOrRel", streamName.c_str(), g_dnfLex.getLastLineNo(),
                lastData.c_str());
        }
    }
}
