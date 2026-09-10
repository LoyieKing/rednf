int ImportDifficultPenaltyInfoScript(const char* path, DifficultPenaltyInfo& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x1bdb;
    }
    std::string token;
    // ORIG 0x0890a33e：flag(-0x15) 为**函数级未初始化 bool**，ScanType 成功后
    // 每轮无条件置 true（0890a39a movb $1），内层 ScanInt(&flag) 复用同槽；
    // v(-0xc) 每轮内层先置 0（0890a3c4 movl $0）再 ScanInt 落值。
    bool flag;
    int i;
    int v;
    // [b28d-solo-OnlyOrigBatch14] P164：ORIG 0x0890a378 条件在**循环头**（入口
    // jmp 条件块 + body 尾 jmp 回条件），4.4 对 while 恒旋转为尾测 ⇒ 改
    // `for(;;){ if(!ScanType) break; ... }` 复现头部条件 + xor$1 物化。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        flag = true;
        if (token == "[COMMON TRAP EXPLOSION]")
        {
            for (i = 0; i <= 4; ++i)
            {
                v = 0;
                v = ScanInt(&flag);
                if (flag)
                {
                    out.m_values[i] = v;
                }
            }
        }
    }
    return 0;
}
