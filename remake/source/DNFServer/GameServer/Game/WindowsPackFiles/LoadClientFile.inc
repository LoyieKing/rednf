bool LoadClientFile(unsigned long* outSize, void** outBuf, const char* path)
{
    FILE* fp = fopen(path, "rb");
    // [b28d-64] P59/P63：ORIG 0x08572f5a `je <尾部 mov $0; leave; ret>` 单跳，
    // 负形早返被 4.4 内联成 `jne body; mov $0; jmp` 岛 ⇒ 改正向包裹让失败出口归共享尾。
    if (fp != 0)
    {
        fseek(fp, 0, SEEK_END);
        *outSize = static_cast<unsigned long>(ftell(fp));
        fseek(fp, 0, SEEK_SET);
        // [b28d-64] P0/P292：ORIG 0x08572fb5 new[] 结果 %edx 直载（无 buf 槽）；
        // 0x08572fe1 fread 结果寄存器直比 `setne %al; test %al; je`（无具名 n 槽）；
        // 0x08572ff3 delete 为显式 `operator delete`（无 4.4 自带判空 test/je）。
        *outBuf = new unsigned char[*outSize];
        if (fread(*outBuf, *outSize, 1, fp) != 1)
        {
            operator delete(*outBuf);   // ORIG _ZdlPv（new[]/delete 不匹配为 ORIG 原样）
            *outBuf = 0;
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
    }
    return false;
