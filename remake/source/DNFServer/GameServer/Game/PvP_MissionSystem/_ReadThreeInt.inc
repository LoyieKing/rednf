// ---- ORIG _Z13_ReadThreeIntP13MissionScript @0x08a5ab91 ----
// 条件参数三连读：+0x78 / +0x7c / +0x80。
void _ReadThreeInt(MissionScript* script)
{
    script->m_kind = ScanInt((bool*)0);
    script->m_field7c = ScanInt((bool*)0);
    script->m_field80 = ScanInt((bool*)0);
}