void CPremiumLetheManager::_update(CUser* user, unsigned char state,
                                   ENUM_SKILL_TREE_KIND kind)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PremiumLetheManager.cpp",
                                                          0x125),
                       true);
    **guard << 0x15c;
    // (int)：CUser::GetUID() 返回 unsigned int（CUser.h:2910），不加转换会选中
    // operator<<(unsigned int)；ORIG 0x085c48b2 调用点为 operator<<(int)
    // （P167 调用点差集实证 ORIG lsEi×2 / 修复前 lsEi+lsEj）。
    **guard << (int)user->GetUID();
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* result =
        guard->GetInBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();
    result->m_accID = user->get_acc_id();
    result->m_characNo = user->getCurCharacNo();
    if (state == USE_LETHE_STATE || state == SKILL_CONFIRM_STATE ||
        INIT_LETHE_SKILL != 0)
    {
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
        {
            memcpy(result->m_skills, user->getCurCharacSkillR() + 0x46, 0x198);
        }
        else
        {
            memcpy(result->m_skills, user->getCurCharacSkillR() + 0x1de, 0x198);
        }
    }
    else
    {
        memset(result->m_skills, 0, 0x198);
    }
    result->m_state = state;
    result->m_state2 = state;
    result->m_treeKind = (unsigned char)kind;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
