bool CFirstServerOpenEvent::IsEventing(CUser* /*user*/)
{
    if (!CEventBase::IsEventing((CUser*)0))
        return false;
    return true;
