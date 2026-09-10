bool initQuestScript(const char* dir, const char* list)
{
    strncpy(g_questScriptBaseDirectory, dir, 0x100);
    // ORIG 0x8a6166a..0x8a616c1 族形：内联补齐（strlen×3 + 追加 "\\"），无空串
    // 防御（ORIG len==0 时读 buf[-1] 为原始 UB 形态，P140 忠实保留）；P49 禁
    // 本 TU 外 helper——原 AppendDirSeparator 属发明符号（ORIG nm 无）。
    // P303：追加段必须 __builtin_strcat 形（手写 memcpy(dst+strlen,..) 恒 DIFF）。
    if (g_questScriptBaseDirectory[strlen(g_questScriptBaseDirectory) - 1] != '\\' && g_questScriptBaseDirectory[strlen(g_questScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_questScriptBaseDirectory, "\\");
    }
    // P304：权威型 &g_questScriptFileList 直压（ORIG movl $&g_..,(%esp)）。
    if (!loadRDARScriptListFile(&g_questScriptFileList,
                                g_questScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
