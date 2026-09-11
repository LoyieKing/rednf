void SIG_SAVE_PC_ROOM_RENT_ITEM::clear()      // ORIG W 0x8694c9a
{
    /* ORIG 帧 0x28/循环变量槽 -0xc：4.1 死 28B pad 槽经 double framePad
     * 撑帧复现（P190/技巧31 系，/tmp/p26_slot.cpp probeC 实证同形）。 */
    double framePad;
    for (int i = 0; i <= 0; ++i)
        m_entry[i].clear();
    (void)&framePad;
}