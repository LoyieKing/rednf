// make_dir(char*) —— ORIG _Z8make_dirPc 0x829ee83：opendir 探测，不存在则 mkdir 0777。
void make_dir(char* path)
{
    // ORIG 0x829ee94：opendir 结果存 -0xc 槽（具名局部），cmpl/jne 直判。
    int ret = (int)opendir(path);
    if (ret == 0)
    {
        mkdir(path, 0777);
    }
}