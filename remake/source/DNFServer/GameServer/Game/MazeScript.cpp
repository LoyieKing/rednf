// ============================================================================
// df_game_r MazeScript::getGridR(int, int) const 还原（G2-5 批次）。
// ORIG 0822adc2：index = y * m_width + x，返回 m_grids[index]（const 引用）。
// 类布局（0x74）与 CDungeon.h 权威声明一致（§7 单声明点；本 TU 不再本地重复
// GridScript/MazeScript 等类型，统一 include CDungeon.h）。
// ============================================================================

#include <utility>
#include <vector>

#include "CDungeon.h"

const GridScript& MazeScript::getGridR(int x, int y) const
{
    return m_grids[m_width * y + x];
}
