// ============================================================================
// CCreature::SetStomach @ 0x0833f070：调 CStomach::SetStomachValue(this+0x34, (float)stomachValue)，
// ORIG 即 call SetStomachValue(写入 +0x4 m_value)。
// ============================================================================
void user_creature::CCreature::SetStomach(int stomachValue)
{
    char _pad[0x10];  // P31 帧对齐：ORIG sub $0x18，默认帧 sub $0x8，补 16B 达一致
    m_stomach.SetStomachValue((float)stomachValue);
}
