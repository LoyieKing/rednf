bool loadRDARScriptFile(const char* directory, const char* path)
{
    // ORIG 0x088bbcf1：path 空返回 false；g_onLoadScriptFileFunction 非空则
    // 调用；随后 g_dnfLex.switchInputStream(directory, path, 0)。
    if (!path)
        return false;
    if (g_onLoadScriptFileFunction != 0)
        g_onLoadScriptFileFunction();
    // P0/P22：ORIG 尾 `mov %al,-0x9; movzbl -0x9,%eax`（bool 结果落槽再回读）
    // ⇒ 具名 bool 局部走槽；直接 return call() 4.4 直返失 2 条。
    bool ok = g_dnfLex.switchInputStream(directory, path, 0);
    return ok;
