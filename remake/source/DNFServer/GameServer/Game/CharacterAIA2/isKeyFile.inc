// isKeyFile —— 报告 _lt_global_gt_/isKeyFile.md：非空且以 ".key" 结尾。
bool isKeyFile(const char* name)
{
    if (name == 0)
        return false;
    // ORIG 0x088c9a65：strlen 结果落命名槽；strcmp 命中/未命中走分支常量岛（P56）。
    unsigned int len = strlen(name);
    if (strcmp(name + (len - 4), ".key") == 0)
        return true;
    return false;
}