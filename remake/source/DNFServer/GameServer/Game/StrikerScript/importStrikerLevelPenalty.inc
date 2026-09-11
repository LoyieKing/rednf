// ORIG T 0x08a9e628：两个 ScanInt((bool*)0) 依次写入 +0x64/+0x68。
void StrikerScript::importStrikerLevelPenalty()
{
    m_field64 = ScanInt((bool*)0);
    m_field68 = ScanInt((bool*)0);
}