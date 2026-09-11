// ---- ORIG _Z12_ReadFourIntP13MissionScript @0x08a5abd2 ----
// [within] 条件四连读：+0x78 / +0x7c / +0x80 / +0x84。
void _ReadFourInt(MissionScript* script)
{
    script->m_kind = ScanInt((bool*)0);
    script->m_field7c = ScanInt((bool*)0);
    script->m_field80 = ScanInt((bool*)0);
    script->m_field84 = ScanInt((bool*)0);
}