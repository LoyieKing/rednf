// ORIG RecvPacket 0x0810645c：select(300s) → SO_ERROR 探测（结果忽略）→
// Sleep(100ms) → read 追加到 m_buf 尾部并累加 m_field1c。
// ORIG 0x0810645c 全量转写：
//   ret(-0x2c)=0；tv(0x12c,0)@-0x54；额外 dead init(-0x28)=0（0x0810647d）；
//   fd_set@-0xdc 经指针槽(-0x1c)+计数槽(-0x20)手循环清 32 词
//   （0x081064a7 `cmpl $0x1f; setbe` i<=31 形）；FD_SET 宏产无符号
//   `shr $5`+`and $0x1f`+`shl` 位图形（0x081064b8）；select 结果 `jg body`
//   岛（0x0810651c，源码 `if (ret > 0) {...} return 0;`）；getsockopt 局部
//   error@-0x58/len@-0x5c；read 结果槽 -0x24；负判定 `shr $0x1f; test`
//   （0x081065ab，等价 `(unsigned)nread >> 31`）；errno 链 ==EAGAIN||==EINTR||
//   ==EAGAIN||==0（0x081065bb-5e3 四次独立取址，11 出现两次为原码忠实转写）。
// [b28d-18 N82] 残差=帧槽/寄存器（N82） | ORIG 0x0810645c
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N82 槽位/寄存器 + N89 STL 展开。
int CmmChannelProxy::RecvPacket()
{
    // ORIG 0x0810651c `cmpl $0; jg body` + fall-through `mov $0;jmp end`——
    // early-return `if (ret <= 0) return 0;` 极性（技巧 17）。
    // ORIG 0x0810654a getsockopt 第三实参硬编码 $0x8（老 SO_ERROR 值，非本
    // 头文件宏的 4）；level 硬编码 $0x1（0x08106552）。
    // ORIG 0x081065ab 负判定 `shr $0x1f; test al`（0x081064d4 select 后同理
    // 无符号语义）——用无符号比较 `(unsigned)nread > 0x7fffffff` 复现。
    // 槽序对齐（4.4 顶层=先声明较近、block 变量优先占近槽）：目标
    // p -0x1c, i -0x20, nread -0x24, x -0x28, ret -0x2c, tv -0x50/54,
    // error -0x58, len -0x5c, fd -0xdc（ORIG 0x08106468-0x08106494）。
    // 槽位靶：ret -0x2c（0x08106468）、dead -0x28（0x0810647d，tv 写之后
    // 落）、nread -0x24、i -0x20、p -0x1c、tv -0x54/50、error -0x58、
    // len -0x5c、fd -0xdc。声明序控远槽，语句序控 init 位置。
    struct timeval tv;         // 先声明 → 远区 -0x54
    fd_set readfds;            // -0xdc 大块
    int ret = 0;               // 0x08106468 → -0x2c（先声明占更远槽）
    int nUnused;               // dead init@0x0810647d → -0x28
    tv.tv_sec = 300;
    tv.tv_usec = 0;
    nUnused = 0;
    int nread;
    {
        unsigned int i;         // ORIG i(-0x20) 比 pBits(-0x1c) 远 → 先声明
        // [SweepGD 2026-09-10] 旧判词 N125 作废：ORIG 0x0810648f `mov -0x20,%edx;
        // mov -0x1c,%eax; movl $0,(%eax,%edx,4)` 的 scaled 直存由**具名数组成员
        // 视角**复现 —— `fd_set* fds = &readfds; __FDS_BITS(fds)[i]`（P291/P288）；
        // 用 `unsigned long* pBits = (unsigned long*)&readfds` 时 4.4 恒折 `shl $2; add`。
        fd_set* fds = &readfds;
        for (i = 0; i <= 31u; ++i)
        {
            __FDS_BITS(fds)[i] = 0;
        }
    }
    // ORIG 0x081064b8：FD_SET 无符号 `shr $5`/`and $0x1f` 位图形
    // （`mov 0x18,%eax; shr $0x5,%eax; mov %eax,%edx; mov -0xdc(%ebp,%eax,4),%ebx`）。
    // [SweepGD 2026-09-10 实测] 具名 `unsigned long ix`（175 条）优于内联表达式
    // （176 条：4.4 重算两份 shr 各带一次 this 重读）；两形均非 ORIG 单份
    // `%eax`+`mov %eax,%edx` 复制形 ⇒ 取条数较优的具名形，余 4 条记 N18a/N96。
    // [GDataWriter 2026-09-11 负结论] ORIG 0x081064b8 段 FDELT 全程寄存器
    // （`shr $0x5,%eax; mov %eax,%edx` 无槽往返）；具名 `ix`（174 条）与纯
    // `register` 提示（174 条）**实测同读数**（register 零效），内联表达式 176 条
    // ⇒ 取具名形，余 N18a/N96 封顶。
    unsigned long ix = static_cast<unsigned long>(m_sock) >> 5;
    readfds.fds_bits[ix] |= (1UL << (static_cast<unsigned long>(m_sock) & 31));
    ret = select(m_sock + 1, &readfds, 0, 0, &tv);
    if (ret <= 0)
    {
        return 0;
    }
    int error = 0;
    socklen_t len = 4;
    // （取参序残余差：ORIG 0x0810653d &error→%edx 先载、m_sock→%eax、
    // &len→%ecx；4.4 恒 &len 先载且 m_sock 占 %eax ⇒ register asm("ecx")
    // 反而打乱 store 序，probe 证不可翻 ⇒ 记 N 类。）
    getsockopt(m_sock, 1, 8, &error, &len);
    OS_API::Sleep(100);
    nread = static_cast<int>(::read(m_sock, m_buf + m_field1c,
                                    static_cast<size_t>(0x80000 - m_field1c)));
    // ORIG 0x081065a8：`mov -0x24,%eax; shr $0x1f,%eax; test %al,%al; je`——
    // 显式移位形（`>0x7fffffffu` / 裸 `>>31` 均被 4.4 fold 成 cmpl/jns），
    // b28 P0 清绑后恢复：具名局部反引入槽往返，表达式内联 uchar 化实测
    // 与 ORIG 同形（探针 /tmp/l12p7.cpp f：`mov; shr $0x1f,%eax; test %al; je`）。
    if (static_cast<unsigned char>(static_cast<unsigned int>(nread) >> 31))
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
        {
            return 0;
        }
        // [K8GlobalData 2026-09-11 · §6.5 串盲区] ORIG .rodata 0x8b44000 =
        // 'int CmmChannelProxy::RecvPacket()'（0x0810661d/0x08106687 两处
        // `movl $0x8b44000,0x4(%esp)` 直证）⇒ 原码用 __PRETTY_FUNCTION__，
        // 原写法 __FUNCTION__ 只产裸 'RecvPacket'。
        cMyTrace(__PRETTY_FUNCTION__, 0x114, 0)(
            "RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
            m_sock, errno, strerror(errno));
        return -1;
    }
    if (nread == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x11a, 0)(
            "Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)",
            errno, strerror(errno), 0x80000, nread);
        return -1;
    }
    m_field1c += nread;
    return nread;
}