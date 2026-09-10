int Inter_LoadDimensionInout::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    bool changed = false;
    bool ultimateChanged = false;                      // ORIG -0x15 具名 bool（movb $0 初值 + 条件置 1 + 末尾 movzbl）

    if (user->get_state() == 0)                        // test eax; sete
        return 0x3ef3;

    // [G2A-PC 手法3/P324 语句顺序=物理序] ORIG 084d83f5 返回 0x3ef3 后才在 084d83ff 发
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`（pPkt 物化）；本方原在函数首行物化
    //   ⇒ 本 diff `insert OURS[4:6]` / `delete ORIG[15:17]`。守卫前移 pPkt 声明即可。
    Inter_LoadDimensionInoutPktView* pPkt = reinterpret_cast<Inter_LoadDimensionInoutPktView*>(data);

    // §P0：ORIG 对 pPkt->m_0 全程寄存器直传（无槽往返）⇒ 原码无 characNo 具名局部，三处内联。
    const int idx = user->get_charac_idx(pPkt->m_0);
    if (idx < 0)                                       // jns
        return 0x3ef9;

    if (user->m_characList[idx].m_characNo != pPkt->m_0)
        return 0x3efb;

    // §P183：ORIG 每处字段访问重发 vector<_Charac_info>::operator[]（8 处 out-of-line call，
    //   我方 recBase 缓存只发 2 处）⇒ 逐点重物化 `&user->m_characList[idx]`，不缓存基址。
    user->m_characList[idx].m_characNo = pPkt->m_0;
