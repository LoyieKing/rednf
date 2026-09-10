bool IsLightServer()   // 同步 DataManagerInitDepends.h:42/62 bool 化（ORIG 0x822ad44 bool 返回形；调用点 test %al 无 setne，giveup_panalty 0x086786cb 实证；经 GDB hub 授权代落）
{
    // ORIG 0x822ad44：旗标指针先落局部变量再解引用。
    bool* flag = GetLightServerFlag();
    return *flag;
