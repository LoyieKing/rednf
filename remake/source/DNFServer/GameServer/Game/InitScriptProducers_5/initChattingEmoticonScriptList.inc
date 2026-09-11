// ============================================================================
// initChattingEmoticonScriptList —— ORIG 0x088e30f8（init.md:2740 +0x36be）。
// strncpy 长度 0xff；目录 0x94f6bc0/列表 0x94f6cc0。消费方
// CChattingEmoticonList::Load_list（ORIG 0x80e54d2 直接遍历对象 m_fileMap）。
// [b28d-solo AE 43/43] __builtin_strcat 内建展开（旧 N(P209/P75) 判词作废）。
// ============================================================================
bool initChattingEmoticonScriptList(char* dir, char* list)
{
    strncpy(g_ChattingEmoticonScriptBaseDirectory, dir, 0xff);
    if (g_ChattingEmoticonScriptBaseDirectory[strlen(g_ChattingEmoticonScriptBaseDirectory) - 1] != '\\' &&
        g_ChattingEmoticonScriptBaseDirectory[strlen(g_ChattingEmoticonScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_ChattingEmoticonScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptListFile(&g_ChattingEmoticonScriptFileList,
                                g_ChattingEmoticonScriptBaseDirectory, list, 0))
        return 0;
    return 1;
}