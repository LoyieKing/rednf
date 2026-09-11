// ORIG 0x08adec40：toTString(wstring const&, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adec40）
void toTString(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = gTCharBufferIndex;
    size_t nRet = wcstombs(gTCharBuffer[curIndex], sSrc.c_str(), 0x1000);
    gTCharBuffer[curIndex][nRet] = 0;
    sDst = gTCharBuffer[curIndex];
    gTCharBufferIndex = (gTCharBufferIndex + 1) % 10;
}