void BlueMarble::checkBlueMarbleUserConfirm(CUser* user)
{
    int userBlueMarbleIndex;
    UserSpace::T moveSpace;
    int userState;
    int i;
    if (user != 0)
    {
        // ORIG 0x080d9c28-0x080d9c3e：1024 字节 rep stos 清零位于 user==0
        // 判空之后 ⇒ 数组声明在 if 块内（函数头声明会在入口即清零）。
        char local_428[1024] = {0};
        sprintf(local_428, "./log/%s/BlueMarbleConfirmInfo",
                G_CEnvironment()->get_file_name());
        for (i = 0; i < m_pScript->m_info.m_playerMax; ++i)
        {
            if (m_users[i].getUser() == user)
            {
                userBlueMarbleIndex = (int)(short)m_users[i].getUser()->getBlueMarbleIndex();
                // ORIG 0x080d9cd3 `mov %eax,%edi` / 0x080d9d58 `mov %eax,%esi` /
                // 0x080d9d8f `mov %eax,%ebx`：三值跨后续 call 保活于 callee-saved
                // 且无栈槽（P231/P248 三条件齐备）⇒ 纯 register 提示（非 asm 绑定）。
                register int memoryPoolIndex = getMemoryPoolIndex();
                moveSpace = m_users[i].getUser()->getMoveSpace();
                userState = m_users[i].getUser()->get_state();
                register int characNo = m_users[i].getUser()->get_charac_no(-1);
                register char* m_id = NumberToString(m_users[i].getUser()->get_acc_id(), 0);
                CMyFileLog log(__PRETTY_FUNCTION__,
                               0x514);
                log(local_428,
                    "checkBlueMarbleUserConfirm() m_id(%s) charac_no(%u)userState(%d) userSpace(%d) blueMarble_Index(%d) user_bluemarble_index(%d) slot_index(%d)",
                    m_id, characNo, userState, moveSpace, memoryPoolIndex,
                    userBlueMarbleIndex, i);
            }
        }
    }
    else
    {
        // P285：空 else{return;} 保留 if 体尾 `jmp <epilogue>; nop` 岛
        // （ORIG 0x080d9e15/0x080d9e17）。
        return;
    }
