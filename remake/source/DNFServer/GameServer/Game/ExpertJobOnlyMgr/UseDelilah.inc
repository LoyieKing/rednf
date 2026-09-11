// ---- UseDelilah（报告 UseDelilah.md，ORIG T 0x085d18ae sz 0x33）----
void CAlchemist::UseDelilah(CUser* user)
{
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        party->m_battleField.SetAssignNPCByPotion(
            true);
    }
}