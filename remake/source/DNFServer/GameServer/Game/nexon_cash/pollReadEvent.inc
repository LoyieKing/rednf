// 共同骨架（ORIG 三者同构，仅 select 第二/三/四参与超时不同）：
// 清零 fd_set(32*int)，FD_SET(m_socket)，select(nfds=2, ...)，ret !=0。
// [b28d-D43] AE: FD_ZERO→显式循环（pset/i 全头部声明复现 -0x1c/-0x20 槽序）+FD_SET unsigned cast 复现 shr（ORIG 0x081afcc8）。
bool TCPSocket::pollReadEvent() const
