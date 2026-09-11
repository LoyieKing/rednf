// ORIG T 0x0834cb7c。
// b28d-solo-W7_UserOnlyC 判词：P197④ size()==0 复现 sete；两循环具名 pos 消双
// operator[]；第三段 find 内联 pair<int&,int&> + P223 三岛 goto 尾标签 ⇒ 345=345，
// TU 层 AE。
bool CDungeon::VerifyMaze(const MazeScript& maze) const
{
    if (maze.m_width == 1 && maze.m_height == 1)
        return true;

    // P197④：ORIG `call size; test; sete; test; je` ⇒ 写 size()==0。
    if (maze.m_vec14.size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x83e,
            "dungeon index(%d) maze(%d) has error. (Start positions were not declared!)",
            m_index, maze.m_64);
        return false;
    }
    for (unsigned int i = 0; i < maze.m_vec14.size(); ++i)
    {
        // ORIG 一次 operator[] 后存 -0x54/-0x50 两槽（具名 pos，非两次下标）。
        const std::pair<int, int> pos = maze.m_vec14[i];
        const GridScript& grid = maze.getGridR(pos.first, pos.second);
        if (m_maplist[grid.m_gridValue].size() == 0)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x847,
                "dungeon index(%d) maze(%d) has error. (Can't found start map(grid: %d,%d)!)",
                m_index, maze.m_64, pos.first, pos.second);
        }
    }

    if (maze.m_vec20.size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x84e,
            "dungeon index(%d) maze(%d) has error. (End positions were not declared!)",
            m_index, maze.m_64);
        return false;
    }
    for (unsigned int i = 0; i < maze.m_vec20.size(); ++i)
    {
        const std::pair<int, int> pos = maze.m_vec20[i];
        const GridScript& grid = maze.getGridR(pos.first, pos.second);
        if (m_dummyMap324[grid.m_gridValue].size() == 0)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x857,
                "dungeon index(%d) maze(%d) has error. (Can't found boss map(grid: %d,%d)!",
                m_index, maze.m_64, pos.first, pos.second);
        }
    }

    for (int y = 0; y < maze.m_width; ++y)
    {
        for (int x = 0; x < maze.m_height; ++x)
        {
            const GridScript& grid = maze.getGridR(y, x);
            // ORIG `test;je d01d` / `ne;test;jne d020` / `ne;test;jne d023` 三岛
            // （P223 多岛：正条件 goto + 尾标签串，标签序=守卫序）。
            if (grid.m_gridValue == 0)
                goto L_grid;
            if (std::find(maze.m_vec14.begin(), maze.m_vec14.end(),
                          std::pair<int, int>(y, x)) != maze.m_vec14.end())
                goto L_start;
            if (std::find(maze.m_vec20.begin(), maze.m_vec20.end(),
                          std::pair<int, int>(y, x)) != maze.m_vec20.end())
                goto L_end;
            if (m_map1a4[grid.m_gridValue].size() == 0)
            {
                // ORIG 0x0834cfdf/0x0834cfe3：vararg3=y、vararg4=x（照抄实参序）。
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__,
                    0x86b,
                    "dungeon index(%d) maze(%d) has error. (Can't found grid map(grid: %d,%d)!)",
                    m_index, maze.m_64, y, x);
            }
        L_end:;
        L_start:;
        L_grid:;
        }
    }
    return true;
}