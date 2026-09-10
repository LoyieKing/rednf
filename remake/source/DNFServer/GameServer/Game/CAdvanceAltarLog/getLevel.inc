long long CAdvanceAltarLog::getLevel(long long key)
{
    return (key << 0x10) >> 0x30;
