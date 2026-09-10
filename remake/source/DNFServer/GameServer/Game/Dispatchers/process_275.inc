int Dispatcher_Skill_Command_All_Default::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081f4??：param→-0x10、msg 死槽→-0xc（声明序）；
    // !defaultSkillCommand()（xor $1 技巧1）→ out.m_4=0xfc; return 0；尾 return 0 双岛。
    stSkillCommandParam* out = reinterpret_cast<stSkillCommandParam*>(&param);
    MSG_BASE* pm = &msg;
    (void)pm;
    if (!user->defaultSkillCommand())
    {
        out->m_4 = 0xfc;
        return 0;
    }
    return 0;
