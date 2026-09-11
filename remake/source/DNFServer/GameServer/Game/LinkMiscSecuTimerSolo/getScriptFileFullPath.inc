// ===========================================================================
// getScriptFileFullPath  ORIG T 0x88bba4e
// 依次用优先根目录表（g_szScriptFilePriorityBaseDirectory）拼 dir/name，
// 命中 access/findFileWithServerToken 即采用；否则落到
// g_szScriptFileBaseDirectory。out 尺寸由 size 限制。
// ===========================================================================
void getScriptFileFullPath(char* out, unsigned int size, const char* dir,
                           const char* name)
{
    if (!g_szScriptFilePriorityBaseDirectory.empty())
    {
        int i = static_cast<int>(g_szScriptFilePriorityBaseDirectory.size()) - 1;
        for (; i >= 0; --i)
        {
            ss_strcpy(out, size,
                      g_szScriptFilePriorityBaseDirectory[i].c_str());
            if (dir != 0)
                ss_strcat(out, size, dir);
            if (name != 0)
                ss_strcat(out, size, name);
            if (access(out, 0) == 0)
                return;
            if (findFileWithServerToken(out))
                return;
        }
        out[0] = 0;
    }
    ss_strcpy(out, size, g_szScriptFileBaseDirectory);
    if (dir != 0)
        ss_strcat(out, size, dir);
    if (name != 0)
        ss_strcat(out, size, name);
    if (access(out, 0) != 0)
        findFileWithServerToken(out);
}