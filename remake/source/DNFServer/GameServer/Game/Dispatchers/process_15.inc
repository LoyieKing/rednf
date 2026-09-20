int Dispatcher_SkillInit::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e598a：`mov 0x14(%ebp),%eax; mov %eax,-0x10(%ebp)` —— p 指针在
    // check_error 调用之前落槽 -0x10（技巧13 声明位=落栈时机）；随后
    // `call; mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)` —— call 结果免槽直用（技巧37），
    // [b28c-L19 N18a] 唯一正文差 `p->m_8 = req.m_0d`：ORIG `movzbl 0xd(%eax),%edx`
    // 单步 vs 4.4 恒 mem→eax→edx 两段（反向不可造），封顶。
    // base 让位 %edx。
    stSkillInit* const p = static_cast<stSkillInit*>(&param);
    p->m_4 = check_error(user, msg);
    if (p->m_4 >= 1)
    {
        return 0;
    }
    if (p->m_4 < 0)
    {
        return LineFunc(0x5f3d,
                        __PRETTY_FUNCTION__,
                        p->m_4, 0);
    }
    MSG_SKILL_INIT& req = reinterpret_cast<MSG_SKILL_INIT&>(msg);
    p->m_8 = static_cast<unsigned char>(req.m_0d);
    // ORIG 0x081e59xx：末次使用仍从 p 的局部槽 -0x10 reload（`mov -0x10(%ebp),%eax`），
    // 直传形参 param 会被 4.4 折成 `mov 0x14(%ebp),%eax`。
    process_skill_init(user, *p);
    return 0;
}