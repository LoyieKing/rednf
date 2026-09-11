// ORIG 0x08adef63：toMbcs(wstring const&, string&)：wcstombs 后 sDst = buf。
// [b28d-L8] AE | 同上（TLS 组布局对齐，ORIG 0x08adef63）
void toMbcs(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = gMbcsBufferIndex;
    size_t nRet = wcstombs(gMbcsBuffer[curIndex], sSrc.c_str(), 0x1000);
    gMbcsBuffer[curIndex][nRet] = 0;
    sDst = gMbcsBuffer[curIndex];
    gMbcsBufferIndex = (gMbcsBufferIndex + 1) % 10;
}