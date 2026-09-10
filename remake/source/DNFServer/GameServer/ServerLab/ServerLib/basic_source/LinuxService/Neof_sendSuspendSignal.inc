void Neof_sendSuspendSignal()
{
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    // ORIG DWARF 声明序：ret(344)、pid(345)、fp(346)；声明不初始化，体中赋值。
    int ret;
    int pid;
    FILE* fp;
    fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("%s process id file open \xBD\xC7\xC6\xD0\n", file_path);
    }
    else
    {
        fscanf(fp, "%d", &pid);
        if (pid < 1)
        {
            fclose(fp);
            printf("%d\xB9\xF8\xC0\xC7 \xC0\xDF\xB8\xF8\xB5\xC8 process id\n", pid);
        }
        else
        {
            ret = kill(pid, 0xc);
            if (ret < 0)
            {
                fclose(fp);
                printf("%d\xB9\xF8 process\xB7\xCE \xC1\xBE\xB7\xE1 signal \xBC\xDB\xBD\xC5 \xBD\xC7\xC6\xD0", pid);
            }
            else
            {
                printf("SEND SUSPEND SIGNAL TO %d\n", pid);
                fclose(fp);
            }
        }
    }
