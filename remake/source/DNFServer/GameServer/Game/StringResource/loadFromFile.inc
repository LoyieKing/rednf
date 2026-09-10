bool StringResource::loadFromFile(const char* path, CharString* error)
{
    FILE* fp = fopen(path, "rb");
    if (fp == NULL)
    {
        if (error)
            *error = "Cannot open file";
        return false;
    }
    fseek(fp, 0, SEEK_END);
    const long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize < 0)
    {
        fclose(fp);
        return false;
    }
    // ORIG 各使用点直接读 ftell 槽（-0x14）：无具名 int 中间量（P183）。
    char* buf = new char[fileSize];
    fread(buf, 1, (size_t)fileSize, fp);
    fclose(fp);

    /* loadFromBinary 实参装载 N80 族封顶（b26 NL26，Main 裁定）：ORIG 4.1 右到左串联装
     * 载全程 %eax；4.4 恒 arg0=eax、arg2/arg1=edx 交错。具名中间量/逗号重
     * 排/size_t 内置 5 探针全败（/tmp/p26_lf.cpp、p26_lf2.cpp），语义完成。 */
    const bool result = loadFromBinary(
        reinterpret_cast<const unsigned char*>(buf), fileSize, error);
    delete[] buf;
    return result;
