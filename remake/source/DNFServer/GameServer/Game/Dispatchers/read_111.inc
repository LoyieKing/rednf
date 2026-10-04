// ORIG 0x0825ed06：读 1 字节 count（须 0..5），再按 count 读 int 写入
// msg+0xd 起任务 idx 区；任一步失败走 LineFunc（ORIG 返回其断言值）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825ed06 orig/ours=62/67 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | movl   $&_ZZN26Dispatcher_SaveQuestNotify4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp
int Dispatcher_SaveQuestNotify::read(PacketBuf& packet, MSG_BASE& msg)
{
    // [b28d-solo-Dispatchers] P212：ORIG 函数头即 `mov 0x10(%ebp),%eax; mov %eax,
    //   -0x10(%ebp)`（msg 指针槽首行物化）⇒ req 声明提前。
    MSG_SAVE_QUEST_NOTIFY& req = reinterpret_cast<MSG_SAVE_QUEST_NOTIFY&>(msg);
    char count = 0;
    if (!packet.get_byte(count))
    {
        return LineFunc(0x2b3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count < 0 || count > 5)
    {
        return LineFunc(0x2b6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (int)count; ++i)
    {
        // [b28d-solo-Dispatchers] P0/P156：ORIG 0x0825efxx `mov -0xc(%ebp),%eax;
        //   shl $0x2,%eax; add -0x10(%ebp),%eax; add $0xd,%eax`（指针算术直传
        //   get_int 目标地址，无 value 局部槽、无独立写回）⇒ 删局部改直接取址；
        //   基址经 req 槽（-0x10）而非形参。
        // [G1-Dispatchers] ORIG 0x0825efxx 段重定位目标为 `_ZN9PacketBuf7get_intERj`
        // （unsigned int&），我方传 int& 选到 `…7get_intERi`（p167 MISS/EXTRA 各 1）。
        // [G6·read_111 负结论 2026-09-11] packed 视图 + 引用绑定形（SaveQuestNotifyIdsView
        //   + `unsigned int m_ids[5]`）实测 61/62 gap−1 ndiff=5 → 61/63 gap−2 ndiff=13
        //   ⇒ 劣化（+1 条、残差翻倍），父子双证引用槽开销 > 索引序收益 ⇒ 保持原指针算术形。
        if (!packet.get_int(req.m_questIdx[i]))

        {
            return LineFunc(0x2bb,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
}