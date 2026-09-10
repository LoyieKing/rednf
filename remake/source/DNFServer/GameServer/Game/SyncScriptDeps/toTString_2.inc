void toTString(const wchar_t* pszSrc, std::string& sDst)
{
    int curIndex = gTCharBufferIndex;
    size_t nRet = wcstombs(gTCharBuffer[curIndex], pszSrc, 0x1000);
    gTCharBuffer[curIndex][nRet] = 0;
    sDst = gTCharBuffer[curIndex];
    gTCharBufferIndex = (gTCharBufferIndex + 1) % 10;
