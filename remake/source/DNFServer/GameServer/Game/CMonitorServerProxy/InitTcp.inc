// [b28d-27] DIFF N80：ORIG 0x08470ad6；ORIG 在 cMyTrace ctor call 之前右起求
// varargs 并跨 call 保活 callee-saved（%edi/%esi），4.4 恒 ctor 后求（具名/临时/
// register 三探针不可翻，N80 实证 0x0847196d）⇒ 封顶。p167 CLEAR（9/9）。
// [M5 2026-09-11 复核] 追加两探针均无效（保持封顶）：
//  ① `({ ret; })` 语句表达式实参 ⇒ ret 仍留槽、仍只 push %esi/%ebx（ndiff 34 不变）；
//  ② 函数尾 dead `char pad[0x10]` 撑帧（0x50→0x60）⇒ 槽号未对齐（ORIG 0x5c 系
//     4.1 outgoing 边界，4.4 恒 16k 量化，N115）且 ndiff 34 不变。
//  另 ORIG `mov -0x1c(%ebp),%esi`（ret 提前保活）vs 我方 `mov -0xc(%ebp),%eax`
//  属 4.1 对纯 load 型 vararg 也提前物化、4.4 只提前有 call 的实参 ⇒ N80 本质。
bool CMonitorServerProxy::InitTcp()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (m_tcp.m_sock < 0)
    {
        int ret = m_tcp.CreateConnectionSocket(m_tcp.m_sock, m_ip.c_str(),
                                               m_tcpPort);
        while (ret != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x49b, 0)
                ("CMonitorServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4a0, 9, true, true)
        ("[!] Connect To Monitor Server ... ");
    return 1;
}