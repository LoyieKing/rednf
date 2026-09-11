// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x082e5a0b）：去 MapArea& 引用缓存
// （P157/P183 逐点 areas->at(i)）；tx/ty/dist 提函数头（P212 全头部声明）+
// GetDistance 实参序换为 (Point2d(x,y), Point2d(tx,ty)) + 比较写 minDist>dist
// 复现 4.1 折痕；MapArea::include 返回 int→bool（P292 调用点 test %al）。
// p167 CLEAR。
bool IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas, int x, int y,
                              int& outX, int& outY, int margin)
{
    int minDist = 0x7fffffff;
    int dist = 0;
    int tx = 0;
    int ty = 0;
    outX = x;
    outY = y;
    for (unsigned int i = 0; i < areas->size(); ++i)
    {
        // ORIG 0x82e5a0b 用 at()（越界抛异常路径与 ORIG 一致），不缓存引用。
        if (areas->at(i).include(x, y, tx, ty, margin))
            return 1;
        dist = GetDistance(Point2d(x, y), Point2d(tx, ty));
        if (minDist > dist)
        {
            if (margin != 0)
            {
                outX = tx;
                outY = ty;
            }
            else
            {
                outX = tx - x;
                outY = ty - y;
            }
            minDist = dist;
        }
    }
    return 0;
}