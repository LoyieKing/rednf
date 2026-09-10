void user_creature::CCreature::SetStomach(int stomachValue)
{
    char _pad[0x10];  // P31 帧对齐：ORIG sub $0x18，默认帧 sub $0x8，补 16B 达一致
    m_stomach.SetStomachValue((float)stomachValue);
}
