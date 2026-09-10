bool Neof_sendTerminateSignal(char* ServiceName)
{
    puts("called Neof_sendTerminateSignal");
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    printf("killing path [%s]\n", file_path);
    // ORIG DWARF 声明序：ret(238)、pid(239)、fp(240)；声明不初始化，体中赋值。
    int ret;
    int pid;
    FILE* fp;
    fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("%s process id file open \xBD\xC7\xC6\xD0\n", file_path);
        return false;
    }
    fscanf(fp, "%d", &pid);
    printf("killing [%s], PID=[%d]\n", file_path, pid);
    if (pid < 1)
    {
        fclose(fp);
        printf("%d\xB9\xF8\xC0\xC7 \xC0\xDF\xB8\xF8\xB5\xC8 process id\n", pid);
        return false;
    }
    ret = kill(pid, 0xc);
    if (ret != 0)
    {
        fclose(fp);
        printf("error, [ret = %d] called Neof_sendTerminateSignal()\n", ret);
        printf("error, %d\xB9\xF8 process\xB7\xCE \xC1\xBE\xB7\xE1 signal \xBC\xDB\xBD\xC5 \xBD\xC7\xC6\xD0 error = '%s'\n", pid, strerror(errno));
        return false;
    }
    puts("************************************************************");
    printf("******************kill pid=\'%5d\' Success!!!***************\n", pid);
    puts("************************************************************");
    fclose(fp);
    return true;
