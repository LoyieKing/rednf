void toMbcs(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = gMbcsBufferIndex;
    size_t nRet = wcstombs(gMbcsBuffer[curIndex], sSrc.c_str(), 0x1000);
    gMbcsBuffer[curIndex][nRet] = 0;
    sDst = gMbcsBuffer[curIndex];
    gMbcsBufferIndex = (gMbcsBufferIndex + 1) % 10;
