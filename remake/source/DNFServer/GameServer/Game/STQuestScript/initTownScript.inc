bool initTownScript(char* dir, char* list)
{
    strncpy(g_townScriptBaseDirectory, dir, 0x100);
    // ORIG 0x8aad9ec 族形：内联补齐（strlen×3 + 追加 "\\"），无空串防御
    // （ORIG len==0 时读 buf[-1] 为原始 UB 形态，P140 忠实保留）；P49 禁本 TU
    // 外 helper——原 AppendDirSeparator 属发明符号（ORIG nm 无）。
    // P303：追加段必须 __builtin_strcat 形。
    if (g_townScriptBaseDirectory[strlen(g_townScriptBaseDirectory) - 1] != '\\' && g_townScriptBaseDirectory[strlen(g_townScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_townScriptBaseDirectory, "\\");
    }
    // P304：权威型 &g_townScriptFileList 直压（ORIG movl $&g_..,(%esp)）。
    if (!loadRDARScriptListFile(&g_townScriptFileList,
                                g_townScriptBaseDirectory, list, 0))
    {
        return 0;
    }
    return 1;
