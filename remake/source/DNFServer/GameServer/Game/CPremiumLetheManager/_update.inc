void CPremiumLetheManager::_update(CUser* user, unsigned char state,
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
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
    // [b28d-solo 负结论 · 新口径 n1_v.py: AE 127/127 gap 0 ✓] ORIG 0x085c48f4
    // `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)`
    // （2 条形）⇒ ORIG 的 m_characNo 与 `getCurCharacNo()`（unsigned int）同型；
    // 头内声明为 int（SigTypes_E.h:959）时 4.4 恒插 `mov %eax,%edx` 值搬运
    // （our 128，gap −1，残余仅此 1 条）。已试并回滚：本 TU 内
    // `*reinterpret_cast<unsigned int*>(&result->m_characNo) = …` ⇒ our 129
    // （更差）；源侧 static_cast<int> 同族无效（P211，Arad_AvatarRoulette.cpp:88-91
    // 实证字段改 unsigned int 后逐条全等）⇒ 需改头，已上报父泳道。
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
}