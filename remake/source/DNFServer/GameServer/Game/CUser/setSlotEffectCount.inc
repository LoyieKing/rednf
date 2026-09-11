// ORIG 0x08230880 (W)：写 +0x8d244（m_ceraUserInfo.m_field60）
void CUser::setSlotEffectCount(unsigned char count)
{
    m_ceraUserInfo.m_field60 = count;   // ORIG 0x865c… 0x8230893 mov %dl 直写无归一 ⇒ 字段 uchar（Main 已回改），同型 this-先直落
}