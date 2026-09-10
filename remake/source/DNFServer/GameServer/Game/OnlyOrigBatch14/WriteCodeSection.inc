void WriteCodeSection(const char* path)
{
    // [b28d-solo-OnlyOrigBatch14] 槽序/零初始化按 ORIG：e_lfanew 无初始化（ORIG
    // 0x0808aa41 只清零 -0x18/-0x1c/-0x20 三槽），其余三变量声明序 = 槽浅→深
    // = sizeOfCode(+0x2c)/sizeOfHeaders(+0x1c)/baseOfCode(+0x34)。
    int e_lfanew;
    int sizeOfCode = 0;
    // [b28d-solo-OnlyOrigBatch14] sizeOfHeaders 声明 unsigned：fwrite 第 2 实参为
    // size_t，int 源会多一条 `mov mem,%eax; mov %eax,%edx` 转换中转（ORIG
    // 0x0808abec 单段 `mov -0x1c(%ebp),%edx`；P265 unsigned 恒单段）。
    unsigned int sizeOfHeaders = 0;
    int baseOfCode = 0;
    FILE* f = fopen(path, "rb");
    if (f != 0)
    {
        fseek(f, 0x3c, 0);
        fread(&e_lfanew, 4, 1, f);
        fseek(f, e_lfanew + 0x2c, 0);
        fread(&sizeOfCode, 4, 1, f);
        fseek(f, e_lfanew + 0x34, 0);
        fread(&baseOfCode, 4, 1, f);
        fseek(f, e_lfanew + 0x1c, 0);
        fread(&sizeOfHeaders, 4, 1, f);
        fclose(f);
    }
    char buf[0x100];
    strncpy(buf, path, 0x100);
    // [b28d-solo-OnlyOrigBatch14] P303：ORIG 0x0808ab98 段是 strcat 内建展开
    // （strlen + lea (%ebx,%eax,1) + mov (%esp),%eax 双中转 + memcpy），必须
    // `__builtin_strcat`；手写 memcpy(dst+strlen(dst),...) 恒 DIFF。
    __builtin_strcat(buf, ".code.bin");
    FILE* out = fopen(buf, "wb");
    if (out != 0)
    {
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x0808abec：size=sizeOfHeaders(0x4(%esp))、
        // nmemb=1(0x8(%esp))，即 `fwrite(ptr, size, 1, fp)`（与我们的 1/size 反序）。
        fwrite((const void*)(sizeOfCode + baseOfCode), sizeOfHeaders, 1, out);
        fclose(out);
    }
}
