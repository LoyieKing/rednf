int CCargo::get_empty_slot() const
{
    // P210 定义顺序定律：本函数定义置于 IsOperatorable 之前 ⇒ 4.4 不按其
    // 帧继承 outgoing，保守大帧 0x28/slot@-0xc 与 ORIG 全等（b26 NL30）。
    int slot = 0;
    for (; slot < 0x98; ++slot) {
        if (m_pItems[slot].m_addInfo == 0)
            break;
    }
    if (IsOperatorable(slot))
        return slot;
    return -1;
}