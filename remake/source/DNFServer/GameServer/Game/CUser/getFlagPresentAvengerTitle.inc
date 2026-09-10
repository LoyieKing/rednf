bool CUser::getFlagPresentAvengerTitle() const  // 0x08697578：movzbl 0x8d246(%eax),%eax（P292/P248档①：调用点 xor $1 直判 ⇒ bool）
{
    return m_ceraUserInfo.m_field62;
