// CShutdowManager::SendShutdownSignal() —— ORIG 0x82a35a6（0x116 字节）：
// 与 send_suspend_signal 同构，向 pid 文件记录的进程发 SIGTERM(12)。
void CShutdowManager::SendShutdownSignal()
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
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", buf);
        return;
    }
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        return;
    }
    r = kill(pid, 12);
    if (r < 0)
    {
        fclose(f);
        printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
        return;
    }
    printf("SEND SHUTDOWN SIGNAL TO %d\n", pid);
    fclose(f);
}