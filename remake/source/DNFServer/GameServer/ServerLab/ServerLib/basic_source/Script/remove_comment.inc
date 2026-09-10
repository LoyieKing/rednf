bool Script::remove_comment(char* line)
{
    int i = 0;
    while (i <= 0x3fd)
    {
        int n = 0;
        for (int j = 0; j < 2; j = j + 1)
        {
            if (line[i + j] == "//"[j])
            {
                n = n + 1;
            }
        }
        if (n == 2)
        {
            memset(line + i, 0, 0x400 - i);
            return true;
        }
        i = i + 1;
    }
    return false;
