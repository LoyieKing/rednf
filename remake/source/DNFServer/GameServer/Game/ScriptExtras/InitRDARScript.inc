bool InitRDARScript(const char* dir, bool compiled, const char* file, int* err)
{
    g_dnfLex.SetHandler(&isDbcsLead, &LexerOutput, &LexerError,
                        compiled ? LoadStreamFromPack : LoadStreamFromPlain);

    std::strncpy(g_szScriptFileBaseDirectory, dir,
                 sizeof(g_szScriptFileBaseDirectory));
    unsigned int len = static_cast<unsigned int>(
        std::strlen(g_szScriptFileBaseDirectory));
    if (len == 0)
        return false;
    if (g_szScriptFileBaseDirectory[len - 1] != '\\' &&
        g_szScriptFileBaseDirectory[len - 1] != '/')
    {
        std::memcpy(g_szScriptFileBaseDirectory + len, "\\", 2);
    }

    std::string packName = "Script.pvf";
    s_bUseScriptPack = compiled ? 1 : 0;
    if (file != 0 && file[0] != 0)
    {
        s_bUseScriptPack = 1;
        packName = file;
    }

    // ORIG 0x088bb72d..0x088bb7ce（R3 迁移后补全的真实语义，替旧「假近似」）：
    //   if (s_bUseScriptPack) {
    //     rc = g_script_package.open(packName.c_str(), 0, PACK_LOAD_INDEX); // +0x18c
    //     if (err) *err = rc;                                              // +0x194..1a0
    //     if (rc) return false;                                            // +0x1a2
    //     if (!loadStringTable()) return false;                            // +0x1af
    //   }
    //   importKeyNameIndexTable();  // +0x1c2（返回值不检查，两路汇入）
    //   return true;                // +0x1c7
    if (s_bUseScriptPack)
    {
        int rc = g_script_package.open(packName.c_str(), 0, PACK_LOAD_INDEX);
        if (err)
            *err = rc;
        if (rc != 0)
            return false;
        if (!loadStringTable())
            return false;
    }
    importKeyNameIndexTable();
    return true;
