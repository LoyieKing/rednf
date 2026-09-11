// ORIG T 0x089aee71："[expUp]" → 1，否则 0。
int getStackableIncreaseStatusType(std::string& s)
{
    if (s == "[expUp]")
        return 1;
    return 0;
}