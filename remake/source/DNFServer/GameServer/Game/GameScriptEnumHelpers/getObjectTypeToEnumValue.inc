// ---------------------------------------------------------------------------
// getObjectTypeToEnumValue — ORIG @ 0x89bbd99，默认 -1。
// （33=passive 对象、17=active 对象、529=monster、273=character，ORIG 立即数）
// ---------------------------------------------------------------------------
int getObjectTypeToEnumValue(std::string& s)
{
    if (s == "passive")   return 33;
    if (s == "active")    return 17;
    if (s == "monster")   return 529;
    if (s == "character") return 273;
    return -1;
}