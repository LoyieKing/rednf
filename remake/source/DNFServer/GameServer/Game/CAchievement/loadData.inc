// [b28d-solo-CAchievement AE] 真语义 bug 修复：`_verifyAchievement` 由 static
// 改非 static（CAchievement.h）后本函数调用点压 this + in->m_saveTime 两实参，
// 与 ORIG 0x0828c956 一致（旧 static 版把 m_saveTime 当 this 解引用 = UB）。
bool CAchievement::loadData(CUser* pUser, char* pData)
{
    stAchievement_DBData* in = (stAchievement_DBData*)pData;
    for (int i = 0; i < in->m_count; ++i)
    {
        m_achievements.insert(
            std::make_pair(in->m_entries[i].m_id, in->m_entries[i]));
    }
    m_user = pUser;
    // [b24F] DONE：_verifyAchievement 参数 (unsigned int)this→in->m_saveTime
    //（ORIG 0x828c956 mov -0x10(%ebp);mov 0x1804(%eax),%eax 传 in->m_saveTime）。
    _verifyAchievement(in->m_saveTime);   // b28c-L10：m_saveTime 已 unsigned（cast 冗余）
    return 1;
}