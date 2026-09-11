void* CUser::GetSecretShopData()
{
    CParty* party = static_cast<CParty*>(GetParty());
    if (party != 0)
        return party->GetSecretShopData();
    return 0;
}