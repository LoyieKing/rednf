// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STDungeonScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CDungeon.h"
#include "CEnvironment.h"             // less_TCHAR：ORIG map 按字符串内容比较
#include "DNFLexWrapper.h"
#include "DataManagerInitDepends.h"   // getCharacterJobToEnumValue 声明（[impossible skills]）
#include "LogManager.h"               // ORIG clear-condition 错误日志
#include "NpcScript.h"                // STScriptFileList（[schedule] 双视图）
#include "STWorldmapPattern.h"        // STWorldmapPattern（[worldmap pattern info] 局部）

#include "STDungeonScript/GetWorldMapCoordFromTable.inc"

extern std::map<int, std::string> g_stDungeonScriptFileList;   // 定义 InitScriptProducers_4.cpp（双视图 STScriptFileList）

// ORIG _ZL13g_gridSymbols（[greed] 网格符号，16 项两字符编码；STMapScript.cpp 同表为文件内
// static，本 TU 单独复现以支撑 import/exportDungeonScript 引用）。
// ORIG nm 实证：该符号类型 = **d**（.data，可写指针数组，尺寸 0x40）；我方原 `const char* const`
// 落 .rodata（r）⇒ 去掉内层 const 使数组本身可写（指向内容仍 const），段归属对齐。
static const char* g_gridSymbols[16] = {
    "AA", "BB", "CC", "DD", "EE", "FF", "GG", "HH",
    "II", "JJ", "KK", "LL", "MM", "NN", "OO", "PP",
};

// ORIG B 0x94f6da0（权威定义 InitScriptProducers_4.cpp；目录前缀，import 传 loadRDARScriptFile）
extern char g_szDungeonScriptBaseDirectory[0x100];

#include "STDungeonScript/ConvertStringToTime.inc"
#include "STDungeonScript/ConvertWeekdayStringToInt.inc"
#include "STDungeonScript/ConvertHHMMSSStringToInt.inc"
                              unsigned char& sec);

// ORIG nm 实证：Clear/C1/C2/D1/D2 全为 **W**（0x0836abc6/0x0836b5e2/0x0836bd5c，
// 尺寸 0x856/0x779/0x789 与我方逐位相同）⇒ 原码是 CDungeon.h 类内 inline 定义。
// 按 Main 已批准的 G_* 单点修法：.cpp 定义加 weak 属性对齐绑定（mangled/尺寸零变化）。
__attribute__((weak)) STDungeonScript::STDungeonScript()
{
    Clear();
}

__attribute__((weak)) STDungeonScript::~STDungeonScript()
{
}

