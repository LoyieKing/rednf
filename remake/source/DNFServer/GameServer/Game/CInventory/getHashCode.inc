void CInventory::getHashCode(unsigned char* out)
{
    // b28d-solo-CInventory：41/41，6 处 lea -0xe0↔-0xe8 全帧槽基差 ⇒ N95 封顶（4.4 帧顶恒留 8B gap）。
    // b28d-37 [N95 封顶，ORIG 0x084fa91c]：fresh 复验 orig 41 / our 41（NDIFF=6，
    // 全为 6 处 `lea -0xe0(%ebp)` vs `lea -0xe8(%ebp)`）。md5_context 大局部
    // （0xdc 字节）4.1 可顶到 ebp（outgoing 区含 8B gap），4.4-O0 局部槽基恒从
    // ebp-8 起 ⇒ 全帧平移 8（范式库 N95 即以本函数为实证；P190 double framePad
    // 反致下移、int pad/声明序调整均不可上移）。清零/更新/收尾调用序与实参宽度
    // 与 ORIG 逐条一致，语义闭合。
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, (unsigned char*)&m_invenItem[0], 0x53e);
    md5_update(&ctx, (unsigned char*)m_pEquipSlot, 0x4a58);
    md5_update(&ctx, (unsigned char*)m_pAvatarSlot, 0x1905);
    md5_update(&ctx, (unsigned char*)m_pCreatureSlot, 0x39aa);
    md5_finish(&ctx, out);
