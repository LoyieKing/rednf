bool isInPathArea(int x, int y, const int (*area1)[2], const int (*area2)[2])
{
    MapArea areas[4];
    makePathArea(areas, area1, area2);
    for (int i = 0; i < 4; ++i)
    {
        if (isInArea(x, y, &areas[i]))
            return 1;
    }
    return 0;
