bool CEnvironment::fgetline(FILE* fp, char* buf)
{
    int i;
    int c;

    memset(buf, 0, 0x400);
    i = 0;
    while (i <= 0x3ff)
    {
        c = fgetc(fp);
        if (feof(fp))
        {
            buf[i] = '\0';
            return i > 0;
        }
        buf[i] = (char)c;
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            if (i > 0 && buf[i - 1] == '\r')
            {
                buf[i - 1] = '\0';
                return 1;
            }
            return 1;
        }
        i = i + 1;
    }
    return 1;
}