// ----------------------------------------------------------------------------
// Secu_DataControl::SetControlData(CUser* user, uchar type, ushort v1, ushort v2)
// ORIG @ 0x08286a30（报告 Secu_DataControl/SetControlData.md 全文转写）。
// 四分支（type 0..3），type==1/2/3 均要求 user->isGMUser()：
//   type 0（Hacktype 注入）：100 < v1 <= 0xfa0 且 v2 < 10 时 trace
//     "[SecuCommand] (mid:%s) Hacktype : %d     Cnt : %d "（line 0x1e）并循环
//     v2 次 getHackAnalyzer()->addServerHackCnt(user, v1, 1, 0, 0)；
//     结束后（无论条件）addServerHackCnt(user, 0x262, 1, 0, 0)。
//   type 1（CleanPad 点数）：v = clamp(v1, 0, 10000)（max/min int）；
//     trace "…CleanPad Set Point : %d"（line 0x32）；
//     m_mcap.setPoint((ushort)v)；若 getPoint() > 1000 →
//     setStdDropRate(100 - getPoint()/100)。
//   type 2（CleanPad Open）：trace "…CleanPad Open!"（line 0x40）；
//     reqHumanCertify4ClearMap(false)。
//   type 3（DropRate）：v = clamp(v1, 0, 100)；trace "…DropRate : %d"
//     （line 0x4a）；setStdDropRate(v)。
// cMyTrace 构造 (name=__PRETTY_FUNCTION__, line, 0)；mid = NumberToString(
// get_acc_id(), 0)。
// ----------------------------------------------------------------------------
// [SweepDBSecuLink] ORIG 0x08286a30：250/250 AE（唯一残差 = 我方循环出口对齐 nop，
//   §6.1 豁免）。三处形态修正：
//   ① hackType/cnt/i unsigned→int：ORIG 0x08286a9e/0x08286aaf/0x08286ab9 为
//      jle/jg、0x08286b6d 尾测 setl（有符号）⇒ unsigned 生 jbe/ja/setb（P196/P2）；
//   ② trace 改全表达式临时链 cMyTrace(...)(fmt, NumberToString(...), ...)：
//      ORIG 0x08286ada 把 NumberToString 结果存 %ebx 跨 ctor 保活、并翻转四个
//      互斥 case 的 trace 对象槽方向（P227/P221；具名 mid 局部槽化且丢 push %ebx）；
//   ③ clamp 改单表达式 std::min(std::max((int)v1, 0), N)，getPoint 两次调用
//      内联、去 static_cast<int>：ORIG 0x08286cc3 为单段 shr/sub 无 regmove（P0/P41）。
void Secu_DataControl::SetControlData(CUser* user, unsigned char type,
                                      unsigned short v1, unsigned short v2)
{
    if (user == 0)
        return;

    switch (type)
    {
    case 0:  // Hacktype 注入
    {
        // ORIG 0x08286a9e/0x08286aaf/0x08286ab9 为 jle/jg（有符号）⇒ 局部 int，
        // 非 unsigned（P196/P2：jcc 逐位判据）。
        int hackType = static_cast<int>(v1);
        int cnt = static_cast<int>(v2);

        if (hackType > 100 && hackType <= 0xfa0 && cnt < 10)
        {
            // ORIG 0x08286ad5..0x08286afa：NumberToString 结果存 %ebx 跨 cMyTrace
            // ctor 保活（P227 全表达式临时链；具名 mid 局部会槽化并丢 push %ebx）。
            cMyTrace(__PRETTY_FUNCTION__, 0x1e, 0)(
                "[SecuCommand] (mid:%s) Hacktype : %d     Cnt : %d ",
                NumberToString(user->get_acc_id(), 0), hackType, cnt);

            // ORIG 0x08286b6d 尾测 setl（有符号）⇒ 循环变量 int。
            for (int i = 0; i < cnt; ++i)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1, 0, 0);
            }
        }

        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x262), 1, 0, 0);
        break;
    }

    case 1:  // CleanPad Set Point（仅 GM）
    {
        if (!user->isGMUser())
            break;

        // ORIG 0x08286bcc..0x08286c04：单表达式 clamp —— 临时槽 -0x64(10000)/
        // -0x60(0)/-0x5c((int)v1) 按 4.1 右→左求值创建，结果落独立槽 -0x10；
        // 两条赋值语句形会让 4.4 写回同一槽并丢临时槽（P211/P0）。
        int v = std::min(std::max(static_cast<int>(v1), 0), 10000);

        cMyTrace(__PRETTY_FUNCTION__, 0x32, 0)(
            "[SecuCommand] (mid:%s) CleanPad Set Point : %d",
            NumberToString(user->get_acc_id(), 0), v);

        user->m_mcap.setPoint(static_cast<unsigned short>(v));

        // ORIG 0x08286c8b/0x08286cab 两次 getPoint（无具名槽、寄存器直比 seta）⇒
        // 表达式内联；`static_cast<int>` 会引入 4 条多余 regmove（ORIG 0x08286cc3
        // `shr $0x5,%edx; mov $0x64,%eax; sub %edx,%eax`）。
        if (user->m_mcap.getPoint() > 1000)
            user->setStdDropRate(100 - user->m_mcap.getPoint() / 100);
        break;
    }

    case 2:  // CleanPad Open（仅 GM）
    {
        if (!user->isGMUser())
            break;

        cMyTrace(__PRETTY_FUNCTION__, 0x40, 0)(
            "[SecuCommand] (mid:%s) CleanPad Open!",
            NumberToString(user->get_acc_id(), 0));

        user->reqHumanCertify4ClearMap(false);
        break;
    }

    case 3:  // DropRate（仅 GM）
    {
        if (!user->isGMUser())
            break;

        // ORIG 0x08286d7c..0x08286db4：同 case 1 的单表达式 clamp（上界 100，
        // 结果落独立槽 -0xc）。
        int v = std::min(std::max(static_cast<int>(v1), 0), 100);

        cMyTrace(__PRETTY_FUNCTION__, 0x4a, 0)(
            "[SecuCommand] (mid:%s) DropRate : %d",
            NumberToString(user->get_acc_id(), 0), v);

        user->setStdDropRate(v);
        break;
    }

    default:
        break;
    }
}