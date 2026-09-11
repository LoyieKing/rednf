// ---------------------------------------------------------------------------
// 5. void CUser::initTPSkill() @0x0868fdbc
void CUser::initTPSkill()
{
    // ORIG 0x0868fdbc：直接读 getCurCharacR()->m_field7b(+0x7b) 与
    // 0x500723d0 比较（setl），不经 getCurCharacLastPlayTick（NEW 曾用
    // <= 0x500723cf，等价但字节不同）。
    // P0：ORIG 有 threshold 槽（movl $0x500723d0,-0xc 预置）；SkillSlot 右值直传。
    int tpSkillLimit = (int)0x500723d0;
    if (getCurCharacR()->m_field7b < tpSkillLimit)
    {
        ((SkillSlot*)getCurCharacSkillW())->initialize_sfp_skill();
        send_skill_info();
    }
}