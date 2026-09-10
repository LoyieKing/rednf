bool Script::fgetln(FILE* fp, char* buf)
{
    int i = 0;
    while (i <= 0x3ff)
    {
        int code = fgetc(fp);
        if (feof(fp) != 0)
        {
            buf[i] = '\0';
            return false;
        }
        buf[i] = (char)code;
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            if ((i > 0) && (buf[i - 1] == '\r'))
            {
                buf[i - 1] = '\0';
                return true;
            }
            return true;
        }
        i = i + 1;
    }
    return true;
