// ---------------------------------------------------------------------------
// OnAccept — ORIG 0x849045e size 0x174
// ---------------------------------------------------------------------------
// [b28d-D33 N7/N109 封顶] ORIG 0x0849045e。p167 CLEAR 13=13。
// [M1S5 2026-09-11 **推翻 D33 旧判词**] 两处新钥匙使本件 DIFF→AE：
// ①P0/P329：删 `port`/`ip` 两个具名局部（ORIG -0x10/-0x12 无此二槽、值走
//   callee-saved %esi/%ebx）⇒ 内联进 cMyTrace 实参，帧 0x84→0x70=ORIG、
//   addr/addrlen/cMyTrace 槽全部归位（-0x50/-0x54/-0x40/-0x30/-0x20）；
// ②P307：`register socklen_t* plen = &addrlen;` 使取址实参提前物化 —— 复现
//   ORIG 0x0849046d `lea -0x54(%ebp),%esi`（函数头、GetSocket 前）+ 调用点
//   `mov %esi,0x8(%esp)`（4.4 裸形恒延迟到调用点用 %edx 中转）。
// 读数：our_insns 94=94（持平）；TU 四数 ae 13→14、合计 16→17、near 0→1。
// 残余：无（唯一差异为 .o 未重定位的 call/数据地址显示形）。
CSession* CSession::OnAccept()
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    sockaddr_in addr;                         // -0x50(%ebp)，len@-0x54
    socklen_t addrlen = 0x10;
    register socklen_t* plen = &addrlen;      // P307：取址实参提前物化

    int fd = accept(GetSocket(), (sockaddr*)&addr, plen);
    if (fd < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f7, 0)("Accept Socket[%d] Error(%s)",
            fd, strerror(errno));
        return 0;                             // mov $0,%eax
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x1fa, 0)(
        "Accepted other game server ip(%s), port(%d), sock(%d)",
        inet_ntoa(addr.sin_addr), (int)ntohs(addr.sin_port), fd);

    // vtable+8 AllocSession()
    CSession* session = (CSession*)ES()->AllocSession();
    if (session != 0)
    {
        session->set_socket(fd, false);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x202, 0)("session_pool_.Acquire() error");
    }
    return session;                           // ORIG 尾：mov -0xc,%eax
}