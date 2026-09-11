void CUser::add_guild_point_item()
{
    int threshold = G_CDataManager()->m_guildParameter.m_field4;   // ORIG 0x865ccd9 槽 -0xc
    // ORIG 0x865cce8 cmp -0xc(%ebp),%eax; setl;test;je skip：call 结果寄存器直参与比较
    // （不物化 rand 局部，probe21 f 实证复现；rand 入槽反产 spill+reload 三行）
    if (get_rand_int(0x2710) < threshold)
        add_inventory_item(0xc84);
    return;   // ORIG 0x865cd05 body 尾 jmp merge + 0x865cd07 nop 岛（技巧 7 尾置 return）
}