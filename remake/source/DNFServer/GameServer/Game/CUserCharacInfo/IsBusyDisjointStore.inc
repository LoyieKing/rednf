bool CUserCharacInfo::IsBusyDisjointStore()
{
    if (GetCurCharacExpertJob() != 0)
    {
        if (GetCurCharacExpertJob()->GetType() == 3)
            return true;
    }
    return false;
}