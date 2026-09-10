void toMbcs(const wchar_t* pszSrc, std::string& sDst)
{
    int curIndex = gMbcsBufferIndex;
    size_t nRet = wcstombs(gMbcsBuffer[curIndex], pszSrc, 0x1000);
    gMbcsBuffer[curIndex][nRet] = 0;
    sDst = gMbcsBuffer[curIndex];
    gMbcsBufferIndex = (gMbcsBufferIndex + 1) % 10;
