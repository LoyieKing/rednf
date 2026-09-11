// ---------------------------------------------------------------------------
// getElementToEnumValue — ORIG @ 0x89bca42，默认 -1。
// ---------------------------------------------------------------------------
int getElementToEnumValue(std::string& s)
{
    // ORIG 0x089bca42：比较序为 water→fire→dark→light→none→all（Ghidra C 实证）
    if (s == "water") return 1;
    if (s == "fire")  return 0;
    if (s == "dark")  return 2;
    if (s == "light") return 3;
    if (s == "none")  return 4;
    if (s == "all")   return 4;
    return -1;
}