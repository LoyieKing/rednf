// ORIG 0x0830e32e checkLayeredMap(x, y)：迷宫格带层叠表则推进层号并置当前图层标记。
void CBattle_Field::checkLayeredMap(int x, int y)
{
    MazeScript* maze = getMaze();
    if (maze != NULL)
    {
        const GridScript* grid = &maze->getGridR(x, y);
        if (grid != NULL && grid->m_layeredMapIndexes.size() != 0)
        {
            if ((int)grid->m_layeredMapIndexes.size() > getLayeredMapIndex() + 1)
                setLayeredMapIndex();

            getCurrentLayeredMapInfo()->m_layeredMap = 1;    // MapInfo+0xe8
        }
    }
}