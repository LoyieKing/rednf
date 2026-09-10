void CAlchemist::UseDelilah(CUser* user)
{
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        party->m_battleField.SetAssignNPCByPotion(
            true);
    }
