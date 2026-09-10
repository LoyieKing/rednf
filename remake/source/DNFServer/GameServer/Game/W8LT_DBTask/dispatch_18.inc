bool DB_UpdateOneDayLetheSkill::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* out =
        stream->GetOutBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();
    int flag = 1;
    if (out->m_state == CPremiumLetheManager::NOT_USE_LETHE_STATE)
    {
        if (!ProcessInitLethe(out))
        {
            flag = 0;
        }
    }
    else if (out->m_state == CPremiumLetheManager::USE_LETHE_STATE)
    {
        if (!ProcessUseLethe(out))
        {
            flag = 0;
        }
    }
    else if (out->m_state == CPremiumLetheManager::SKILL_CONFIRM_STATE)
    {
        if (!ProcessSkillConfirm(out, slot))
        {
            flag = 0;
        }
    }
    else if (out->m_state == CPremiumLetheManager::INIT_LETHE_SKILL)
    {
        if (!ProcessInitLetheSkill(out))
        {
            flag = 0;
        }
        out->m_state = CPremiumLetheManager::INIT_LETHE_SKILL;
        out->m_state2 = CPremiumLetheManager::USE_LETHE_STATE;
    }
    else
    {
        return 0;
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7cac), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    SIG_PREMIUM_LETHE_FLAG* p = guard->GetInBuffer<SIG_PREMIUM_LETHE_FLAG>();
    p->m_flag = flag;
    p->m_state = out->m_state;
    p->m_treeKind = out->m_treeKind;
    memcpy(p->m_skills, out->m_skills, 0x199);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