__attribute__((weak)) void STDungeonScript::Clear()
{
    // b28d-solo-STDungeonScript 判词：AE（473=473、p167 48=48 CLEAR）。
    // 修法：①函数级 i 声明+复用——ORIG 5 个物理循环共用 -0x18（首条
    //   `int i=0;` + 各 `for(i=0;…)`），块内 j 占 -0x14/-0x10、末循环 i 占 -0xc
    //   ⇒ 我方 7 槽降到 4 槽、帧 0x34→0x24（P212/P309 槽序）；②m_5c 匿名 union
    //   二维视角 `m_5cGrid[3][2]`（CDungeon.h，ORIG 0x0836acd0 `add %ecx,%ecx;
    //   lea (%ecx,%edx,1),%edx`，一维写法 4.4.6 恒产 add mem 形，P235）。
    // ORIG 0x0836abc6（473 insn 二进制切片逐段转写）：语句序、string 混用形
    // （assign "" = _ZNSsaSEPKc vs _ZNSs5clearEv）与物理循环全部按汇编还原。
    m_18 = 0;                                        // 0836abd0
    m_str1c = "";                                    // 0836abe8 assign
    m_str20 = "";
    m_str24 = "";
    m_str28 = "";
    m_2c = 0;
    m_str30 = "";
    m_34 = 0;
    m_vec38.clear();
    m_44 = 0;
    m_vec48.clear();
    m_14 = 0;
    m_vec3c8.clear();                                // 0836ac94 vector<STNamedMonsterMapPos>
    m_0 = 0;
    m_c = 0;                                         // movb 0836aca5
    // ORIG 0x0836aca9/0836acb0：函数级 i 声明+初始化后由后续物理循环复用
    // （同槽 -0x18；块内 j/i 各占 -0x14/-0x10/-0xc）。P212/P309 槽序还原。
    int i = 0;
    // 0836acb0：ORIG 物理双层循环 (i<=2)×(j<=1)，地址 0xc+4*(0x14+2i+j)=+0x5c..
    for (i = 0; i <= 2; ++i)
        for (int j = 0; j <= 1; ++j)
            m_5cGrid[i][j] = 0;
    m_str10.clear();                                 // 0836ad02 本串用 clear 形
    m_str74 = "";                                    // 0836ad18 assign
    m_78 = 0;
    m_requiredItem7c.clear();                        // 0836ad30 RequiredItem::clear
    m_a4 = -1;
    m_str54 = "";                                    // 0836ad53 assign
    m_88 = 0;                                        // movb
    m_requiredItem8c.clear();
    m_vec98.clear();
    m_a8 = -1;
    m_ac = 1;
    m_b0 = 1.0f;
    for (i = 0; i <= 4; ++i)                         // 0836adb0 物理循环 +0xb4+4i
        m_b4[i] = 0;
    m_vecc8.clear();                                 // 0836adde
    // 0836ade3：ORIG 物理双层循环 (i<=3)×(j<=2)，地址 +0xf8+(3i+j)*4 连续 12 int 写 -1
    for (i = 0; i <= 3; ++i)
        for (int j = 0; j <= 2; ++j)
            m_pathgate.m_cells[i][j] = -1;
    m_mazeList170.clear();                           // 0836ae3a vector<MazeScript>
    m_str17c = "";                                   // 0836ae52 assign
    m_str1c.clear();                                 // 0836ae60：ORIG 对 +0x1c 二次操作
    m_str20.clear();                                 // 0836ae6e：同上 +0x20
    m_warRoom18c.clear();                            // 0836ae7e STWarRoomData::clear
    m_deadTower180.clear();                          // 0836ae8e STDeadTowerData::clear
    m_28c = 0;
    m_28d = 0;
    m_28e = 0;
    m_2ab = 0;
    m_2ac = 0;
    memset(&m_2b0, 0, 0xc);                          // 0836aee3 memset 形
    memset(&m_2bc, 0, 0xc);                          // 0836af03
    m_vec2c8.clear();
    m_clearReward2d4.clear();                        // advancealtar::ClearRewardData
    m_survival2f8.m_vec.clear();                     // vector<RoundRewardData>
    m_304 = 0;
    m_305 = 0;
    m_306 = 0;
    m_314 = 0;
    m_315 = 0;
    m_316 = 0;
    m_317 = 0;
    m_dailySchedule.clear();
    m_324 = 4;                                       // ORIG movb $0x4,0x324：默认队伍人数上限
    m_325 = 0;
    m_bossRoom328.clear();
    m_140 = 0;
    m_141 = 0;
    m_142 = 1;                                       // ORIG movb $0x1,0x142
    m_vec144.clear();
    m_150 = 0;
    m_154 = 0;
    m_158 = 0;
    m_159 = 1;                                       // ORIG movb $0x1,0x159
    for (i = 0; i <= 4; ++i)                         // 0836b00c 物理循环 +0x128+4i
        m_128[i] = 0;
    m_13c = 0;
    m_168 = -1;                                      // ORIG movl $0xffffffff,0x168
    m_vecd4.clear();
    m_vece0.clear();
    m_vecec.clear();                                 // 0836b07a vector<stDungeonAssignItem_t>
    m_16c = 0;
    m_338 = 0;
    m_33c = 0;
    m_map340.clear();
    m_358 = 0;
    m_vec35c.clear();
    m_368 = 0;
    m_36c = 1;
    m_370 = 1;
    m_374 = 1.0f;
    m_378 = 1.0f;
    m_37c = 1;
    m_380 = 1;
    m_vec384.clear();
    m_towerDialog390.clear();                        // STTowerFullDialog::clear
    m_3b4 = 1;                                       // ORIG movl $0x1,0x3b4
    m_3b8 = 0;
    m_3bc = 0;
    m_3c0 = 0;
    m_3d4 = 0;
    m_3d8 = 0;
    m_3dc = 0;
    m_vec3e0.clear();
    m_3ec = 0;
    m_3f0 = 0;
    m_3f4 = 0;
    m_vec3f8.clear();
    m_vec404.clear();
    for (int i = 0; i <= 4; ++i)                     // 0836b202 同一循环写 +0x410/+0x424
    {
        m_410[i] = 0;
        m_424[i] = 0;
    }
    m_438 = 0.0f;
    m_43c = 2.0f;
    m_440 = 0.0f;
    m_444 = 1.5f;
    m_str448.clear();                                // ORIG 此处用 _ZNSs5clearEv
    m_44c = 1;                                       // ORIG movb $0x1,0x44c
    m_454 = -1;                                      // ORIG 先写 0x454…
    m_450 = m_454;                                   // …再回读 0x454→0x450（0836b2a3）
    m_vec458.clear();
    m_vec464.clear();
    m_vec470.clear();
    m_47c = 0;
    m_484 = 0;                                       // ORIG 顺序 0x484 先于 0x480
    m_480 = 0;
    m_488 = 0;
    m_48c = 0;
    m_2a9 = 0;
    m_2aa = 0;
    m_490 = -1;                                      // ORIG movl $0xffffffff,0x490
    m_494 = 0;
    m_498 = 1;                                       // ORIG movl $0x1,0x498
    m_map49c.clear();
    m_itemRate4b4.Clear();
    m_4c4Value = 1.0f;
    m_map4c8.clear();
    m_4e0 = 0;
    m_map4e4.clear();
    m_500 = -1;                                      // ORIG 顺序 0x500 先于 0x4fc
    m_4fc = 0;
    m_504 = 0;
    m_505 = 0;
    m_28f = 0;
    m_statMap290.clear();
    m_2a8 = 0;
    m_508 = 0;
    m_50c = 0;
    // ORIG 未清：m_4/m_8、m_str58、m_vec15c、m_dimension308（按 ORIG 实证保留不清）
}
#include "STDungeonScript/ImportDungeonScript.inc"




#include "STDungeonScript/getScriptFileFullPath.inc"
                           const char* name);
#include "STDungeonScript/convertBinaryToText.inc"

#include "STDungeonScript/exportDungeonScript.inc"


#include "STDungeonScript/getStageType.inc"


#include "STDungeonScript/getDifficulty.inc"


#include "STDungeonScript/getRewardItemType.inc"


#include "STDungeonScript/ImportAdvanceAltarDungeonTag.inc"

