// ---------------------------------------------------------------------------
// ORIG W 0x0844d54c / 0x0844d55a（objdump：mov 0x134/0x138(%eax); ret）。
// ORIG W 0x082652d8（objdump：+0x38 自增返回新值）。
int Sanicova::CPad::incFailCnt()
{
    m_failCnt38 += 1;
    return m_failCnt38;
}