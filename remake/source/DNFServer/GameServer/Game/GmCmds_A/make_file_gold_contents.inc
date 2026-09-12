// ORIG make_file_gold_contents 0x084af7fc：金币分布行 + 尾部 max gold 合计。
// [W13BGmCmdsWarRoom 2026-09-11] **串轴真缺陷修复**：ORIG 0x084af983
//   `movl $"max gold = %I64u \n",0x4(%esp)` 逐字节 = `max gold = %I64u \n`；
//   我方原写 `%llu`（前轮「glibc 语义承载」的改写）⇒ .rodata 实际字节与 ORIG 不同，
//   而件级 verdict 因 §AE 重定位归一（.rodata+addend 形）**看不见**该差
//   （陷阱⑪族：`.rodata`/`.LC*` 串差异被 clean() 归一）。strall 轴暴露。
//   ⇒ 按 ORIG 原样回填 `%I64u`（ABI 无差：实参仍是 64 位 maxGold）。
// [b28d-47] verdict=DIFF(routed) [N82/P112] ORIG 0x084af7fc orig/ours=121/113
//   p167 CLEAR（18/18 调用点全等）⇒ 语义完成；残差为槽/寄存器分配跨版本差。
// [b28d-solo-GmCmds_A] verdict=IDENTICAL/AE ndiff=0 [P214③+P171+P0]
// 首清零 rep stos 0x400B（数组零初始化）+ 末尾死写入 memset（P171）+ maxGold 前置。
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
            // [R11fixE5] ORIG 0x084af8cc `movl $0x8c7ff03,0x4(%esp)`（与 makeFileContents
            // 同一 .rodata 串）⇒ 字面量直传，raw 逐字节同下。
            SendChatMsg(user, "\xb0\xe1\xb0\xfa \xc6\xc4\xc0\xcf \xc0\xcc\xbe\xee\xba\xd9\xc8\xfb");
            break;
        }
        strcat(fileBuf, line);
        memset(line, 0, 0x400);
        result.erase(it++);
    }

    // [W13BGmCmdsWarRoom] ORIG 0x084af983 原样串（逐字节）：`max gold = %I64u \n`
    sprintf(line, "max gold = %I64u \n", maxGold);
    strcat(fileBuf, line);
    // [b28d-solo-GmCmds_A] P171：ORIG +0x1cb 末尾死写入 memset(line,0,0x400)
    // （结果从不被读）照抄，缺失即少一个调用点（p167 MISSING memset）。
    memset(line, 0, 0x400);
}