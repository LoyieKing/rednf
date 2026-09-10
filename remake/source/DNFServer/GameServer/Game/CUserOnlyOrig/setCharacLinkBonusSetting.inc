void CUser::setCharacLinkBonusSetting(int slot, bool flag)
{
    // ORIG 0x084ecdc8 call get_charac_count；0x084ecdd0 `jle mov$1岛`、0x084ecdd6 `jns mov$0岛`
    // = 短路 `count <= slot || slot < 0` 物化 bool；0x084ecde6 `test %al,%al; jne END`
    // = 技巧24 goto 落点岛（END=nop;leave，body 末 call 后 `jmp`）。
    if ((get_charac_count() <= slot) || (slot < 0))
        goto SKIP_SET;
    m_characList[slot].m_linkBonusSetting = flag;
    m_characterView.enableSaveCharacView();
SKIP_SET:;
