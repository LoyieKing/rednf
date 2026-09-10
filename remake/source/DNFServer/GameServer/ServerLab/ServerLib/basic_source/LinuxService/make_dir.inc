void make_dir(char* szdir_name)
{
    puts("In make_dir");
    DIR* p_dir = opendir(szdir_name);
    if (p_dir == NULL)
    {
        mkdir(szdir_name, 0x1ff);
    }
    puts("Out make_dir");
