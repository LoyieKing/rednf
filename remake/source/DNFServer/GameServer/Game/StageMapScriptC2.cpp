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
    ~StageData();
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

StageData::~StageData()
{
}

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
// [b28d-24] DIFF 残279: P167 EXTRA push_back×3/StageData D1×2/ne×1（解析分支结构差异），待按 P165/P166 收敛。ORIG 0x088a9f28
bool importStageMapScript(stagemap::StageMapScript* out, const char* path)
{
    if (!loadRDARScriptFile(g_stageMapScriptBaseDirectory, path))
        return false;

    out->clear();

    std::string token;
    bool ok = false;

    for (;;)  // outer token loop
    {
        if (!ScanType(token, true))
            break;

        if (token == "[name]")
        {
            (void)ScanStr(&out->m_name);
            continue;
        }

        if (token == "[map image]")
        {
            (void)ScanStr(&out->m_mapImage);
            out->m_field8 = ScanInt((bool*)0);
            continue;
        }

        if (token == "[dungeon]")
        {
            int dungeon = ScanInt(&ok);
            if (!ok)
                break;  // ORIG finalizes directly at +0x5da
            out->m_dungeonIdx = dungeon;
            continue;
        }

        // [本波·P222] p167 EXTRA operator!=(string,char const*) / MISSING
        // operator==×1 ⇒ ORIG 用 `==` 形式（我方原写 `!=` 选错重载）。
        if (!(token == "[area]"))
            continue;

        // The first [area] integer is special: a failed scan selects the
        // ORIG fatal/finalize path (ebx=0), rather than merely ending this
        // area.  The lexical scope still destroys AreaData as ORIG does.
        bool finalizeAfterArea = false;
        {
            std::string areaToken;
            stagemap::AreaData area;
            area.clear();

            int areaIndex = ScanInt(&ok);
            if (!ok)
            {
                finalizeAfterArea = true;
            }
            else
            {
                area.m_field0 = areaIndex;
                stagemap::DistrictData district;
                district.clear();

                for (;;)  // token loop within [area]
                {
                    if (!ScanType(areaToken, true))
                        break;  // ORIG cleanup, then resume outer loop

                    if (areaToken == "[district]")
                    {
                        district.clear();
                        int districtIndex = ScanInt(&ok);
                        if (!ok)
                            break;  // ORIG +0x547 cleanup path
                        district.m_field0 = districtIndex;
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

                            int v0 = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field0 = v0;

                            int v1 = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field4 = v1;

                            int v2 = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field8 = v2;

                            int v3 = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_fieldC = v3;

                            int v4 = ScanInt(&ok);
                            if (!ok)
                                break;
                            slot.m_field10 = v4;

                            // ORIG performs find first and only fills a new
                            // key; an existing slot type is left untouched.
                            if (area.m_slots.find(slot.m_field0) ==
                                area.m_slots.end())
                            {
                                stagemap::SlotType& dst =
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

                            int v0 = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;   // P276 共享调用点（ORIG 单处）
                            stage.m_field0 = v0;

                            int v1 = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_field4 = v1;

                            int v2 = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_field8 = v2;

                            int v3 = ScanInt(&ok);
                            if (!ok)
                                goto stage_rows_done;
                            stage.m_fieldC = v3;
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
                        int survival = ScanInt(&ok);
                        if (!ok)
                            break;  // ORIG +0x54a cleanup path
                        area.m_field2c = survival;
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

        if (finalizeAfterArea)
            break;
    }

    out->m_fileIndex = g_stageMapScriptFileList.GetIndex(path);
    return true;
}
