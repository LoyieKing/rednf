// check_cfg_file() —— ORIG 0x829e9ca（0x82 字节）：
// sprintf("<cfg>/%s") + access(F_OK)；存在→true。路径前缀「推断」。
bool check_cfg_file()
{
    char path[0x1e];   // ORIG 0x1e（38B）
    std::memset(path, 0, 0x1e);
    std::sprintf(path, "./cfg/%s.cfg",
                 G_CEnvironment()->get_file_name());
    std::printf("\t- cfg path: %s\n", path);
    // ORIG 0x829ea35：test/sete 物化 + mov$1/mov$0 块选择（P56 勿直 return 表达式）。
    if (access(path, F_OK) == 0)
        return true;
    return false;
}