// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StageMapScriptC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// StageMapScriptC2.cpp — stagemap script restoration (exclusive TU)
//
// ORIG symbols restored here:
//   stagemap::StageMapScript::clear()                    W 0x08373428
//   stagemap::StageMapScript::StageMapScript()           W 0x08373488
//   stagemap::StageMapScript::~StageMapScript()          W 0x0837f782
//   importStageMapScript(stagemap::StageMapScript*,    T 0x088a9f28
//                         char const*)
//
// StageMapList::makeStageMapUniqueId(int,int,int), ORIG T 0x08364818, is
// already provided by StrikerWorldDropB2.cpp and is intentionally not
// duplicated here.
//
// The stagemap POD layouts below are taken from the ORIG constructors,
// clear functions, and importStageMapScript offsets.  The project toolchain
// uses the legacy libstdc++ string ABI (sizeof(std::string)==4), so these
// declarations have the same sizes and offsets as df_game_r:
//   StageData  0x10, SlotType 0x14, DistrictData 0x18,
//   AreaData   0x30, StageMapScript 0x20.

#include <map>
#include <string>
#include <vector>

#include "DNFLexWrapper.h"
#include "NpcScript.h"


extern char g_stageMapScriptBaseDirectory[0x100];
extern STScriptFileList g_stageMapScriptFileList;

namespace stagemap
{

// ORIG W 0x088aa604 (C1/C2), 0x088ac168 (D1/D2), 0x088aa5d8 (clear).
struct StageData
{
    StageData();
    void clear();

    int m_field0;  // +0x00
    int m_field4;  // +0x04
    int m_field8;  // +0x08
    int m_fieldC;  // +0x0c
};

// ORIG W 0x088aa6fc (C1/C2), 0x088aa6c6 (clear).  No ORIG destructor
// symbol is emitted: all five fields are plain integers.
struct SlotType
{
    SlotType();
    void clear();

    int m_field0;   // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
    int m_fieldC;   // +0x0c
    int m_field10;  // +0x10
};

// ORIG W 0x088aa658 (C1/C2), 0x083e803c (D1/D2), 0x088aa618 (clear).
// The implicit copy constructor/assignment are memberwise, matching ORIG
// W 0x088ab100 / 0x088ab1d2 used by vector::push_back.
struct DistrictData
{
    DistrictData();
    ~DistrictData();
    void clear();

    int m_field0;                       // +0x00, district index
    std::vector<StageData> m_vec4;     // +0x04, [stage] rows
    std::string m_appendImage;         // +0x10, [append image]
    int m_field14;                     // +0x14, trailing [append image] int
};

// ORIG W 0x088aa75e (C1/C2), 0x083dd7bc (D1/D2), 0x088aa710 (clear).
// The implicit copy constructor/assignment are memberwise, matching ORIG
// W 0x088ab532 / 0x088ab634.
struct AreaData
{
    AreaData();
    ~AreaData();
    void clear();

    int m_field0;                       // +0x00, area index
    std::string m_slotImage;            // +0x04, [slot image]
    std::map<int, SlotType> m_slots;   // +0x08, [slot type]
    std::vector<DistrictData> m_vec20; // +0x20, district list
    int m_field2c;                      // +0x2c, [survival]
};

// ORIG W 0x08373488 (C1/C2), 0x0837f782 (D1/D2), 0x08373428 (clear).
struct StageMapScript
{
    StageMapScript();
    ~StageMapScript();
    void clear();

    int m_fileIndex;                    // +0x00, final GetIndex result
    int m_dungeonIdx;                   // +0x04, [dungeon]
    int m_field8;                       // +0x08, [map image] trailing int
    std::string m_name;                 // +0x0c, [name]
    std::string m_mapImage;             // +0x10, [map image]
    std::vector<AreaData> m_areaVec;    // +0x14, [area] list
};

#include "StageMapScriptC2/StageData.inc"


#include "StageMapScriptC2/clear.inc"


#include "StageMapScriptC2/dtor_DistrictData.inc"


#include "StageMapScriptC2/clear_1.inc"


#include "StageMapScriptC2/SlotType.inc"


#include "StageMapScriptC2/clear_2.inc"


#include "StageMapScriptC2/DistrictData.inc"


#include "StageMapScriptC2/AreaData.inc"


#include "StageMapScriptC2/dtor_AreaData.inc"


#include "StageMapScriptC2/clear_3.inc"


#include "StageMapScriptC2/clear_4.inc"


#include "StageMapScriptC2/StageMapScript.inc"


#include "StageMapScriptC2/dtor_StageMapScript.inc"


}  // namespace stagemap

#include "StageMapScriptC2/importStageMapScript.inc"

