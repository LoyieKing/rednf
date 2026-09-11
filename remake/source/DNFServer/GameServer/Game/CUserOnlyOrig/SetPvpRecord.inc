// ---------------------------------------------------------------------------
// 5. CUser::SetPvpRecord @0x0864deea —— 返回恒 1(undefined4 mov $1)
// ---------------------------------------------------------------------------
bool CUser::SetPvpRecord(PvpResultType& record)
{
    getPVPResultW()->Set(record);
    return 1;
}