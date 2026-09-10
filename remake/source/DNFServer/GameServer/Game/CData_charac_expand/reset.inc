void CData::reset()
{
    // [b28c-11] NEAR 封顶（ORIG 0x832aebc）：同 saveData（N7 虚调用寄存器互换）。
    m_bIsSave = 0;
    _reset();
