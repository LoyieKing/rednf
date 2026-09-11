RestrictInputtingNameData::RestrictInputtingNameData()
{
    static const char* scriptPath =
        "etc\\RestrictNameUnicodeRangeTable.etc";  // .rodata 0x08e2e68c 实测
    if (!isLoaded_)                                // ORIG movzbl+xor$1/test/je
    {
        isLoaded_ = this->importUnicodeRangeData(
            scriptPath, &unicodeRangeDataList_);
    }
}