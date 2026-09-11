// ==== Dispatcher_CeraPackageOpen::read (ORIG 0x081e76fa) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e76fa orig/ours=95/98 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0x10(%ebp),%eax | movl   $&_ZZN26Dispatcher_CeraPackageOpen4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp) | mov    -0x10(%ebp),%e
int Dispatcher_CeraPackageOpen::read(PacketBuf& packet, MSG_BASE& msg)
{
    // [G6] ORIG 0x081e7706 `mov -0x10(%ebp),%eax; add $0xe,%eax` ⇒ M 是**具名槽**（-0x10）；
    //   我方 `char* const M`（顶层 const）被 4.4 折成形参直读 ⇒ 去 const 恢复槽局部。
    // [g05 试形 D] ORIG 使用点一律从具名槽读（`mov -0x10(%ebp),%eax`）。4.4 对**入口块内
    //   初始化**的指针局部做前向替换（折叠成 `0x10(%ebp)` 形参直读；g05 已在
    //   check_error_69 实证「声明移出入口块」即保留槽）⇒ 先声明后赋值，并让 MPK 同走 M。
    char* M;
    M = reinterpret_cast<char*>(&msg);
    // +0x10 数量字节落在冻结头 m_pad10 区（MSG_OPEN_CERAPACKAGE 无具名字段，
    // 已 hub 报 Main 补 m_itemCount；此处按具名成员（内联视图，零新增槽）访问）
    // [L7DispG2] 读值形：ORIG 0x081e7793/0x081e786d `movzbl 0x10(%eax),%eax`
    //   单 disp；旧形 `reinterpret_cast<const unsigned char*>(&MPK->m_pad10[0])`
    //   产 `add $0x10,%eax; movzbl (%eax),%eax`（+1/处）。改用 packed 视图的
    //   unsigned char 成员 ⇒ 取址仍 `add $0x10`（与 ORIG 0x081e774e 同形）、
    //   读值折成单 disp。
    //   （`__attribute__((packed))` 成员取址会被 4.4 拒绝 ⇒ 用 pragma pack 形。）
#pragma pack(push, 1)
    struct CeraPkgMsgView
    {
        char m_pad0d[0xe];       // +0x00..+0x0d
        short m_0e;              // +0x0e
        unsigned char m_count;   // +0x10
    };
#pragma pack(pop)
#define MPK (reinterpret_cast<CeraPkgMsgView*>(M))
    if (!packet.get_short(*reinterpret_cast<short*>(&MPK->m_0e)))
    {
        return LineFunc(
            0x636b,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    if (!packet.get_byte(MPK->m_count))
    {
        return LineFunc(
            0x636e,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    // cmp $0x23,%al; jbe → unsigned byte ≤ 0x23 才通过
    if (MPK->m_count > 0x23)
    {
        return LineFunc(
            0x6371,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    for (unsigned int i = 0;
         i < MPK->m_count; ++i)
    {
        // [g05 试形 E 负结论] 分层指针算术 `(ulong*)(M+(i+2)*8)+1` ⇒ 95 条且 M 读提前
        //   （劣化，已回滚）⇒ 维持左结合 `M + (i+2)*8 + 4` 形（残余 = 4.4 把 +4 折进
        //   索引侧，ORIG 独立第三段；P205 类）。
        if (!packet.get_int(
                *reinterpret_cast<unsigned long*>(M + (i + 2) * 8 + 4)))
        {
            return LineFunc(
                0x6376,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        if (!packet.get_byte(
                *reinterpret_cast<unsigned char*>(M + (i + 2) * 8 + 8)))
        {
            return LineFunc(
                0x6379,
                __PRETTY_FUNCTION__,
                0, 0);
        }
    }
    return 0;
}