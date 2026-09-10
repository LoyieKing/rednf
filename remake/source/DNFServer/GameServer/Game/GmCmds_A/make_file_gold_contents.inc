void CBoosterTest::make_file_gold_contents(CUser* user, char* fileBuf,
                                           std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] ORIG +0x00b 先清零 maxGold（-0x10/-0xc）再 rep stos
    // ⇒ 声明序 maxGold 在前（P0 槽序）。
    unsigned long long maxGold = 0;
    // [b28d-solo-GmCmds_A] P214③：ORIG 首清零为内联 rep stos 0x100 dwords
    // （=0x400 字节，ORIG +0x019 段）⇒ 源形是数组零初始化，且长度真值 0x400
    // （旧 `memset(line,0,0x100)` 少清 0x300 字节，属真 bug）。
    char line[1024] = {0};

    for (std::map<int, int>::iterator it = result.begin(); it != result.end();)
    {
        sprintf(line, "%d gold count = %d \n", it->first, it->second);
        // [b28d-solo-GmCmds_A] P0：ORIG 无 gold 具名槽（imul 结果直入
        // `mov %eax,%edx; sar $0x1f; add %eax,-0x10; adc %edx,-0xc`）。
        maxGold += it->first * it->second;

        if (strlen(fileBuf) + strlen(line) > 89999)
        {
            static char sAppend[] = "결과 파일 이어붙힘";
            SendChatMsg(user, sAppend);
            break;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }

    sprintf(line, "max gold = %llu \n", maxGold);
    strcat(fileBuf, line);
    // [b28d-solo-GmCmds_A] P171：ORIG +0x1cb 末尾死写入 memset(line,0,0x400)
    // （结果从不被读）照抄，缺失即少一个调用点（p167 MISSING memset）。
    memset(line, 0, 0x400);
