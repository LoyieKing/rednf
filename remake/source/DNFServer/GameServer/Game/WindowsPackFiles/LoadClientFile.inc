// LoadClientFile —— _Z14LoadClientFilePmPPvPKc T @0x08572f3a（0xf2）
// fopen("rb") → fseek END 取长 → new → fread(1) 成功即 1。
// ---------------------------------------------------------------------------
// [N 类标注] 0x08572f3a：66/65（差 1），P167 调用点 9/9 双向差集 0 ⇒ 纯指令形差：
//  ① ORIG fread 返回值为 `cmp $0x1,%eax; setne %al; test %al,%al; je`（比较结果
//     经 setne 物化后再分支，且结果不落槽），我方按具名 n 变量存槽后 `cmpl;je`
//     （范式库 setcc-fold 负例的镜像变体：4.4 恒把该形折回存槽+整数比较）；
//  ② ORIG new[] 返回值以 %edx 直载跨两句使用（无缓存槽），4.4 走 -0x10 槽存取；
//  ③ fopen==0 的失败出口 ORIG 为 `je 远端 +0xeb` 单跳，4.4 生成 `jne 跳过 +
//     mov $0 + jmp` 三句岛。三者皆编译器版本 RA/分支物化策略，源码侧不可控。
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
}