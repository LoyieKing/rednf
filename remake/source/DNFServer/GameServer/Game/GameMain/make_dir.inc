void make_dir(char* path)
{
    // ORIG 0x829ee94：opendir 结果存 -0xc 槽（具名局部），cmpl/jne 直判。
    int ret = (int)opendir(path);
    if (ret == 0)
    {
        mkdir(path, 0777);
    }
