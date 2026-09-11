// ---------------------------------------------------------------------------
// Secu_HackLogCheck::Init —— ORIG T 0x0827a7d0：写基类 +0x34 = user，
// 再调基类 SetUserPtr(user)。Secu_MonsterCheck::SetUserPtr —— ORIG T
// 0x0827abec：m_user = user。
// ---------------------------------------------------------------------------
void Secu_HackLogCheck::Init(CUser* user)
{
    m_user34 = user;
    SetUserPtr(user);
}