// 【P210】FindSkill(kind) 定义前置：调用者须在 get_skillslot_buf 定义之前，
// 否则 4.4 按 callee 帧精确继承 outgoing，0x28 保守大帧塌缩为 0x18。
int SkillSlot::FindSkill(int skillIdx, ENUM_SKILL_TREE_KIND kind) const
{
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index == skillIdx)
        {
            return buf[i].m_level;
        }
    }
    return 0;
}