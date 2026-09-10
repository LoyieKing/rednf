bool CSharedServerMessageManager::IsChangableMessage(int nIndex, int nTime)
{
    // [b28d-solo-CSharedServerMessageManager 判词 · AE] ORIG 0x08600cc8：fresh 复验 TU ae（23=23），
    // 清单 NEAR(2) 为 .o 重定位标签伪差；语义/控制流与 ORIG 全等。
    // b28c-L20: AE — fresh 复验 orig=23 our=23 ndiff=0（归一口径）。槽往返仅 pMsg 一处
    // （ORIG -0xc(%ebp) `mov %eax,-0xc(%ebp); cmpl $0x0,-0xc(%ebp)`）；pMsg->m_endTime(+0x54)
    // +0x78 与 nTime 的有符号 `jl` 走 RHS 值先物化（P0/P85），get_channel_no 类返回宽度无关。
    stSERVER_MESSAGE* pMsg = getCurMessage(nIndex);
    if (pMsg == 0)
        return true;
    // ORIG 0x…：`add $0x78,%eax; cmp; jl <RET1>`（`m_endTime+0x78 < nTime` 时
    // je 直跳 return-true 岛，return-false 为 fall-through+jmp over）——用 goto
    // 落点岛把 return true 放 jump 目标（P133/P16；普通 if(cond)return true 被
    // 4.4 折成反向 jge 跳 return-false 岛）。
    if (pMsg->m_endTime + 0x78 < nTime)
        goto ret_true;
    return false;
ret_true:
    return true;
