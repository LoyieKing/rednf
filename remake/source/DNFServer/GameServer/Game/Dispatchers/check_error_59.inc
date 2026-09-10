int Dispatcher_ChangeAnotherSkillTree::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81d2094：state 3/6 才允许切换，并校验请求树索引。
    if (user == 0)
    {
        return -1;
    }
    // ORIG：`!(get_state()==3 || get_state()==6)` 展开为两次 get_state + 布尔合并。
    if (!(user->get_state() == 3 || user->get_state() == 6))
    {
        return 0x13;
    }
    const MSG_CHANGE_ANOTHER_SKILL_TREE& req =
        reinterpret_cast<const MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    // b28d-D0: P200 拓宽直传实参 —— ORIG 0x081d20e5 `movzbl 0xd(%eax),%eax;
    //   movsbl %al,%ebx` 值升格 callee-saved 跨 GetCurCharacSkillTreeIndex 存活；
    //   经具名 int 局部退化 `mov %eax,-0x10; mov -0x10,%eax; movsbl %al,%eax`。
    // ORIG：GetCurCharacSkillTreeIndex 调用两次（比较 + ==-1 检查）。
    if (static_cast<signed char>(req.m_0d) != user->GetCurCharacSkillTreeIndex())
    {
        return -2;
    }
    // b28d-D0: ORIG 0x081d2129 `sete %al; test %al,%al; je` 物化形；三目被 4.4 折直跳。
    if (user->GetCurCharacSkillTreeIndex() == -1)
    {
        return -3;
    }
    return 0;
