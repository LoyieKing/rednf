// ORIG getType @0x081572ea：返回 key 高 32 位的高 16 位（= key>>48，符号扩展位型扩展）。
long long CAdvanceAltarLog::getType(long long key)
{
    return key >> 0x30;
}