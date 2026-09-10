void CUser::SetPowerUp(bool flag)
{
    m_powerUp = flag;   // ORIG 直写 mov %dl,0x8d253（P32 bool 字段直赋）
