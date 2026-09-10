bool nsl::LinuxService::checkConfigFile()
{
    puts("In LinuxService::checkConfigFile");
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    puts(file_path);
    if (access(getConfigFileName(), 0) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
