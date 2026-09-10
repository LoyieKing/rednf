void CRecipeSuccess::execute()
{
    CUser* user = GetUser();
    user->recipeForceProbability(m_result);
