void toUnicode(const char* src, std::wstring& dst)     // ORIG 0x08adf32f
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src, 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    dst = gUnicodeBuffer[curindex];
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
}