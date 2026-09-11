bool PvpResultRecvFlag::IsValidIdx(int idx) const
{
    if (idx >= 0 && idx <= 7)
    {
        return true;
    }
    else
    {
        return false;
    }
}