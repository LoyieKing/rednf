// [b28d-62] 修：3 处 memcpy→直读/结构赋值（P167/N84），去 memcpy call 与
//   多余 ctor。ORIG 0x085f7830
// [b28d-63 本 TU] P0/P48/N84：ORIG 0x085f7848 把 pData 拷入具名局部 -0x10；
//   count 每轮重读（无局部）、check/entry 免槽、push_back 直接传 pData 内地址
//   （无 info 临时对象）；i 为 unsigned（cond `cmp;seta`）。原形多 4 局部 + 0x44
//   临时 + 3 callee-saved push（帧 0x7c vs ORIG 0x28）。
// [b28d-63 判词] 帧/指令数已与 ORIG 对齐（0x28；check 段 7=7、else 段 11=11），
//   verdict 仍 DIFF，残差全为后端形态：①ORIG 0x085f7857 提前载 buf 到 %edx 用
//   `mov 0xa(%eax,%edx,1)`，4.4 恒 `add -0x10(%ebp),%eax; mov 0xa(%eax)`；
//   ②ORIG 0x085f788c `lea 0x8(%eax),%edx`，4.4 恒 `add $0x8,%eax`（N102/N4 族，
//   union 视图/指针算术/数组下标三形均不翻）。gap≤0，非缺段。
// [N1D 2026-09-11 · n1_v 口径] 真残差三段（本波全部试毕，判后端封顶）：
//   ① 0x085f783e 虚调 `_reset()`：ORIG `mov (%eax),%edx … call *%edx` vs 我方
//      `mov (%eax),%eax … call *%eax`（vtable slot 落 %edx vs %eax）⇒ N7；
//   ② 0x085f7857 check 段：ORIG 提前把 buf 载入 %edx 并以 `mov 0xa(%eax,%edx,1)`
//      一次寻址，4.4 恒 `add -0x10(%ebp),%eax; mov 0xa(%eax)` ⇒ N102/N4；
//   ③ 0x085f788c else 段：ORIG `lea 0x8(%eax),%edx`（源地址 → %edx）vs 我方
//      `add $0x8,%eax`（源 → %eax、this → %edx，两寄存器互换）⇒ N102/N4。
//   本波试形·负结论：`&buf[i].m_body`（取成员地址以逼出 `lea`）**不可用** ——
//   RedeemEntryView 是 **union**，m_body 偏移为 **0**（非 +8）⇒ 源地址少 8 字节
//   （语义错，改后 our 50→48 的假改善即此），已回滚；正解只能经 `m_body.m_info`
//   （Body 内 +8），4.4 仍折叠为 add。
bool CRedeemItem::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();   // ORIG 0x085f7846：虚调 vtable slot7
    const RedeemEntryView* buf =
        reinterpret_cast<const RedeemEntryView*>(pData);   // ORIG 0x085f7848
    for (unsigned int i = 0; i < ((const RedeemListHeadView*)buf)->m_count; ++i)
    {
        if (buf[i].m_head.m_check == 0)   // ORIG 0x085f7867 mov 0xa(%eax,%edx,1)
        {
            alter();   // ORIG 0x085f7875: charac_expand::CData::alter()
        }
        else
        {
            // ORIG 0x085f788c：lea 0x8(%eax),%edx 直传源地址（无临时对象）。
            // [N1D 2026-09-11 试形·负结论] `&buf[i].m_body` 不可用：RedeemEntryView 是
            //   **union**，m_body 偏移为 0（非 +8）⇒ 源地址少 8 字节，语义错；已回滚。
            //   正解只能经 m_body.m_info（Body 内 +8）。4.4 仍折叠为 `add $0x8,%eax`
            //   （ORIG 0x085f788c 为 `lea 0x8(%eax),%edx`）⇒ N102/N4 族，源码不可控。
            // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N102/N4）→ 试=①prett2 缺口 0、
            //   strall 串差 0（仅 base64 假阳性）、p167 本函数 MISSING/EXTRA CLEAR；
            //   ②本波复核两处残差：ORIG 0x085f7857 `mov 0xa(%eax,%edx,1)`（基址提前
            //   入 %edx + indexed 寻址）vs 4.4 `add -0x10(%ebp),%eax; mov 0xa(%eax)`；
            //   同名 0x085f788c `lea 0x8(%eax),%edx` vs `add $0x8,%eax`（视图/union/
            //   指针算术三形 + m_body 偏移 0 陷阱历史已试）⇒ 维持（新=DIFF 49/50
            //   gap=-1, blocks=2：两处均为 4.1 基址/indexed 寻址折叠差）。
            m_redeemList.push_back(
                *reinterpret_cast<const Inven_Item_Expand_RedeemInfo*>(
                    buf[i].m_body.m_info));
        }
    }
    return true;
}