// ORIG execute 0x084b3a34：GetUser()->recipeForceProbability(slot)。
void CRecipeSuccess::execute()
{
    CUser* user = GetUser();
    user->recipeForceProbability(m_result);
}