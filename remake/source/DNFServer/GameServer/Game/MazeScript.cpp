// ============================================================================
// df_game_r MazeScript::getGridR(int, int) const 还原（G2-5 批次）。
// ORIG 0822adc2：index = y * m_width + x，返回 m_grids[index]（const 引用）。
// 类布局（0x74）与 CDungeon.h:77 一致；ctor/copy/dtor/clear/getGrid/operator=
// 已由 CDungeon.cpp 实现，本 TU 只定义 getGridR。
// ============================================================================

#include <utility>
#include <vector>

struct GridScript
{
    int m_gridValue;                        // +0x00
    std::vector<int> m_mapCandidates;       // +0x04
    std::vector<int> m_secondCandidates;    // +0x10
    std::vector<int> m_layeredMapIndexes;   // +0x1c
};

struct RidableObject
{
    char m_pad[4];
};

class RidableScript
{
public:
    int m_0;                                // +0x00
    char m_4;                               // +0x04
    std::vector<RidableObject> m_objects;   // +0x08
};

struct DungeonClearCondition
{
    int m_0;   // +0x00
    int m_4;   // +0x04
    int m_8;   // +0x08
};

// MazeScript（0x74 字节）
class MazeScript
{
public:
    const GridScript& getGridR(int x, int y) const;

    int m_width;                              // +0x00
    int m_height;                             // +0x04
    std::vector<GridScript> m_grids;          // +0x08
    std::vector<std::pair<int, int> > m_vec14;  // +0x14
    std::vector<std::pair<int, int> > m_vec20;  // +0x20
    int m_2c;                                 // +0x2c
    int m_30;                                 // +0x30
    int m_34;                                 // +0x34
    RidableScript m_ridable;                  // +0x38
    int m_4c;                                 // +0x4c
    int m_50;                                 // +0x50
    int m_54;                                 // +0x54
    std::vector<DungeonClearCondition> m_clearCondition; // +0x58
    int m_64;                                 // +0x64
    int m_68;                                 // +0x68
    int m_6c;                                 // +0x6c
    int m_70;                                 // +0x70
};

const GridScript& MazeScript::getGridR(int x, int y) const
{
    return m_grids[m_width * y + x];
}
