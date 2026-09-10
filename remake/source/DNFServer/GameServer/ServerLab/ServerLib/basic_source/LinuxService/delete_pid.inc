bool delete_pid()
{
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    printf("delete_pid() : file[%s]\n", file_path);
    if (unlink(file_path) == 0)
    {
        printf("delete %s\n", file_path);
        return true;
    }
    else
    {
        return false;
    }
