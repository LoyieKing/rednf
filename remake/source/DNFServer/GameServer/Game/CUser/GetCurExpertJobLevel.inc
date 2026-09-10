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
        // 2026-09-10（G3CUser-B，P171 删发明防御）：ORIG 0x0868bd14 `call G_CDataManager;
        //   mov %eax,(%esp); call GetExpertJobEtcScript; mov %eax,-0xc(%ebp)` →
        //   0x0868bd2a 起直接把 `-0xc(%ebp)` 当 this 传 `map::find`
        //   (0x0849fd46)，全程**无** `cmpl $0x0,-0xc(%ebp); je` 判空；Ghidra 反编译 C
        //   同一位置亦无判空 ⇒ 删我方 `if (etc != 0)` 发明防御。
        // 推断：etc 对象首部为 std::map<uchar,short>（等级 → 所需角色等级）
        std::map<unsigned char, short>* m =
            (std::map<unsigned char, short>*)G_CDataManager()->GetExpertJobEtcScript();
        std::map<unsigned char, short>::iterator it =
            m->find((unsigned char)level);
        if (it != m->end() && it->second <= get_charac_level())
            return level;
        level = level - 1;
    }
    return level;
