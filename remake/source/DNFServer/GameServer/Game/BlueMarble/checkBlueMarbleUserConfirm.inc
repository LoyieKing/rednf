// [b28d-solo-BlueMarble NEAR(26)/N82附] 三处结构修复：①1024B 数组声明移入 if 块
// （ORIG rep stos 在 user==0 判空之后，0x080d9c28-3e）；②memoryPoolIndex/
// characNo/m_id 加纯 register 提示复现 ORIG edi/esi/ebx 跨调用保活
// （P231/P248 三条件；push edi/esi/ebx 齐备，无 asm 绑定）；③空 else{return;}
// 复现尾 `jmp <epilogue>; nop` 岛（P285）。137=137 gap 0。
// 余 26 条 = 4.4 标量/聚合分区（N82附）：buf -0x430 vs ORIG -0x424、三标量
// -0x28/-0x24/-0x20 vs ORIG -0x434/-0x430/-0x42c、log -0x30 vs -0x24。
// 本轮再实测两形均不翻：标量移入 if 块（buf 之后）、三标量包局部聚合 struct
// （劣化 DIFF，深槽多 -0x43c/-0x44c）；连同旧「块内提头/标量 register/全头部
// 声明」共六形 ⇒ N82附/P112 封顶。
// ORIG 0x080d9c12
void BlueMarble::checkBlueMarbleUserConfirm(CUser* user)
{
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
                // [M3S4 本轮实测-3形 · 138=138 与 ORIG 等长] 三标量声明位置三形
                // （函数头 / 外层 if 块 buf 之前 / 最内层 if 块使用处）产物**逐行相同**
                // ⇒ GCC -O0 槽分配与块层级无关；ORIG 槽分区（-0x434/-0x430/-0x42c
                // 三标量在 buf -0x424 **之下**，log -0x24、i -0x1c）不可复现
                // ⇒ N82附 封顶（四数 AE 化不可达）；本形使 our 139→138。
                int userBlueMarbleIndex = (int)(short)m_users[i].getUser()->getBlueMarbleIndex();
                // ORIG 0x080d9cd3 `mov %eax,%edi` / 0x080d9d58 `mov %eax,%esi` /
                // 0x080d9d8f `mov %eax,%ebx`：三值跨后续 call 保活于 callee-saved
                // 且无栈槽（P231/P248 三条件齐备）⇒ 纯 register 提示（非 asm 绑定）。
                register int memoryPoolIndex = getMemoryPoolIndex();
                UserSpace::T moveSpace = m_users[i].getUser()->getMoveSpace();
                int userState = m_users[i].getUser()->get_state();
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
}