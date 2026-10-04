// [b28d-lane2] verdict=DIFF ndiff=133（ORIG 0x084d83d2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+24。
// [BigParty] 判 **N89**（本 TU 唯一剩余物化缺口件，已定性别动）：ORIG 对
//   `std::vector<_Charac_info>::operator[](unsigned)` 发 **8 处 out-of-line call**（含
//   `mov 0xc(%ebp),%edx; add $0x796e8,%edx` 的逐次物化 8 处），本方仅 2 处 —— 但 4.4 的
//   头文件里 operator[] 恒内联，**无论怎么展开 recBase 都发不出该 out-of-line call**，
//   故非可攻缺口（Main 已把此形态并入 N89 条目）。TOT call 数 ORIG 18 / 本方 12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=146（ORIG 0x84d83d2）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_LoadDimensionInout::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    bool changed = false;
    bool ultimateChanged = false;                      // ORIG -0x15 具名 bool（movb $0 初值 + 条件置 1 + 末尾 movzbl）

    if (user->get_state() == 0)                        // test eax; sete
        return 0x3ef3;

    // [G2A-PC 手法3/P324 语句顺序=物理序] ORIG 084d83f5 返回 0x3ef3 后才在 084d83ff 发
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`（pPkt 物化）；本方原在函数首行物化
    //   ⇒ 本 diff `insert OURS[4:6]` / `delete ORIG[15:17]`。守卫前移 pPkt 声明即可。
    SIG_LOAD_DIMENSION* load =
        reinterpret_cast<SIG_LOAD_DIMENSION*>(data);

    // §P0：ORIG 对 load->m_field00 全程寄存器直传（无槽往返）⇒ 原码无 characNo 具名局部，三处内联。
    const int idx = user->get_charac_idx(load->m_field00);
    if (idx < 0)                                       // jns
        return 0x3ef9;

    if (user->m_characList[idx].m_characNo != load->m_field00)
        return 0x3efb;

    // §P183：ORIG 每处字段访问重发 vector<_Charac_info>::operator[]（8 处 out-of-line call，
    //   我方 recBase 缓存只发 2 处）⇒ 逐点重物化 `&user->m_characList[idx]`，不缓存基址。
    user->m_characList[idx].m_characNo = load->m_field00;

    for (int i = 0; i <= 5; ++i)                       // cmpl $5; setle
    {
        user->m_characList[idx].m_demensionInout[i] =  // +0xeb9 数组成员直存
            static_cast<unsigned char>(load->m_dimensionInout[i]);                      // 源 +0x4 stride-1
        // §P0：ORIG got 值留 %ebx 跨 G_CDataManager call 保活（无槽）⇒ 内联比较表达式。
        if (user->m_characList[idx].m_demensionInout[i] !=
            (unsigned char)G_CDataManager()->get_dimensionInout(i))   // [H2·laneD] ORIG `cmp %al,%bl`（8 位）⇒ 无符号
            changed = true;
    }

    user->m_characList[idx].m_ultimateInout[2] = static_cast<unsigned char>(load->m_limitInoutCount);   // +0xec0+2 ⇒ ORIG `mov %dl,0xec2(%eax)`
    if (user->m_characList[idx].m_ultimateInout[2] !=
        (unsigned char)G_CDataManager()->get_limit_inout_count(ENUM_BLOOD_TYPE_2))   // [H2·laneD] 同上
        ultimateChanged = true;
    user->m_characList[idx].m_saveDemensionInoutValue = 0;            // +0xeb8 ⇒ ORIG `movb $0x0,0xeb8(%eax)`
    user->m_characList[idx].m_saveBloodInoutValue = 0;            // +0xebf ⇒ ORIG `movb $0x0,0xebf(%eax)`

    // [LaneE 2026-09-11 P225 试形] ORIG 084d85b5/084d85d8 两处各发 `setne/setg %al;
    //   test %al,%al; je <共享出口>`（bool 物化后测试，无具名 bool 槽）⇒ 原码条件
    //   不是 `&&` 短路链（4.4 产 jle/jmp/mov$0/mov$1 岛 + 一次 jcc），而是
    //   `while (cond) { body; break; }` 逐层物化形（P225：CVillageAttacked/CDropItem 同族）。
    // [LaneE P225 实测] 单 while+&& 链退回 setne/setg 岛（ndiff 3）⇒ 维持逐层
    //   `while (cond) { …; break; }` 三嵌套形：ORIG 084d85b5/084d85d8 的
    //   `setne/setg %al; test %al,%al; je <共享出口>` 归位，唯余尾 2 条 jmp
    //   （ndiff 2→1；gap −1→−2）。备选形读数：单 while+&& = ndiff 3（已回退）。
    while (user->getCurCharacR() != 0)
    {
        while (user->IsFirstTimeLogin())
        {
            while (user->get_charac_level() > 0x3b)
            {
                user->DimensionInoutUpdate(changed, ultimateChanged);
                break;
            }
            break;
        }
        break;
    }
    return 0;
}