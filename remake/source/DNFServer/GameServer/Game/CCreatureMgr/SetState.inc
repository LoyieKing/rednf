void CCreature::SetState(CUser* user, CREATURE_STATE state)
{
    float stomachValue = m_stomach.GetStomachValue((CREATURE_STATE)m_state, m_isGrow);
    // [b23S3C] P191/P194 极性：ORIG 0x8337192/0x195、0x833719b/0x19f、
    // 0x83371ac/0x1ae 三条判定全部「假则前跳」到 0x0d4 复活测试块，定时器分支走
    // fall-through ⇒ 原码先判 timer 条件（&& 短路正向形），else 内再判复活条件；
    // 旧 || 负形使 4.4 产出 xor $0x1（bool==0 折叠）+ 块序倒置。语义等价。
    // [P321-S1 已试] 交换左右操作数后 4.4 仍产 `mov 0x3c,%edx; mov state,%eax;
    // cmp %eax,%edx`（两寄存器比较）⇒ 恢复直证形；残余 = P209/N18b 形参 mem 直比
    // 不可造（4.4 恒载入寄存器）。ORIG 0x8337192 `cmp 0x10(%ebp),%eax`。
    // [C01/F2 复查·已试无效] Main 转发 C05 F2（int 局部 ⇒ 比较内存直比）在本件
    // **不适用**：ORIG 0x8337192 `cmp 0x10(%ebp),%eax` 的 mem 直比对象是**形参槽**
    // （enum CREATURE_STATE，mangled 固定不可改型），非局部；试具名 int 局部形
    // `const int st=(int)state;` 后本件 135→**141**（多 3 条 store/load 且预载
    // `mov 0x10(%ebp),%edx` 仍在）⇒ 已回退，维持 P209/N18b 封顶。
    if ((int)state != m_state && (int)state == 1 && m_isGrow != 0)
    {
        // 切入战斗态（state 1）：按 60 秒 × 饱食度系数插入复活定时器
        // （常量 .rodata 0x8c2f460 = double 60.0；TIMER_MESSAGE 0x5c）。
        // [b23S3C] P0 律/P48 去缓存：ORIG 0x83371d7/0x8a/0x98 三个 call 结果只进
        // %esi/%edi/%ebx，无 -off 槽 ⇒ 原码无 characNo/uniqueId 具名局部，
        // 直落 InsertTimer 实参（求值序右起：characNo→uid→GetUID→G_TimerQueue，
        // 与 ORIG 一致）；sec 走 -0x1c 槽（fistpl 实证）保留具名。
        int sec = (int)(60.0 * stomachValue);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                    (TIMER_MESSAGE)0x5c, sec,
                                    user->get_unique_id(),
                                    user->getCurCharacNo());
    }
    else
    {
        // 进入 state 3 且非同值切换：死亡宠复活流程（ORIG 0x08337269 起）。
        if ((int)state != m_state && (int)state == 3 && m_isGrow != 0)
        {
            if (IsDieCreature())
            {
                CPacketRespondent pkt(user, 0, 0);
                pkt.MakeNotipacketRevivalCreature();
                pkt.SendAll();
                m_stomach.SetStomachValue(1.0f);
                pkt.clear();  // InterfacePacketBuf::clear（复用包对象重填状态包）
                // [b23S3C] ORIG 0x8337269→0x83372c9：状态包 uid 实参来自
                // CCreatureItem::GetUid(this) 调用（非 m_uid 直读）⇒ 补真调用。
                pkt.MakeNotipacketCreatureState(
                    reinterpret_cast<CCreatureItem*>(this)->GetUid(), 1);
                pkt.Send();
            }
        }
    }
    m_state = (int)state;
    m_saveFlag = 1;
}
