void toTString(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = gTCharBufferIndex;
    size_t nRet = wcstombs(gTCharBuffer[curIndex], sSrc.c_str(), 0x1000);
    gTCharBuffer[curIndex][nRet] = 0;
    sDst = gTCharBuffer[curIndex];
    gTCharBufferIndex = (gTCharBufferIndex + 1) % 10;
