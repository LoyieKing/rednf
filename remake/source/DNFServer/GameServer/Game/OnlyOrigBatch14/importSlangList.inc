bool importSlangList(char* path, std::vector<std::string>* out)
{
    // [b28d-solo-OnlyOrigBatch14] 真语义 bug（同 importQuestPieceScript）：
    // ORIG 0x088ffab1 压参序 = loadRDARScriptFile(dir="", path)。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    std::string a;
    std::string b;
    for (;;)
    {
        if (!ScanType(token, true))
            goto slang_done;
        if (token == "[SLANG]")
        {
            // [b28d-solo-OnlyOrigBatch14] ORIG 0x088ffb4a 段布局：body 在前、
            // 条件块在尾（`jmp <cond>` 后 body，cond 为
            // `call ScanStr; test %al,%al; je`×2 —— bool 返回**正向**判断，
            // 无 xor 取反物化）⇒ 源形为 `while (ScanStr(&a) && ScanStr(&b))`
            // （P283 底部条件 + P292 bool 返回正极性）。
            // 判词·AE 归零（107=107）。
            while (ScanStr(&a) && ScanStr(&b))
            {
                out->push_back(a);
                out->push_back(b);
            }
        }
    }
slang_done:
    return true;
}
