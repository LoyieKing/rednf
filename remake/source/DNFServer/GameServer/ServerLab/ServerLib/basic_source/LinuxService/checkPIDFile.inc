bool nsl::LinuxService::checkPIDFile()
{
    puts("In LinuxService::checkPIDFile");
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), getPIDFileName());
    puts(file_path);
    if (access(file_path, 0) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
