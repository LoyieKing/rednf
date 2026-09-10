void send_suspend_signal()
{
    int pid;
    char buf[0x1e];
    int r;
    FILE* f;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "pid/%s.pid", G_CEnvironment()->get_file_name());
    f = fopen(buf, "r");
    if (f == 0)
    {
        printf("%s process id file open ½ÇÆÐ\n", buf);
        return;
    }
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d¹øÀÇ Àß¸øµÈ process id\n", pid);
        return;
    }
    r = kill(pid, 10);
    if (r < 0)
    {
        fclose(f);
        printf("%d¹ø process·Î Á¾·á signal ¼Û½Å ½ÇÆÐ", pid);
        return;
    }
    printf("SEND SUSPEND SIGNAL TO %d\n", pid);
    fclose(f);
