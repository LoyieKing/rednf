// [b28d-lane2] verdict=DIFF ndiff=94（ORIG 0x084e7884）：槽序/帧差(N82/P212/P221)+混合残差；gap+10。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=40（ORIG 0x084e7884）：本波已消 p167
// MISSING vector<_Charac_info>::operator[] ×2（成员引用绑定使写+读共用一次 op[]、
// 3 处 slot 使用逐点重读）⇒ P167 CLEAR 11=11；残余 N18a/N82 结构差。
int Inter_RequestCharacSkillInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* recv = data;                                 // -0x14(%ebp) 缓存消息指针
    if (user == 0)
        return 0;
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    const int tagIdx =
        user->get_charac_idx(reinterpret_cast<Inter_RequestCharacSkillInfoPktView*>(recv)->m_0);
    if (tagIdx == -1)                                  // cmpl $0xffffffff
        return 0;

    // ORIG 0x084e78fa：写 m_field1239 与随后的读共用**同一次** operator[]
    // （`mov %dl,0x1239(%eax); movzbl 0x1239(%eax),%eax`）⇒ 原码是成员引用绑定。
    unsigned char& field1239 = user->m_characList[tagIdx].m_field1239;
    field1239 = reinterpret_cast<Inter_RequestCharacSkillInfoPktView*>(recv)->m_19c;                           // ORIG movzbl 0x19c; mov %dl
    const char skillTreeKind = (char)field1239;        // ORIG movzbl 0x1239(%eax)

    // [b28d-solo-InterDispatcher] P157 逐点重读：ORIG 0x084e78fa/0x084e792c/0x084e7959
    // （+1）每个使用点重新 operator[] 取 _Charac_info（无 slot 指针缓存）⇒ 消除 p167
    // MISSING vector<_Charac_info>::operator[] ×2。SkillSlot 子对象 @ +0x892。
    reinterpret_cast<SkillSlot*>(
        reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892)->set_parent(user);

    _PutSkillInfoToCharac(skillTreeKind,
                          reinterpret_cast<_Mastered_skill*>(recv + 4),
                          reinterpret_cast<SkillSlot*>(
                              reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892));

    reinterpret_cast<SkillSlot*>(
        reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892)
        ->SendSkillInfo(tagIdx,
                        (ENUM_SKILL_TREE_KIND)(int)skillTreeKind);  // movsbl 符号扩展
    reinterpret_cast<SkillSlot*>(
        reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892)
        ->SetLoadStrikerSkill(true);
    return 0;
}