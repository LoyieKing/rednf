bool Manager::Load()
{
    // ORIG 0x080e0e08：printf("[!] Loading CeraShop Add Restrict Script ...") 后
    // 调 ScriptLoader::LoadScript()；非 0 返回 0，否则 1。
    printf("[!] Loading CeraShop Add Restrict Type script - ");  // ORIG 串无 \n，仅尾随空格（带 \n 会被 GCC 转 puts）
    // [b28c-4 P0] 存量 `register unsigned char nz asm("al")` 已清除（全域禁绑，
    // N96 登记），按 P168a① 复测自然形：ORIG 尾 test/setne/test/je 常量岛。
    if (m_loader.LoadScript() != 0)
        return 0;
    return 1;
}