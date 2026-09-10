void CBossPlay::InitPlayUser(int questCond, int towerIdx)
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = m_party->get_user(i);
        // [b28d-46 实测] ORIG 单条短路 or 守卫（Ghidra bVar1 汇聚）：
        // `cmpl $0,-0xc; je T` + `call; xor $1; test; je F` + `T:mov $1/jmp;
        //  F:mov $0; test %al; jne skip` ⇒ 合并 `||` 形。
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        {
            // [b28d-46 P52/P200] ORIG receiver 是**内联** get_user 调用结果
            // （0x081492e5 call 后 eax 直作 this），towerIdx 短整截断先物化
            // `movswl %ax,%ebx` 并跨该 receiver call 保活 ⇒ 禁具名 receiver 局部。
            m_party->get_user(i)->setBossTowerIndex(towerIdx);
            // ORIG 连续两次 call CUser::getCurCharacQuestW（0x081492f1 与
            // 0x0814930d），结果不入局部变量：第一次 `add $0x75d8,%eax`
            // + call _Quest_Authen_Data::reset ⇒ 具名成员 UserQuest::m_authenData
            //（CUser.h 权威布局 +0x75d8）；第二次直接 set_authen_data。
            user->getCurCharacQuestW()->m_authenData.reset();
            user->getCurCharacQuestW()->set_authen_data(
                (QUEST_CONDITION)0, questCond, 0xffffffff);
            user->set_state((ch_state)0xc);
            setMemberAlive(i, true);
        }
    }
}
