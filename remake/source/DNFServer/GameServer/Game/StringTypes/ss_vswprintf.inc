int ss_vswprintf(wchar_t* buffer, size_t count, const wchar_t* format, char* argList)
{
    // ORIG _Z12ss_vswprintfPwjPKwPc @0x08ad45f3
    int ret = vswprintf(buffer, count, format, argList);
    if (ret < 0 || static_cast<int>(count) <= ret)
    {
        *buffer = L'\0';
        return 0;
    }
    return ret;
}