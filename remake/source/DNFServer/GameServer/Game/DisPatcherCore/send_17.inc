void DisPatcher_UseSkill::send(CUser* user, ParamBase& param)

{
    // ORIG 0x081caf28：指针局部落栈 -0x4 → 取 +0x4 m_errNo → cmp $0x7fffffff
    // 后 `return`（then==merge，jcc 被 tail-merge 清理 → dead cmp 残留）。
    stUseSkillParam* pParam = static_cast<stUseSkillParam*>(&param);
    if (pParam->m_errNo == 0x7fffffff)
        return;
}
