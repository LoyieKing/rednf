bool save_pid(char* pidName)
{
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), pidName);
    printf("pid file_path-%s\n", file_path);
    // ORIG DWARF 声明序：buf(618)、write_byte(619)、fd(620)——fd 在 buf 后声明。
    char buf[0x200];
    ssize_t write_byte;
    int fd = open(file_path, 0x42, 0x1a4);
    if (fd < 0)
    {
        return false;
    }
    memset(buf, 0, 0x200);
    sprintf(buf, "%ld\n", getpid());
    write_byte = write(fd, buf, strlen(buf));
    if (write_byte < 0)
    {
        close(fd);
        return false;
    }
    return true;
