// ORIG T 0x085b8fa6（IsThereGhostInParty.md）。
bool CParty::IsThereGhostInParty()
{
    for (int i = 0; i <= 3; i++)
    {
        // [拆&&] ORIG 双段直分支，&& 合一 4.4 产物化岛 5 行。
        if (_checkValidUser(i))
        {
            if (reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                    ->IsCurCharacGhost())
            {
                return true;
            }
        }
    }
    return false;
}