// [b28d-solo-EmblemCompoundServer] CheckCondition 判词（ORIG 0x08191636，124 insn）：
// IDENTICAL/AE。修法：grade0 具名 int 局部（ORIG 槽 -0xc，movzwl+cwtl）；money/need
// 去具名局部改内联（`get_money() < m_cost[grade0]`，P52 左值先物化 %ebx）；
// logFormat 的 money/need 逐实参重算（P157/P92）。
bool EmblemCompoundServer::CheckCondition(CUser* user,
                                          arad::SigAradEmblemCompound* sig)
{
    // [b23S3C] N：帧/寄存器分配差（ORIG push esi/sub 0x30 vs 0x34）+ grade
    // 有符号扩展比较（movswl/cwtl 物化）非本件可复现，语义等价已核（详见 b22_s3_verdicts.tsv）
    if (!GetEmblemInfo(user, sig, 0))
        return false;
    if (!GetEmblemInfo(user, sig, 1))
        return false;
    // ORIG +0x6c：grade0 提升为具名 int 局部（槽 -0xc），grade1 重读提升后比较。
    int grade0 = (int)sig->m_grade0;
    if (!((int)sig->m_grade1 == grade0 && grade0 < 7))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd8);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x1e8,
            "EMBLEM COMPOUND : [grade1:%d] [grade2:%d] [charac_no:%d]",
            (int)sig->m_grade0, (int)sig->m_grade1,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    // ORIG +0x10c：money 左值先求值保活 %ebx，m_cost[grade0] 右值直比（无 need/money 槽）。
    if (user->getCurCharacInvenR()->get_money() < m_cost[grade0])
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 10);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x1f0,
            "EMBLEM COMPOUND : [money:%d] [need:%d] [charac_no:%d]",
            user->getCurCharacInvenR()->get_money(), m_cost[grade0],
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    return true;
}