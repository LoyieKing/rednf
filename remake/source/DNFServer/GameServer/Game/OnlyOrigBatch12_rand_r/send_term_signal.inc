// ===========================================================================
// send_term_signal  ORIG T 0x0829e5de：读 pid/<name>.pid，kill(pid,15)。
// （ORIG TU = App.cpp）
// ===========================================================================
// [b28d-solo-OnlyOrigBatch12] AE：P309 头部声明序倒置翻正（原 NEAR 9）。
// ORIG 槽序 f(-0xc)/r(-0x10)/pid(-0x14)/path(-0x32..-0x15) 即「先声明浅」；
// 4.4 对函数头声明亦按先声明浅（P309，推翻旧 N95/P112 判词），故声明序改为
// r,f,pid,path——同层 int/指针中先声明者深（r 先于 f 才得 r@-0x10、f@-0xc），
// pid 先于 path 才得 pid@-0x14、path@-0x32。四槽与 ORIG 逐一对齐、帧 0x48
// 不变、助记符序列全等。ORIG 0x0829e5de
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
}