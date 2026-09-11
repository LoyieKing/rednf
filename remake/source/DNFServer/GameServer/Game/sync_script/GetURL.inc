const char* CSyncScript::GetURL(const char* str, int len, int idx)
{
    static std::string strUrl;
    strUrl.clear();
    int start = 0;
    int end = 0;
    int i = 0;
    for (i = 0; i < len; ++i)
    {
        if (str[i] == '/')
        {
            start = i + 1;
        }
        if (str[i] == '.')
        {
            end = i;
        }
    }
    strUrl.insert(0, str + start, end - start);
    char tmp[16] = {0};
    sprintf(tmp, "/%05d.png", idx);
    strUrl += tmp;
    return strUrl.c_str();
}