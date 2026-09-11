// ORIG 0x08230870 (W)：movzbl +0x8d244（m_ceraUserInfo.m_field60）
unsigned char CUser::getSlotEffectCount() const
{
    return static_cast<unsigned char>(m_ceraUserInfo.m_field60);
}