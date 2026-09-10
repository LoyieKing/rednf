int DB_LoadEtc::GetUserRecord(SIG_LOAD_ETC* sig)
{
    (void)sig;
    // ORIG 0x084075c4 六指令含 `sub $0x10,%esp`+leave：4.1 对本函数体预留帧空间
    // （无 call 也展开帧），dummy 数组令 4.4 同形（P31；16B 边界内不触发 4.4
    // 的 32B 对齐，实测同形）。
    char frame[0x10];
    (void)frame;
    return 1;
