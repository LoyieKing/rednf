// rdar_init(char**) —— ORIG 0x829e5bf（0x1f 字节）：make_dir("./rdar") 后
// 将 argv 交予 daemon_init（ORIG 0x829e8b9 调用，argv[2]="start"/"start_all"
// 时 fork 守护化；定义 OnlyOrigBatch11.cpp，ORIG T 0x0829e8b9）。
int daemon_init(char** argv);
