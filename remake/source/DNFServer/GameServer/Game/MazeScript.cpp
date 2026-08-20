// ============================================================================
// df_game_r MazeScript::getGridR(int, int) const 还原（G2-5 批次）。
// ORIG 0822adc2：index = y * m_width + x，返回 m_grids[index]（const 引用）。
// 类布局（0x74）与 CDungeon.h 一致；ctor/copy/dtor/clear/getGrid/operator=
// 已由 CDungeon.cpp 实现，本 TU 只定义 getGridR。
// ============================================================================

#include <utility>
#include <vector>

#include "DungeonClearCondition.h"

struct GridScript
{
    int m_gridValue;
    std::vector<int> m_mapCandidates;
    std::vector<int> m_secondCandidates;
    std::vector<int> m_layeredMapIndexes;
};

struct RidableObject
{
    char m_pad[4];
};

class RidableScript
{
public:
    int m_0;
    char m_4;
    std::vector<RidableObject> m_objects;
};

// MazeScript（0x74 字节）
class MazeScript
{
public:
    const GridScript& getGridR(int x, int y) const;

    int m_width;
    int m_height;
    std::vector<GridScript> m_grids;
    std::vector<std::pair<int, int> > m_vec14;
    std::vector<std::pair<int, int> > m_vec20;
    int m_2c;
    int m_30;
    int m_34;
    RidableScript m_ridable;
    int m_4c;
    int m_50;
    int m_54;
    std::vector<DungeonClearCondition> m_clearCondition;
    int m_64;
    int m_68;
    int m_6c;
    int m_70;
};

const GridScript& MazeScript::getGridR(int x, int y) const
{
    return m_grids[m_width * y + x];
}
