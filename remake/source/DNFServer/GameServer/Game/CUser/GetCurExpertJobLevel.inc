int CUser::GetCurExpertJobLevel(int exp)
{
    if (getCurCharacR() == 0)
        return -1;
    if (GetCurCharacExpertJobType() == 0)
        return 0;
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
    // b28d-solo-CUser P171：ORIG 0x0868bc7c 无 script 判空（我方多 `cmpl $0,-0x1c; jne;
    //   mov $0,%eax; jmp` 段）⇒ 删防御照抄。
    char isBoundary = script->isBoundaryExpValue(exp);
    int level = script->GetLevel((unsigned int)exp);
    if (isBoundary != 0)
    {
        void* etc = G_CDataManager()->GetExpertJobEtcScript();
        // 推断：etc 对象首部为 std::map<uchar,short>（等级 → 所需角色等级）
        if (etc != 0)
        {
            std::map<unsigned char, short>* m =
                (std::map<unsigned char, short>*)etc;
            std::map<unsigned char, short>::iterator it =
                m->find((unsigned char)level);
            if (it != m->end() && it->second <= get_charac_level())
                return level;
        }
        level = level - 1;
    }
    return level;
