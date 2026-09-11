// b28d-solo-ARAD verdict=IDENTICAL/AE：P248 档①（callee 返回类型旋钮）——返回类型
// int→bool 复现 ORIG 0x0819798d `call CUser::Send; mov %eax,%ebx`（4.4 对 int 返回恒
// `movzbl %al,%ebx`；bool 返回直出全宽 mov，探针 /tmp/probe_arad2.cpp f_bool）。
// 返回类型不入 mangled；调用点（Arad_ServerStateManager/GameWorld/Inter_LoadEtc）均
// 作语句调用忽略返回值，语义等价（0=失败，非0=成功）。
// p167：ORIG 0x08197963 调 put_binary(char*,int)（Pc）⇒ cast 改 (char*)（原
// (const char*) 选 PKc，差集 1/1）；改后 0/0 CLEAR。
bool notifyOpenMessageDialog(CUser* user, int a, int b,
                             const AradServerStateMessage& msg)
{
    // [b23判词·AE假阳性] 65/65 归一化逐条相等（含 guard 局部 -0x18 寻址、put_binary
    // 的 `movl $0xc` 长度实参、异常清理路径的 %ebx/%esi 交换序）；残余 diff 全为未
    // relink .o 的 call 重定位占位 + GlobalData::s_systemTime_ 数据地址显示 0x0
    // （§6.3 假阳性），relink 后转 IDENTICAL_AE。
    if (user == 0)
        return 0;
    PacketGuard guard;
    // ORIG 0x8197907：getCurSec() 结果存 -0xc(%ebp) 栈槽后未再读（P0 具名死局部，
    // ORIG 原码含该存储语句；-O0 下保留槽与 store，与 ORIG 一致）。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    (void)curSec;
    guard.put_header(0, 0x21a);
    guard.put_int(a);
    guard.put_int(b);
    // b28d-solo-ARAD：ORIG 0x08197963 调 put_binary(char*,int)（Pc），非 const 版；
    // 原 (const char*) cast 选成 PKc 重载（p167 EXTRA/MISSING 各 1）。
    guard.put_binary((char*)&msg, 0xc);
    guard.finalize(true);
    return user->Send(guard);
}