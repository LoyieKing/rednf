bool initCreatureScript(const char* dir, const char* list, const char* expTable)
{
    strncpy(g_creatureScriptBaseDirectory, dir, 0x100);
    if (g_creatureScriptBaseDirectory[strlen(g_creatureScriptBaseDirectory) - 1] != '\\' &&
        g_creatureScriptBaseDirectory[strlen(g_creatureScriptBaseDirectory) - 1] != '/')
    {
        __builtin_strcat(g_creatureScriptBaseDirectory, "\\");
    }
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, expTable))
        return false;
    for (int i = 0; i <= 0x62; ++i)
        CreatureScript::ExpTable_[i] = ScanInt((bool*)0);
    if (!loadRDARScriptListFile(&g_creatureScriptFileList,
                                g_creatureScriptBaseDirectory, list, 0))
        return 0;
    return 1;
