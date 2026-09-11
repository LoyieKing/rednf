// ORIG 0x08307e44：以当前位置为基准的单向方向判定。
int CBattle_Field::get_invade_direction(int x, int y)
{
    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    int direction = 0;
    if (curX == x)
    {
        if (curY > y)
            direction = 8;
        if (curY < y)
            direction = 2;
    }
    if (curY == y)
    {
        if (curX < x)
            direction = 4;
        if (curX > x)
            direction = 1;
    }
    return direction;
}