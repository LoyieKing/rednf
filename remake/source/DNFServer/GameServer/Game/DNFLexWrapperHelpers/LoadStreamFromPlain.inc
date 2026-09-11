// [b28d-47] verdict=DIFF(routed446) [N82/N78] ORIG 0x088baf77
//   修：删发明 joinScriptPath、改真 getScriptFileFullPath ×2 + strcmp(完整路径) 语义
//   + `char path[0x104]={0}` rep stos（P167 CLEAR 13/13）。残余：帧/槽分配 + rep stos
//   展开形（N78/N82）。
bool LoadStreamFromPlain(const char* dir, const char* filename, DNFLex::stream_data_t* s)
{
    if (!dir || !filename || !s)
        return false;
    // [b28d-47] 真语义修正（P167：MISSING getScriptFileFullPath ×2 + EXTRA 发明
    // joinScriptPath，§4.1 违规）：ORIG 0x088bafb8/0x088bb040 两次 getScriptFileFullPath
    // 解析完整路径，按 strcmp(完整路径, channel_info 完整路径)==0 决定 memcpy 覆盖；
    // 旧实现以 filename 比较 + 自造路径拼接，语义不同。
    std::string unusedTarget("./Script/Etc/channel_info.etc");  // ORIG 死临时（0x088bafc3）
    (void)unusedTarget;
    char path[0x104] = {0};          // ORIG rep stos 0x41 dwords（P214③）
    getScriptFileFullPath(path, 0x104, dir, filename);
    char altPath[0x104] = {0};
    getScriptFileFullPath(altPath, 0x104, dir, "Etc/channel_info.etc");
    if (std::strcmp(path, altPath) == 0)
    {
        std::memcpy(path, "./channel_info/channel_info.etc", 0x20);
    }
    if (!g_dnfLex.setFileStreamData(s, path))
    {
        // ORIG 0x88bb0aa：失败时若注册了回调则调用（参数为完整路径），返回 0。
        if (g_fpScriptLoadingErrorFunction)
        {
            g_fpScriptLoadingErrorFunction(path);
        }
        return false;
    }
    return true;
}