void CUser::SendTagCharacInfo()
{
    // ORIG 0x086903f8（SendTagCharacInfo.md）：getTagCharacR() 判空 + 取
    // m_field14BC 重复调用两次（P84 多结果判断同一局部，ORIG 不缓存），
    // 且判空为 `test;sete;test;jne` ==0 早退形。
    if (getTagCharacR() == 0)
        return;
    // ORIG O@0x2b..0x37：m_field14BC 真值取反 movzbl;xor$1;test;je（非
    // !=1 的 cmp;setne）。
    // [b28d-solo-CUserOnlyOrig P168a④/P248档②] 具名 uchar v + v^=1 令 4.4 落
    // -0x9 栈槽物化（xorb/cmpb）；ORIG 无槽往返 ⇒ 裸 `if(!field)` 内联形。
    if (!getTagCharacR()->m_field14BC)
    {
        getTagCharacW()->m_field14BC = 1;
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19e);
    packet.put_short(1);
    MakeTagCharacInfo(packet);
    packet.finalize(true);
    Send(packet);
