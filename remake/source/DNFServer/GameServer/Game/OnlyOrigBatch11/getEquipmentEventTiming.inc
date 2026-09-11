// ORIG T 0x089c2a81："jump attack"→0、否则 1。
int getEquipmentEventTiming(const std::string& s)
{
    if (s == "jump attack")
        return 0;
    return 1;
}