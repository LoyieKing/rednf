// ORIG CharacterEtcStats::clear W 0x08367752（objdump 转写）：vector clear +
// +0xc..+0x18 四个 int 清零；+0x4..+0xb 未触。
void CharacterEtcStats::clear()
{
    m_category.clear();
    for (int i = 0; i < 4; ++i)
        m_moduleDamageRate[i] = 0.0f;
}