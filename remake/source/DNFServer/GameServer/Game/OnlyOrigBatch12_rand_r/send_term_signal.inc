int send_term_signal()
{
    // pid 无预清零（ORIG 无 movl $0,-0x14），f/r 声明赋值分离（P198 孪生）。
    int r;
    FILE* f;
    int pid;
    char path[0x1e];
    memset(path, 0, sizeof(path));
    sprintf(path, "pid/%s.pid", G_CEnvironment()->get_file_name());
    f = fopen(path, "r");
    if (!f)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path);
        return 0;
    }
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        return 0;
    }
    r = kill(pid, 15);
    if (r < 0)
    {
        fclose(f);
        printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
        return 0;
    }
    fclose(f);
    return 0;
