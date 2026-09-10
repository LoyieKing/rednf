int CBingo::calNewValue(unsigned int& board, int idx)
{
    int index = getIndexTodayBoard(idx);
    if (index == -1)
    {
        return 0;
    }
    if (index > 24)  // ORIG：cmp $0x18 / jle
    {
        return -1;
    }
    if (!isSetBit_Uint(board, index))
    {
        setBit_Uint(board, true, index);
        return calBoradMatchLine(static_cast<int>(board));
    }
    return 0;
