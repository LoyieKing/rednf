// ---------------------------------------------------------------------------
// getGradeToEnumValue — ORIG @ 0x89bcd5b，4 个分支，默认 4。
// ---------------------------------------------------------------------------
int getGradeToEnumValue(std::string& s)
{
    if (s == "common")   return 0;
    if (s == "champion") return 1;
    if (s == "boss")     return 2;
    if (s == "named")    return 3;
    return 4;
}