// [b28d-47] verdict=DIFF(routed446) [N82/N78] ORIG 0x088baf77
//   修：删发明 joinScriptPath、改真 getScriptFileFullPath ×2 + strcmp(完整路径) 语义
//   + `char path[0x104]={0}` rep stos（P167 CLEAR 13/13）。残余：帧/槽分配 + rep stos
//   展开形（N78/N82）。
// [G17 2026-09-11 结构对齐（ORIG 0x088baf77 逐条切读）]
//   ORIG 语句序实证：① `char path[0x104]={0}` rep stos 0x41 dwords 为**首条**语句
//   （0x88baf83..0x88baf97）→ ② getScriptFileFullPath(path,0x104,dir,filename)
//   （0x88bafb8）→ ③ 死临时 std::string("./Script/Etc/channel_info.etc")
//   （0x88bafbd..0x88bb005 构造+析构）→ ④ altPath 清零 0x228
//   （0x88bb00a..0x88bb01e）→ ⑤ getScriptFileFullPath(altPath,0x104,dir,"Etc/channel_info.etc")
//   → ⑥ strcmp/memcpy → ⑦ setFileStreamData → ⑧ 回调 → return。
//   **ORIG 头部无三参数判空**（0x088baf77 起直接 rep stos；旧版 `if(!dir||!filename||!s)`
//   多 7 条 cmpl/je+mov，ORIG 无对应序列）⇒ 删除；死临时 string 移到首个
//   getScriptFileFullPath **之后**（ORIG 的 string 构造发生在 path 解析后）。
bool LoadStreamFromPlain(const char* dir, const char* filename, DNFLex::stream_data_t* s)
{
    // [b28d-47] 真语义修正（P167：MISSING getScriptFileFullPath ×2 + EXTRA 发明
    // joinScriptPath，§4.1 违规）：ORIG 0x088bafb8/0x088bb040 两次 getScriptFileFullPath
    // 解析完整路径，按 strcmp(完整路径, channel_info 完整路径)==0 决定 memcpy 覆盖；
    // 旧实现以 filename 比较 + 自造路径拼接，语义不同。
    char path[0x104] = {0};          // ORIG rep stos 0x41 dwords（P214③）
    getScriptFileFullPath(path, 0x104, dir, filename);
    std::string unusedTarget("./Script/Etc/channel_info.etc");  // ORIG 死临时（0x088bafbd/0x088bb005）
    (void)unusedTarget;
    char altPath[0x104] = {0};
    getScriptFileFullPath(altPath, 0x104, dir, "Etc/channel_info.etc");
    if (std::strcmp(path, altPath) == 0)
    {
        std::memcpy(path, "./channel_info/channel_info.etc", 0x20);
    }
    // [G17 试形 D] ORIG 块布局 = 成功块 fall-through（0x88bb0a3 `mov $1,%ebx`）+
    //   失败块经 `jne` 落到后方（0x88bb0aa 回调）⇒ goto 形试翻块序。
    if (!g_dnfLex.setFileStreamData(s, path))
        goto fail;
    return true;
fail:
    // ORIG 0x88bb0aa：失败时若注册了回调则调用（参数为完整路径），返回 0。
    // [G17 试形 E 回滚] 显式解引用形（`(*g_fpScriptLoadingErrorFunction)(path)`）实测零变化
    //   ⇒ 该处寄存器角色（ORIG receiver→%edx / 实参→%eax；4.4 反之）为 P75/N18a 跨版本族。
    if (g_fpScriptLoadingErrorFunction)
    {
        g_fpScriptLoadingErrorFunction(path);
    }
    return false;
}