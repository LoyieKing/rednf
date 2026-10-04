// ---------------------------------------------------------------------------
// CUser::SendTagCharacInfo() T 0x086903f8 sz0xf8
// 报告：docs/class_func_reports/CUser/SendTagCharacInfo.md
// getTagCharacR()!=0：byte@+0x14bc 非 1 则 W 侧置 1；clear→put_header(0,0x19e)
// →put_short(1)→MakeTagCharacInfo→finalize→Send。
// （已改善 ndiff 39→3：m_tagCharacInfoSent 判断改重复调用 getTagCharacR() 不缓存
// + ==0 早退形；残差为 `xor $0x1`（ORIG gcc4.4）vs `cmp $1;setne`（本工具链）
// 跨版本 ISel 单条，见报告）
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086903f8：P248 档②（m_tagCharacInfoSent char→bool，Main
// 代落 CUser.h:719）+ 裸 `if(!field)` 内联形归零（原具名 uchar v + v^=1 落 -0x9 槽）。
void CUser::SendTagCharacInfo()
{
    // ORIG 0x086903f8（SendTagCharacInfo.md）：getTagCharacR() 判空 + 取
    // m_tagCharacInfoSent 重复调用两次（P84 多结果判断同一局部，ORIG 不缓存），
    // 且判空为 `test;sete;test;jne` ==0 早退形。
    if (getTagCharacR() == 0)
        return;
    // ORIG O@0x2b..0x37：m_tagCharacInfoSent 真值取反 movzbl;xor$1;test;je（非
    // !=1 的 cmp;setne）。
    // [b28d-solo-CUserOnlyOrig P168a④/P248档②] 具名 uchar v + v^=1 令 4.4 落
    // -0x9 栈槽物化（xorb/cmpb）；ORIG 无槽往返 ⇒ 裸 `if(!field)` 内联形。
    if (!getTagCharacR()->m_tagCharacInfoSent)
    {
        getTagCharacW()->m_tagCharacInfoSent = 1;
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19e);
    packet.put_short(1);
    MakeTagCharacInfo(packet);
    packet.finalize(true);
    Send(packet);
}