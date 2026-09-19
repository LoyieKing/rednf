// b28c-1 AE: P223 尾标签后 0 差。
int BlueMarble::setMaxBoardSize()
{
    BlueMarbleMapScript* map = m_pScript->getMap(getBlueMarbleMap());
    // ORIG 0x080d6e88：以 getMap() 返回值为 this 直调 vector::size（m_tiles 在
    // BlueMarbleMapScript +0x00，地址等价）⇒ 具名 map->m_tiles.size()。
    // b28c-1：P223 空块 nop 中继岛（ORIG je 跳过 body 落 nop、body 尾 jmp 汇合）
    //   ⇒ goto 尾空标签形，等价语义。
    if (map == 0) goto done;
    m_maxBoardSize = (int)map->m_tiles.size();
done:;
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}