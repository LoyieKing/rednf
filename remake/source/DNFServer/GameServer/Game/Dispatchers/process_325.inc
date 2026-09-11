// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081d9412 orig/ours=162/159 ndiff-blocks=32
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x18(%ebp) | mov    -0x18(%ebp),%edx | mov    -0x18(%ebp),%eax
int Dispatcher_ExpertExtraction::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stExpertExtraction& p = static_cast<stExpertExtraction&>(param);

    p.m_4 = check_error(user, msg);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x45bf, __PRETTY_FUNCTION__, p.m_4, 0);
    }

    MSG_EXPERT_EXTRACTION& m = reinterpret_cast<MSG_EXPERT_EXTRACTION&>(msg);
    // [G4-325 2026-09-11 · P11/P0] ORIG 0x081d9493 `movzbl 0xd(%eax),%eax; movsbl %al,%eax`
    //   ⇒ uchar 读 + 显式 (char) 符号扩展（类内 m_0d 为 unsigned char）；0x081d94a2
    //   `mov %eax,-0x10(%ebp)` ⇒ GetExtracterType 结果**具名槽**往返后再传 get_extracter。
    const int type = expert_extraction::GetExtracterType(
        (ENUM_EXPERT_JOB_TYPE)(char)m.m_0d);
    expert_extraction::CExtraction* ex =
        GlobalData::g_ExtractionMgr->get_extracter((unsigned int)type);
    if (ex == 0)
    {
        return LineFunc(0x45cb, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }

    std::vector<std::pair<int, int> > materials;
    materials.clear();
    if (!ex->extraction(user, (short)m.m_10, materials))
    {
        p.m_4 = 1;
        return 0;
    }

    std::vector<int> results;
    results.clear();
    if (!ex->inven_process(user, (short)m.m_10, materials, results))
    {
        p.m_4 = 0x11;
    }
    else
    {
        p.m_8 = (short)m.m_10;
        p.m_list = results;
    }
    return 0;
}