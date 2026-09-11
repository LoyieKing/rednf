void toUnicode(const std::string& src, std::wstring& dst)  // ORIG 0x08adf25c
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src.c_str(), 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    dst = gUnicodeBuffer[curindex];
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
}