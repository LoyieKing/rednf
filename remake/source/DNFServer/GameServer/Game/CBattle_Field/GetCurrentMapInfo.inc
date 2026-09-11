MapInfo* CBattle_Field::GetCurrentMapInfo()
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    return GetMapInfoFromPos(x, y, false);
}