void LevelupSupportEventManger::UpdateEventInfo(CUser* user)
{
    // b28d-solo-CUserDepsB: NEAR（114=114，助记符序列全等）——①P214① 正极性
    // 包裹 `if(A||B){主流程}` 复现
    // ORIG 0x081471ec 的 mov$1/mov$0 短路物化 + 0x0814721d `test;je 尾`
    // （具名 bool pass 恒槽化 movb/-0x19；语句表达式/register 两形实测均劣化）；
    // ②循环上界是**本类 m_events**（ORIG 0x08147248 `this+0x34`）而非
    // user->m_levelupEvents（P171 照抄原码对象选择）；③P157 循环体逐次重调
    // operator[]（去具名 info 引用）；④真语义 bug 修正：makeRequest 第2/3参
    // 为 (characNo, level)——W8LT_DBTask.cpp dispatch 读流序 characNo→level→state、
    // InterDispatcher.cpp:17832 / UserMiscLink.cpp:522 两调用点同序，ORIG
    // 0x081472f2 压栈 `$1, level, characNo, uid` 实证。114=114，残差仅
    // N7/P75：uid/characNo 两实参装载时刻（ORIG `mov -0x14,%eax`(characNo) 先、
    // uid 后载；4.4 先保活 uid 于 %eax、characNo 走 %edx）。[S191 本轮复验]
    // 删 cast / uid 局部改 int 两形实测：前者无变化、后者把 NEAR 打成 DIFF
    // （movzwl %ax,%edx→%eax 破坏助记符序列）⇒ 维持 unsigned + 直传，N7/P75 封顶。
    // [L29 2026-09-09 复验] 判据① 全 TU 逐位：本函数**无** jcc/jmp 目标差；
    // 残差仅 makeRequest 压栈前 characNo 的载入寄存器（ORIG `mov -0x14(%ebp),%eax`
    // 后 `mov %eax,0x4(%esp)`；我们 `mov -0x14(%ebp),%edx` 后 `mov %edx,0x4(%esp)`），
    // 参数序/槽位/指令数全同。再试两形：`register int characNo`（免槽，NEAR→DIFF）、
    // `unsigned int characNo`（NEAR→DIFF）⇒ 两旋钮皆不可用，维持 N7 封顶。
    // ORIG 0x081471c6。
    int level = user->get_charac_level();
    // P214① 正极性包裹：复现 ORIG 0x081471ec 的短路物化岛（test/jne L_pass →
    // B 假落 mov$0）+ 0x0814721d `test %al,%al; je 函数尾`（主流程直落）。
    if (CheckNewStep(level) ||
        GlobalData::s_GM_Manager->GetCurrentGmMode(
            user->get_acc_id()))                   // T 0x08299276
    {
        unsigned int uid = user->GetUID();         // W 0x080c8c96
        int characNo = user->getCurCharacNo();
        unsigned int i = 0;
        unsigned int count = m_events.size();      // ORIG this+0x34（非 user）
        for (; i < count; ++i)
        {
            // P214① && 正极性包裹：复现 ORIG 的 mov$1/mov$0 物化岛。
            if (user->m_levelupEvents[i].m_level <= level &&
                user->m_levelupEvents[i].m_state != 2)
            {
                user->m_levelupEvents[i].m_state = 1;   // movw $0x1, 0x2
                DB_SaveEventLevelupSupport::makeRequest(
                    uid, characNo,
                    static_cast<int>(user->m_levelupEvents[i].m_level), 1);
            }
        }
        if (CheckNewStep(level))
            SendCharacState(user, level);
        else
            SendCharacState(user, 0);
    }
