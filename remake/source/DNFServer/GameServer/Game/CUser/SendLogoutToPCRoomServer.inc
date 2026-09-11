// ORIG T 0x0868170c sz0x113：PC 房 logout 通知。stLoginPacket 打包 →
// GetLength → SendTcpPacket（nexon_packet 已建模，符号直调）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868170c | _ZN5CUser24SendLogoutToPCRoomServerEv
void CUser::SendLogoutToPCRoomServer()
{
    // 2026-09-11（G4）：ORIG 0x0868170c 的 stLogin 基址 -0x216、末字段
    //   m_canTry 落 -0x11 ⇒ 结构实体为 0x20e 字节（nexon_packet::stLogin，
    //   packed 且 sizeof==0x20e）；我方 `char login[0x210]` 的强转指针写法使
    //   gcc 每次经 `lea -0x21c(%ebp),%ebx` 缓存基址（push %ebx + 4×lea），
    //   ORIG 全部为 ebp 相对直写。改结构实体 + 成员访问（P0/P183 正形）。
    nexon_packet::stLogin login;
    // b28d-lane5：同 SendLoginToPCRoomServer——ORIG 0x086817xx 逐字段重发
    // `lea -0x226(%ebp)`（P183），删 lp 指针缓存。
    // 2026-09-10（G3CUser-B 负结论）：本 .inc 只含 `char login[0x210]` 声明，函数的
    //   全部指令在**聚合器 CUser.cpp:3336+** 内联段。实测 diff（ORIG 0x0868170c vs
    //   ours 78 insn）：ORIG `sub $0x1238` 且**不 push %ebx**、每站点重发
    //   `lea -0x216(%ebp),%eax; add $0x2,%eax`；我方 `push %ebx; sub $0x1234` 并把
    //   `lea -0x21c(%ebp)` 缓存进 %ebx（P183 反例，每站点省 1 条 but 帧/寄存器形反）。
    //   ⇒ 修复点在 CUser.cpp 的宏段（`#define LP_LOGOUT` 后的 LP_LOGOUT->… 语句需
    //   改为每次 `((nexon_packet::stLogin*)(login))->…` 直写、不引入 `pLogin` 局部），
    //   非本 .inc 写域；已报 Main。
    login.m_type = 1;
    strcpy(login.m_account, get_acc_name());
    login.m_accLen = (char)strlen(get_acc_name());
    strcpy(login.m_account, get_acc_name());   // ORIG 重复一次（+0x049/+0x064 双 strcpy）
    login.m_accLen = (char)strlen(get_acc_name());
    strcpy(login.m_characName, getCurCharacName());
    login.m_charLen = (char)strlen(getCurCharacName());
    login.m_ip = (unsigned int)get_local_ip_address();
    login.m_canTry = 1;

    nexon_packet::stLoginPacket pkt(login);
    // ORIG 0x086817f0..0x08681818：GetLength 结果经 %eax 直传 0x8(%esp)，
    //   无 len 栈槽（P0 右值内联；我方旧写多 2 条 + 一个 -0xc 槽）。
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, pkt.GetLength());
}