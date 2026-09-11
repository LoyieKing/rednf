void SIG_SAVE_DIMENSION_RENT_ITEM::clear()    // ORIG W 0x8694cd2
{
    /* ORIG 帧 0x28/循环变量槽 -0xc：4.1 死 28B pad 槽经 double framePad
     * 撑帧复现（P190/技巧31 系，NL26 probeC 同族实证）。 */
    double framePad;
    for (int i = 0; i <= 10; ++i)
        m_entry[i].clear();
    (void)&framePad;
}