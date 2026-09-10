bool check_pid_file()
{
    char path[0x1e];   // ORIG 0x1e（38B）
    std::memset(path, 0, 0x1e);
    std::sprintf(path, "./pid/%s.pid",
                 G_CEnvironment()->get_file_name());
    std::printf("\t- pid path: %s\n", path);
    if (access(path, F_OK) == 0)
        return true;
    return false;
