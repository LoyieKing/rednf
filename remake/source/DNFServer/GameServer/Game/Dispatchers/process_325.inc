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
    expert_extraction::CExtraction* ex = GlobalData::g_ExtractionMgr->get_extracter(
        expert_extraction::GetExtracterType((ENUM_EXPERT_JOB_TYPE)m.m_0d));
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
