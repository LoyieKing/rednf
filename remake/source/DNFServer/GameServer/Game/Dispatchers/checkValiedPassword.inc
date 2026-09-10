bool Dispatcher_MouseRegister::checkValiedPassword(char const* pwd)
{
    int len = strlen(pwd);
    if (len > 6)
    {
        return 0;
    }
    for (int i = 0; i < len; ++i)
    {
        // ORIG 0x082632ca：三个判定各自完整重取 `mov -0xc,%eax; add 0x8(%ebp),%eax;
        // movzbl (%eax),%eax`（P0/P48：字符值**无槽往返** ⇒ 原码无 `char c` 局部）。
        // 放行条件为空 then、拒绝走 else{return;}（P285：4.4 对纯正向 if 恒
        // jump-thread 掉体尾 jmp 岛；ORIG 第三条件为 `jle <i++>` + 拒绝块直落）。
        if (pwd[i] == 'a' || (pwd[i] > '/' && pwd[i] <= '9'))
        {
        }
        else
        {
            return 0;
        }
    }
    return 1;
