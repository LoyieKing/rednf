int save_pid()
{
    char path[0x1e];
    memset(path, 0, sizeof(path));
    sprintf(path, "./pid/%s.pid", G_CEnvironment()->get_file_name());
    ssize_t n;
    int fd = open(path, 0x42, 0x1a4);
    if (fd < 0)
        return 0;
    char buf[0x200];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%ld\n", static_cast<long>(getpid()));
    n = write(fd, buf, strlen(buf));
    if (n < 0)
    {
        close(fd);
        return 0;
    }
    return 1;
