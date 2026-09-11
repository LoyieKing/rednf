void CDimensionActivationEvent::set_dungeon_play_count()
{
    // ORIG 0x080eebc0：读 m_dimensionAct.m_field0（日计数）→ trace；
    // >0 时对难度 0..5 逐个 set_dimensionInout(i, (char)dayCount)。
    int dayCount = G_CDataManager()->m_dimensionAct.m_field0;
    cMyTrace tr(__PRETTY_FUNCTION__, 0x27, 0);
    tr("CDimensionActivationEvent day_play_count : %d", dayCount);
    // [b28c-16] P223：ORIG 尾 `jle +0x90(nop) / then 体尾 jmp +0x91 / +0x90: nop`
    // 空 else 占位岛 ⇒ 复现形=否定 goto + 体尾空标签（4.4 对 if{}else{body}
    // 恒 jump-thread 少 jmp+nop）。
    if (dayCount <= 0)
        goto SKIP;
    for (int i = 0; i < 6; ++i)
        G_CDataManager()->set_dimensionInout(i, (char)dayCount);
SKIP:
    ;
}