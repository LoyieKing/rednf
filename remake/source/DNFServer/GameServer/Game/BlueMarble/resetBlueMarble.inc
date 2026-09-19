int BlueMarble::resetBlueMarble()
{
    m_diceNumber = 0;
    m_blueMarbleMap = 0;
    m_set.clear();
    m_maxBoardSize = 0;
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}