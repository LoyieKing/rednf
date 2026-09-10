bool CUser::SetPvpRecord(PvpResultType& record)
{
    getPVPResultW()->Set(record);
    return 1;
