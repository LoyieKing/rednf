// ORIG T _Z24SetPriorityBaseDirectoryPKc 0x088bb40a
// （报告 _lt_global_gt_/SetPriorityBaseDirectory.md）：
// 构造临时 string；非空才 push_back（空串忽略）。
void SetPriorityBaseDirectory(const char* dir)
{
    std::string path(dir);
    if (path.length() == 0)
    {
    }
    else
        g_szScriptFilePriorityBaseDirectory.push_back(path);
}