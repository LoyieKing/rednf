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

StageData::StageData()
{
    clear();
}

void DistrictData::clear()
{
    m_field0 = 0;
    m_appendImage = "";
    m_field14 = 0;
    m_vec4.clear();
}

DistrictData::~DistrictData()
{
}

// 两 ctor 置于各自 clear() 定义前：callee 未知时 4.4 的 outgoing 保守分配
// 恰为 ORIG 帧 0x18；clear 定义在前则继承其小帧（sub $0x4）失配。
void StageData::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
}

SlotType::SlotType()
{
    clear();
}

void SlotType::clear()
{
    m_field0 = -1;
    m_field4 = -1;
    m_field8 = -1;
    m_fieldC = -1;
    m_field10 = -1;
}

DistrictData::DistrictData()
{
    clear();
}

AreaData::AreaData()
{
    // ORIG C1 only constructs m_slotImage, m_slots, and m_vec20; it does not
    // call clear().  Their C++ member initializers perform those operations.
}

AreaData::~AreaData()
{
}

void AreaData::clear()
{
    m_field0 = 0;
    m_slots.clear();
    m_vec20.clear();
    m_slotImage = "";
    m_field2c = 0;
}

void StageMapScript::clear()
{
    m_fileIndex = 0;
    m_dungeonIdx = 0;
    m_name = "";
    m_mapImage = "";
    m_field8 = 0;
    m_areaVec.clear();
}

StageMapScript::StageMapScript()
{
    clear();
}

StageMapScript::~StageMapScript()
{
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
bool importStageMapScript(stagemap::StageMapScript* out, const char* path)
{
    if (!loadRDARScriptFile(g_stageMapScriptBaseDirectory, path))
        return false;

    out->clear();

    std::string token;
    // [本波·P160-P162 槽复用] ORIG 全部 ScanInt 结果写同一槽 -0xc
    // （0x088aa019/0x088aa086/0x088aa10b/0x088aa1ca… 全为 `mov %eax,-0xc(%ebp)`）
    // ⇒ 源码只有一个被复用的具名 int；写成 v0..v4 独立局部会让帧深 +0x38。
    int scanValue = 0;
    bool ok = false;

    for (;;)  // outer token loop
    {
    scan_next:                     // [P164] 显式回边落点：入口直落条件头
        if (!ScanType(token, true))
            goto finish;

        if (token == "[name]")
        {
            (void)ScanStr(&out->m_name);
            goto scan_next;
        }

        if (token == "[map image]")
        {
            (void)ScanStr(&out->m_mapImage);
            out->m_field8 = ScanInt((bool*)0);
            goto scan_next;
        }

        if (token == "[dungeon]")
        {
            scanValue = ScanInt(&ok);
            if (!ok)
                goto finish;  // ORIG finalizes directly at +0x5da
            out->m_dungeonIdx = scanValue;
            goto scan_next;
        }

        // [P222] 正向包裹：ORIG 0x088aa051 为 `test %al,%al; je <loop_head>`
        // （无 xor 物化）；写 `if (!(...)) goto` 会让 4.4 先 xor $1 再分支。
        if (token == "[area]")
        {
        // The first [area] integer is special: a failed scan selects the
        // ORIG fatal/finalize path (ebx=0), rather than merely ending this
        // area.  The lexical scope still destroys AreaData as ORIG does.
            std::string areaToken;
            stagemap::AreaData area;
            area.clear();

            scanValue = ScanInt(&ok);
            if (!ok)
            {
                // [P164] ORIG 0x088aa099 `mov $0x0,%ebx` → +0x4b4 共享析构块
                // → 收尾 return true（不设具名标志：ORIG 用 ebx 寄存器，
                // 4.4 的具名 bool 会多占一个栈槽并整体加深帧 0x4）。
                goto finish;
            }
            else
            {
                area.m_field0 = scanValue;
                stagemap::DistrictData district;
                district.clear();

                for (;;)  // token loop within [area]
                {
                    if (!ScanType(areaToken, true))
                        break;  // ORIG cleanup, then resume outer loop

                    if (areaToken == "[district]")
                    {
                        district.clear();
                        scanValue = ScanInt(&ok);
                        if (!ok)
                            break;  // ORIG +0x547 cleanup path
                        district.m_field0 = scanValue;
                        continue;
                    }

                    if (areaToken == "[slot image]")
                    {
                        (void)ScanStr(&area.m_slotImage);
                        continue;
                    }

                    if (areaToken == "[append image]")
                    {
                        (void)ScanStr(&district.m_appendImage);
                        district.m_field14 = ScanInt((bool*)0);
                        continue;
                    }

                    if (areaToken == "[slot type]")
                    {
                        for (;;)  // five-int slot records
                        {
                            stagemap::SlotType slot;
                            slot.clear();

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field0 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field4 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field8 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_fieldC = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field10 = scanValue;

                            // ORIG performs find first and only fills a new
                            // key; an existing slot type is left untouched.
                            if (area.m_slots.find(slot.m_field0) ==
                                area.m_slots.end())
                            {
                                register stagemap::SlotType& dst =
                                    area.m_slots[slot.m_field0];
                                dst.m_field0 = slot.m_field0;
                                dst.m_field4 = slot.m_field4;
                                dst.m_field8 = slot.m_field8;
                                dst.m_fieldC = slot.m_fieldC;
                                dst.m_field10 = slot.m_field10;
                            }
                        }
                        continue;
                    }

                    if (areaToken == "[stage]")
                    {
                        for (;;)  // rows of four ints
                        {
                            stagemap::StageData stage;
                            stage.clear();

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;   // P276 共享调用点（ORIG 单处）
                            stage.m_field0 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_field4 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_field8 = scanValue;

                            scanValue = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_fieldC = scanValue;
                            district.m_vec4.push_back(stage);
                        }
                    stage_rows_done:
                        // [本波·P276] p167 EXTRA push_back<DistrictData>×3 ⇒
                        // ORIG 四条失败分支 **goto 汇入单一 push_back**
                        // （原写四份独立调用点）。
                        area.m_vec20.push_back(district);
                        continue;
                    }

                    if (areaToken == "[survival]")
                    {
                        scanValue = ScanInt(&ok);
                        if (!ok)
                            break;  // ORIG +0x54a cleanup path
                        area.m_field2c = scanValue;
                        continue;
                    }

                    if (areaToken == "[/area]")
                    {
                        out->m_areaVec.push_back(area);
                        break;  // pending district is intentionally not added
                    }

                    // Unknown area tags are ignored just as the ORIG chain of
                    // string comparisons does.
                }
            }
        }
        goto scan_next;
    }

finish:
    out->m_fileIndex = g_stageMapScriptFileList.GetIndex(path);
    return true;
}
