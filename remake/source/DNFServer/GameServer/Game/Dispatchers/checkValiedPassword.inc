// ORIG T 0x082632ca：长度 <7 且仅含 'a'/'0'..'9' 时有效（返回 int，无 bool 规范化 P9）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082632ca orig/ours=39/35 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | cmpl   $0x6,-0x10(%ebp) | movl   $0x0,-0xc(%ebp)
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
}