int Stream::str_len(char* s)
{
    int len = 0;
    int i = 0;
    while (i <= 9999999)
    {
        if (s[i] == 0)
        {
            return len;
        }
        ++len;
        ++i;
    }
    return 0;
