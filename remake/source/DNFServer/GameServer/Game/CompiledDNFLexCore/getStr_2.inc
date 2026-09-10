char* CompiledDNFLex::getStr(bool* result)
{
    bool rslt = getStr(stringBuffer_, STRING_BUFFER_SIZE);
    if (result != 0)
        *result = rslt;
    return stringBuffer_;
