bool CEnvironment::load()
{
    char path[0x22];
    FILE* fp;
    char line[0x400];

    memset(path, 0, 0x22);
    sprintf(path, "./cfg/%s.cfg", get_file_name());
    fp = fopen(path, "rb");
    if (!fp)
        return 0;
    while (true)
    {
        bool ret = fgetline(fp, line);
        if (!ret)
            break;
        if (!check_comment(line))
        {
            if (!parse(line))
                return 0;
        }
        else
            continue;
    }
    fclose(fp);
    return 1;
}