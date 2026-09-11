bool CEnvironment::check_comment(char* line)
{
    int i;
    int cnt;
    int j;

    if (*line == '\0')
        return 1;
    if (*line == '[' || *line == '\r' || *line == '\n')
        return 1;
    i = 0;
    while (i <= 0x3fd)
    {
        cnt = 0;
        j = 0;
        while (j <= 1)
        {
            if (line[i + j] == "//"[j])
                cnt = cnt + 1;
            j = j + 1;
        }
        if (cnt == 2)
            return 1;
        i = i + 1;
    }
    return 0;
}