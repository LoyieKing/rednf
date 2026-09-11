void CSharedServerMessageManager::SetServerMessage(SIG_EVENT_SERVER_MESSAGE* pMsg)
{
    // b28c-L20: AE — ndiff 3→0（fresh orig=59 our=59）。P0 清绑（删 `register unsigned char
    // set asm("eax")`）后的正解是**字段 bool 化**（CSharedServerMessageManager.h: m_isSet
    // unsigned char→bool，+0x04/sizeof 0x18 不变），配 P168a① 裸形 `if (!m_isSet)` 即逐条复现
    // ORIG 0x08600b83..0x08600b8c 的 `movzbl 0x4(%eax),%eax; xor $0x1,%eax; test %al,%al; je`，
    // 无需任何寄存器绑定。清绑过程中的实测负例（供各 lane 避坑）：uchar 字段下
    // `(m_isSet^1)!=0` 与 P226 `({m_isSet^1;})!=0` 均被前端折成 `cmp $0x1,%al`（ndiff 3）；
    // 具名 uchar 局部 `v^=1`/`v=v^1` 恒槽化（ndiff 5）；裸 `register uchar set`（无 asm 绑定）
    // 反劣最剧（ndiff 13：4.4 强占 %ebx ⇒ `push %ebx; sub $0x24` + pop 尾，帧/epilogue 全破）。
    // 全消费段 fresh 联判（P211 陷阱法）：IsSetServerMessage 6/6、setLoadingComplete 6/6、
    // C1Ev 42/42、C1EP25 47/47、D1Ev 27/27 均 ndiff=0（IsSetServerMessage 返回类型按 P9
    // 保持 unsigned char，ORIG 0x084ed182 W sz 0xc 为 movzbl 直返无归一）。
    if (!m_isSet)
    {
        if (pMsg->m_count == 0)
            return;

        m_event.m_count = pMsg->m_count;
        for (int i = 0; i < pMsg->m_count; ++i)
        {
            if (m_adapter.IsOnIndex(pMsg->m_msg[i].m_index))
                m_event.m_vecMsg.push_back(pMsg->m_msg[i]);
        }
        m_event.m_count = (unsigned char)m_event.m_vecMsg.size();
    }
    setLoadingComplete();
}