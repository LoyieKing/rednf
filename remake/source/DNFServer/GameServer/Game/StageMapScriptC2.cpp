// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StageMapScriptC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "StageMapScriptC2/clear.inc"
}

#include "StageMapScriptC2/dtor_DistrictData.inc"
}

// 两 ctor 置于各自 clear() 定义前：callee 未知时 4.4 的 outgoing 保守分配
// 恰为 ORIG 帧 0x18；clear 定义在前则继承其小帧（sub $0x4）失配。
#include "StageMapScriptC2/clear_1.inc"
}

#include "StageMapScriptC2/SlotType.inc"
}

#include "StageMapScriptC2/clear_2.inc"
}

#include "StageMapScriptC2/DistrictData.inc"
}

#include "StageMapScriptC2/AreaData.inc"
}

#include "StageMapScriptC2/dtor_AreaData.inc"
}

#include "StageMapScriptC2/clear_3.inc"
}

#include "StageMapScriptC2/clear_4.inc"
}

#include "StageMapScriptC2/StageMapScript.inc"
}

#include "StageMapScriptC2/dtor_StageMapScript.inc"
}

}  // namespace stagemap

// ORIG T 0x088a9f28.  This is a direct C++ transcription of the report's
// ScanType/ScanInt state machine.  In particular, [stage] consumes rows of
// four integers until the first failed ScanInt, then appends the current
// district; [slot type] consumes five-integer records and only inserts a new
// key; and [/area] appends the area without appending the pending district.
// [本波收敛] ORIG 0x088a9f28 / 457 insns；本波 475→459（纯指令差 2 条）：
//   ① P160-P162 槽复用：全部 ScanInt 结果写 ORIG 同一槽 -0xc（单具名 int），
//      13 个 v0..v4 独立局部曾使帧深 0x38（帧 0xd0→0xa0 与 ORIG 全等）；
//   ② P164 显式回边：`scan_next:` 标签 + `goto finish`/`goto scan_next`
//      复现 ORIG「入口直落 + 双 jmp/nop 岛」（ORIG +0x50/+0x53）;
//   ③ P222 正向包裹：`if (token == "[area]") {...}` 消掉 4.4 的 `xor $1` 物化；
//   ④ 去 finalizeAfterArea 具名 bool（ORIG 用 ebx 寄存器）⇒ 槽再上移 4；
//   ⑤ StageData 用户空析构删除（P12：ORIG D1/D2 为空体 W，且 importStageMapScript
//      全程不调用它）⇒ [stage] 失败路径的 dtor 调用消失，missing 86→82；
//   ⑥ operator[] 结果用 `register SlotType&` 绑定（非 register 会落槽 +6 条）。
// 残差 2 条（N 封顶，均有 ORIG 指令形证据）：
//   · 0x088aa2e7 段 ORIG `mov SLOT,%edx; mov %edx,(%eax)` vs 4.4 把引用绑进
//     %ebx 并多 1 条 `mov %eax,%ebx`（引用/间接基址寄存器分配差，N7 族）；
//   · 0x088aa46c 岛区 4.4 多 1 条 `jmp`（多目标共享 continue 岛布局，
//     负例「空 else nop;jmp 岛 vs jump-threading」族）。
// [C05 范式复查留痕] 已按 Main 转发的 F1（尾部 fail 块）/F2（int 局部⇒cmp 内存
//   直比）/③（P223 早返中继岛）逐条复查本件残差，**均不适用**：
//   · 本函数无 `cmp` 类残差（元组不同），F2 无对象；
//   · 残差 jmp 经 objdump 目标分析为 **4.4 跳转合并跳板**：6 处 `jmp 962`（其中
//     0x8f3/0x92b 用 rel8、0x61f/0x64c/0x685/0x805 用 rel32）+ 跳板 `962: jmp 5bc`
//     共 7 条 vs ORIG 6 条直跳 `jmp 088a9f7e`；属编译器跳转合并布局（P223 管的是
//     「早返 return 各发 jmp+nop 中继岛」，形不同，已实测非其族）；
//   · 另一条 `mov %eax,%ebx` 为间接基址寄存器分配差（N7/N18a 族）。
//   · D08/C05③ **试前闸门**（Main 定：我方岛数 − ORIG 岛数 ≥ 2 才试合并）：本件
//     岛数 **3 vs 3 相同**（0x088aa46c/46f/472 ↔ 我方 960/968/96b），差 = 0 ⇒
//     按闸门**不试**；实测反例族（A06 `_onChangeGrowType` 合并 `||` 反劣 +4、
//     A28 `generateRandomItem` 改 `goto end` 无效）亦支持判 A28 epilogue-threading。
#include "StageMapScriptC2/importStageMapScript.inc"
}
