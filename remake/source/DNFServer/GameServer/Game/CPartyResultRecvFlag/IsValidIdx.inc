bool CPartyResultRecvFlag::IsValidIdx(int idx) const
{
    if (idx >= 0 && idx <= 3)
    {
        return true;
    }
    else
    {
        return false;
    }
