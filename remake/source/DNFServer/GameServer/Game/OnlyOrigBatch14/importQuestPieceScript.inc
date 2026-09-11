// ORIG T 0x08919127 _Z22importQuestPieceScriptPKcRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE。
bool importQuestPieceScript(const char* path, std::map<int, int>& out)
{
    // [b28d-solo-OnlyOrigBatch14] 真语义 bug：ORIG 0x0891912f 压参序为
    // `mov 0x8(%ebp),%eax; mov %eax,0x4(%esp); movl $"",(%esp)` ⇒ 调用
    // loadRDARScriptFile(dir="", path)（与 importDifficultPenaltyInfoScript
    // 同形）；原写 (path,"") 使目录/文件参数互换。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x0891915e：`movb $0,-0x25(ok)` 在**外层
    // 循环之前**初始化一次，内层循环回跳 0x0891919a（ScanInt），不重置 ok。
    bool ok = false;
    for (;;)
    {
        // [b28d-solo-OnlyOrigBatch14] ORIG 0x08919178 `xor $1; test; jne
        // <函数尾 return-false 块 08919224（nop 开头）>`（P59/P223）；试
        // goto 尾标签形复现失败块落点。
        if (!ScanType(token, true))
            goto script_fail;
        if (token == "[piece quest list]")
        {
            for (;;)
            {
                int left = ScanInt(&ok);
                if (!ok)
                    break;
                int right = ScanInt(&ok);
                if (!ok)
                    break;
                out.insert(std::make_pair(left, right));
            }
        }
    }
script_fail:
    return false;
}
