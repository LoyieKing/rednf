// ORIG _ZN13user_creature13CCreatureItem6GetUidEv W 0x0833efbe（S23B：
// SetStomach 0x08339ee0+0x69 实证调用点为 call，内联形违 ORIG，同 GetItemId 案）。
int CCreatureItem::GetUid()
{
    return m_uid;
}