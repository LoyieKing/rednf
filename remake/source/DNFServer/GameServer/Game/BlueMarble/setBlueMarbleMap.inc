int BlueMarble::setBlueMarbleMap()
{
    m_blueMarbleMap = (unsigned int)m_pRands[1].randInt() %
                    (unsigned int)m_pScript->getMapTotalCount() +
                1;
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}