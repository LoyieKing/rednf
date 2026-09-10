Inven_Item* CCargo::GetCargoRef(int idx) const
{
    // ORIG 0x850b38c `sub $0x18`：dead pad 撑帧（技巧31）。
    char framePad[0x10];
    (void)framePad;
    if (!IsOperatorable(idx))
        return 0;
    return &m_pItems[idx];
