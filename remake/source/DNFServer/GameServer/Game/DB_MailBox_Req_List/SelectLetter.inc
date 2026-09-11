//   清 SIG_MAILBOX_LIST 信件区（+0x101c 起 0x230a）→ letterIdx==0 时先删过期
//   stat=2/1 邮件 → 按 (charac_no, stat=3) or (charac_no, reg_date 15 天内)
//   [letterIdx>0 时加 letter_id>idx] 查 letter 表 → 逐行按 stat 分流到三个 vector
//   （stat==3 单独；stat 1/2 按 IsContainedLetter 分流）→ ArrangeMailAndLetterIndex
//   整理 → 限额拷回 +0x101c 区（stride 0x12b）→ 写 +0x332a/+0x798/+0x1018。
// ============================================================================
// [b28d-solo-DB_MailBox_Req_List] verdict=DIFF（ORIG 513 vs 554），§6.2 超大函数
// （orig_insns>500）三件套：①p167 调用点差集 **CLEAR 34=34**；②gap = -41（我方大）；
// ③帧 ORIG sub $0x1cc vs 我方 sub $0x1ec（+0x20 槽组平移）。残余为 N82/N95
//   [F2d-C05② 甄别] 本轮实测（show_func_diff fresh .o）orig=511 our=544 gap=-33 ndiff=357
//   （旧读数 -41 作废）；全 ORIG mem 直比点（`cmp -0x38/-0x40(%ebp),%eax`）我方同为
//   mem 直比（同形）⇒ 无 C05② 命中形；多出 = OURS-only{mov×11,lea×11,add×4,movzwl×3,…}
//   （具名局部缓存 + SIG 宏裸偏移 add/lea）；另含符号扩展点（ORIG cwtl vs 我方 movzwl×3，
//   建议后续按 P196 复核）。无 F2 落点。
// 帧-槽号族 + SIG 宏 offset 形（本轮已修 exec 校验为 ORIG 的
// `movzbl; mov slot; cmpl; jne` 形）。语义完成、byte 层 N 封顶。
bool DB_MailBox_Req_List::SelectLetter(MySQL* db, int slot, int characNo,
                                       unsigned int letterIdx,
                                       SIG_MAILBOX_LIST* out)
{
    (void)slot;
    memset(SIG_PTR(out, 0x101c), 0, 0x230a);
    // [b28d-solo-DB_MailBox_Req_List] ORIG 0x0841d5a8 三处 exec 校验为
    // `call exec; movzbl %al,%eax; mov %eax,-0x40(%ebp); cmpl $0x0,-0x40(%ebp);
    // jne` ⇒ 源形是「int 局部接收 + `== 0` 早返」（不是 `!(unsigned char)exec()`
    // 的 xor 形，探针 /tmp/p2/ex.cpp g1/g2）。
    int bExec;   // ORIG 无前置清零（首次写入在 exec 调用后）

    if (letterIdx == 0)
    {
        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 2);
        bExec = db->exec(true);
        if (bExec == 0)
            return 0;

        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 1);
        bExec = db->exec(true);
        if (bExec == 0)
            return 0;

        db->set_query(
            "seLect letter_id,send_charac_no,send_charac_name,letter_text,unix_timestamp(reg_date),stat from letter where (charac_no=%d and stat = 3) or (charac_no=%d and reg_date>subdate(now(),interval %d day)) order by letter_id",
            characNo, characNo, 0xf);
    }
    else
    {
        db->set_query(
            "seLect letter_id,send_charac_no,send_charac_name,letter_text,unix_timestamp(reg_date),stat from letter where (charac_no=%d and stat = 3) or (charac_no=%d and letter_id > %d and reg_date>subdate(now(),interval %d day)) order by letter_id",
            characNo, characNo, letterIdx, 0xf);
    }

    bExec = db->exec(true);
    if (bExec == 0)
        return 0;

    int oldCount = out->m_box.m_count;   // +0x794
    int oldLetter = SIG_I32(out, SIG_OFF_LETTER_COUNT);  // +0x1018
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = 0;

    int letterCount = db->get_n_rows();
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = letterCount;

    // 三个分流 vector：vec[0] 已含（IsContainedLetter）、vec[1] 新（stat 1/2）、
    // vec[2] stat==3；局部 SIG_LETTER_INFO 缓冲按 ORIG 0x12b。
    std::vector<SIG_LETTER_INFO> vec[3];
    int groupSize[3] = { 0, 0, 0 };

    int i;
    for (i = 0; i < letterCount; ++i)
    {
        if (letterIdx != 0 && oldCount >= oldLetter &&
            out->m_box.m_remain != 0 &&
            (int)vec[0].size() >= oldLetter)
            break;

        if (!db->fetch())
            return 0;

        // [M5F_CtorDB] P329/P183：ORIG 0x0841d795..0x0841d900 六次 get_* 直接把
        // **结构缓冲字段地址**当实参（`lea -0x1a3(%ebp),%eax; add $0xNN,%eax;
        // mov %eax,0x8(%esp)`），无具名中间量、更无回写段。原写法 4 个具名局部
        // 接收再逐条 SIG 回写 ⇒ 多 3×movl+movw 清零、4×(lea+mov) 回写与缓冲
        // 中转（我方 546 vs ORIG 513 的主体）。表达式内联回实参位即复现。
        SIG_LETTER_INFO infoRaw;
        memset(&infoRaw, 0, 0x12b);

        if (!db->get_uint(0, SIG_U32(&infoRaw, 0x00))) return 0;
        if (!db->get_int(1, SIG_I32(&infoRaw, 0x04))) return 0;
        if (!db->get_str(2, SIG_PTR(&infoRaw, 0x08), 0x1d)) return 0;
        if (!db->get_str(3, SIG_PTR(&infoRaw, 0x25), 0x100)) return 0;
        if (!db->get_int(4, SIG_I32(&infoRaw, 0x125))) return 0;
        if (!db->get_short(5, *(short*)SIG_PTR(&infoRaw, 0x129))) return 0;

        // ORIG 0x0841d900 `movzwl -0x7a(%ebp),%eax; cwtl; cmp $0x1,%eax; jl`
        // vs 4.4 16 位直比（`test %ax,%ax` / `cmp $0x2,%ax`）。stat 不再落独立
        // 栈槽：三处比较各直读缓冲 +0x129（`movzwl -0xNN(%ebp)`），(int) 提升
        // 触发 cwtl 方向；具名 short 局部会是 store/load 中转而非 ORIG 形。
        if ((int)*(short*)SIG_PTR(&infoRaw, 0x129) > 0)
        {
            if ((int)*(short*)SIG_PTR(&infoRaw, 0x129) < 3)
            {
                // ORIG 0x0841d916：`call IsContainedLetter; test %al,%al;
                // je <vec[1]>` —— 正向 if（al!=0 落 vec[0]），无 `xor $0x1`。
                if (IsContainedLetter(out, SIG_I32(&infoRaw, 0x00)))
                    vec[0].push_back(infoRaw);
                else
                    vec[1].push_back(infoRaw);
            }
            else if ((int)*(short*)SIG_PTR(&infoRaw, 0x129) == 3)
            {
                vec[2].push_back(infoRaw);
            }
        }
    }

    int n = (int)vec[1].size() + (int)vec[0].size() - SIG_I32(out, SIG_OFF_LETTER_USED);

    if (n > 0)
    {
        ArrangeMailAndLetterIndex(out, vec[1], n, vec[0]);
        if (out->m_box.m_count != 0)
        {
            // ORIG 0x0841d9f5 `lea -0x1(%eax),%edx; mov this,%eax;
            // imul $0x61,%edx,%edx; mov (%edx,%eax,1),%edx` —— 具名 postal 数组
            // 视图访问（SIG_U32 指针算术版会多发一条 `lea` 把地址落 eax）。
            out->m_box.m_maxPostalId =
                (unsigned int)out->m_box.m_postals[out->m_box.m_count - 1].m_postalId;
        }
    }

    int adjust = 0;
    if (SIG_I32(out, SIG_OFF_LETTER_COUNT) != 0)
    {
        int k;
        for (k = 0; k < 3; ++k)
            groupSize[k] = (int)vec[k].size();

        if (SIG_I32(out, SIG_OFF_LETTER_USED) < groupSize[0])
            groupSize[0] = SIG_I32(out, SIG_OFF_LETTER_USED);
        if (SIG_I32(out, SIG_OFF_LETTER_USED) <
            out->m_box.m_count + groupSize[1])
        {
            adjust = (out->m_box.m_count + groupSize[1]) -
                     SIG_I32(out, SIG_OFF_LETTER_USED);
            groupSize[1] -= adjust;
        }
        if (groupSize[2] > 10)
            groupSize[2] = 10;
    }

    int outIdx = 0;
    int k;
    for (k = 0; k < 3; ++k)
    {
        unsigned int j;
        for (j = 0; (int)j < groupSize[k]; ++j)
        {
            *(SIG_LETTER_INFO*)SIG_PTR(out, 0x101c + outIdx * 0x12b) = vec[k].at(j);
            ++outIdx;
        }
    }

    SIG_I32(out, SIG_OFF_LETTER_NEW) = groupSize[1];
    // ORIG 0x0841dc0c：`mov 0x798(%eax),%ebx; … call _ZSt3maxIiERKT_S2_S2_;
    // mov (%eax),%eax; lea (%ebx,%eax,1),%edx; mov %edx,0x798(%eax)` —— 无具名
    // remain 中转（我方原多一条 store+load）。
    out->m_box.m_remain = out->m_box.m_remain +
                          std::max(adjust - SIG_I32(out, SIG_OFF_LETTER_ADD), 0);
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = outIdx;
    return 1;
}