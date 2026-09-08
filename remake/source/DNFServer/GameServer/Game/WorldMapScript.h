#ifndef GAME_WORLDMAPSCRIPT_H_
#define GAME_WORLDMAPSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— WorldMapScript / WorldMapDungeon（WorldMap/ 脚本域）。
// 布局证据：docs/class_func_reports/_lt_global_gt_/importWorldMapScript.md、
// WorldMapScript::WorldMapScript 0x08369f08 W（6 个子对象构造顺序）、
// ~WorldMapScript 0x0837f5ae W、CWorldMap::set_world_map 0x0836421a T
// （消费 +0x00/+0x14/+0x20/+0x24/+0x30/+0x3c）。
// ============================================================================

#include <string>
#include <utility>
#include <vector>

#include "STWorldmapPattern.h"    // aWorldMapPatternTable_E 元素（0x28）
#include "STWorldmapPattern2ND.h" // aWorldMapPatternTable2ND_E 元素（0x1c）

// ORIG WorldMapDungeon（ctor 0x08ab09e8 W / clear 0x08ab09c6 W /
// D1 0x08ab0f74 W）：clear 置 [-1, -1, 0]。
struct WorldMapDungeon
{
    int m_dungeonIndex;   // +0x00（clear 置 -1；import [dungeon] ScanInt 首值）
    int m_state;          // +0x04（clear 置 -1）
    int m_flag;           // +0x08（clear 置 0）

    WorldMapDungeon() { clear(); }   // ORIG ctor 先 clear
    void clear()
    {
        m_dungeonIndex = -1;
        m_state = -1;
        m_flag = 0;
    }
    ~WorldMapDungeon() {}
};  // sizeof 0xc

class WorldMapScript
{
public:
    WorldMapScript();   // ORIG _ZN14WorldMapScriptC1Ev 0x08369f08 W
    ~WorldMapScript();  // ORIG _ZN14WorldMapScriptD1Ev 0x0837f5ae W
    void clear();       // ORIG _ZN14WorldMapScript5clearEv（importWorldMapScript 开头调用）

    int m_index;                                  // +0x00（FindIndexByFullScan 回写）
    std::string m_name;                           // +0x04 ([name])
    std::string m_mapImage;                       // +0x08 ([map image])
    std::string m_uiPath;                         // +0x0c ([ui path])
    int m_field10;                                // +0x10（[map image] 后 ScanInt）
    std::vector<WorldMapDungeon> m_dungeons;      // +0x14 ([dungeon])
    // [b28d-Main 代落·S139 P292/P248档②] unsigned char→bool：消 set_world_map
    // 0x08… 的 `test; setne %dl` 物化（ORIG 直 movzbl→mov）。1 字节宽度不变，
    // sizeof/偏移/mangled 零影响；消费点 CtorProducers_1.cpp:889/976/1081。
    bool m_isHellDungeon;                         // +0x20 ([hell dungeon])
    std::vector<int> m_hellQuests;                // +0x24 ([hell quest])
    std::vector<std::pair<int, int> > m_freepassItems;  // +0x30 ([hell freepass item])
    std::vector<int> m_itemConditions;            // +0x3c ([item condition])
    std::vector<std::pair<int, int> > m_minimapPoints;  // +0x48 ([minimap points])
    // ORIG 静态图案表（B 0x09501020 / 0x095014e0；GetWorldMapCoordFromTable
    // T 0x088e5ef8 依 400*row+40*col 寻址 aWorldMapPatternTable_、依 0x1c 步长
    // 读 aWorldMapPatternTable2ND_。定义见 InitScriptProducers_5.cpp；成员名含尾下划线）。
    static STWorldmapPattern aWorldMapPatternTable_[3][10];
    static STWorldmapPattern2ND aWorldMapPatternTable2ND_[20];
};

// ORIG _Z25GetWorldMapCoordFromTableiiR17STWorldmapPattern 0x088e5ef8 T：
// (a>0 && b>0) 时 out = aWorldMapPatternTable_E[a-1][b-1]，否则不变。
void GetWorldMapCoordFromTable(int a, int b, STWorldmapPattern& out);

// ORIG _Z28GetWorldMapCoordFromTable2NDiR20STWorldmapPattern2ND 0x088e5f44 T：
// (a>=0) 时 out = aWorldMapPatternTable2ND_E[a]，否则不变。
void GetWorldMapCoordFromTable2ND(int a, STWorldmapPattern2ND& out);

// ORIG _Z20importWorldMapScriptP14WorldMapScriptPKc 0x08ab0388 T。
bool importWorldMapScript(WorldMapScript* script, const char* path);

#endif  // GAME_WORLDMAPSCRIPT_H_
