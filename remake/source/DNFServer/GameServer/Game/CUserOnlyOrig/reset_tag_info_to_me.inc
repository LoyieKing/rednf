// ----------------------------------------------------------------------------
// ORIG 0x0864f930 CUser::reset_tag_info_to_me(unsigned int tagAccId) void
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0864f930：调用点/形态已对齐（本波修法见函数内注释）。
void CUser::reset_tag_info_to_me(unsigned int tagAccId)
{
    // ORIG 0x0864f930：touched byte 槽 -0x9；vector base 每使用点 `lea 0x796e8(%eax)`
    // 重算（删 chars 引用中转槽，技巧36）；for-init 声明 it ⇒ begin sret 直落
    // it 槽 -0x14（无 tmp→it 拷贝）；end tmp -0x10
    bool touched = false;
    for (std::vector<_Charac_info>::iterator it = m_characList.begin();
         it != m_characList.end(); ++it) {
        // [b28d-solo-CUserOnlyOrig P167] ORIG 0x0864f95x 段 4 处命中
        // `__normal_iterator<...>::operator->()`；旧写法 `(*it).m` 生 operator*() ⇒
        // 按 ORIG 改 `it->m`（MISSING operator-> ×4 / EXTRA operator* ×4 归零）。
        if (it->m_tagCharacNo == tagAccId) {                                   // +0x14b8
            it->m_tagCharacNo = 0;
            it->m_strikerSkillIndex = 0;                                    // +0x14b7
            it->m_tagCharacInfoSent = 0;                                    // +0x14bc viewDirty（2026-09-04 已具名）
            touched = true;
        }
    }
    if (touched)
        m_characterView.enableSaveCharacView();                          // +0x796f4
}