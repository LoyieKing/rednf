void rdar_init(char** argv)
{
    static char s_rdarDir[] = "./rdar";  // ORIG 字符串 0x8c18171 内容「推断」
    make_dir(s_rdarDir);
    daemon_init(argv);
