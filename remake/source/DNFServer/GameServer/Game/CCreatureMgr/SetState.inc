// ORIG _ZN13user_creature9CCreature8SetStateEP5CUserNS_14CREATURE_STATEE T 0x0833715a
// （docs/class_func_reports/user_creature__CCreature/SetState.md）。
// [b28d-solo-CCreatureMgr N] ndiff=10：①三处 `(int)state != m_state` ORIG 为
// `mov 0x3c(%eax),%eax; cmp 0x10(%ebp),%eax`（形参 mem 直比），4.4 恒
// `mov 0x3c,%edx; mov 0x10(%ebp),%eax; cmp %eax,%edx`——8 形探针（f1..f8 含
// `m_state != (int)state` / `(CREATURE_STATE)m_state != state` / unsigned 双向）
// 均不可产（N18b/P209 族）；②InsertTimer 三实参首值 callee-saved 角色互换
// （ORIG esi=第 6 实参 getCurCharacNo / ebx=GetUID，4.4 相反，N82附/P78）。
// [b28d-solo-CCreatureMgr S1 已试] 交换左右操作数（m_state != (int)state）后 4.4 仍产
// `mov 0x3c,%edx; mov state,%eax; cmp %eax,%edx`（两寄存器比较，135 条不变）⇒ 恢复直证形。
// 语义全等（p167 CLEAR）。ORIG 0x0833715a
void CCreature::SetState(CUser* user, CREATURE_STATE state)
{
    float stomachValue = m_stomach.GetStomachValue(m_state, m_isGrow);
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
    // [2026-09-11 旋钮①-负结论] 交换首比较书写序（m_state 在左）实测无效：
    //   4.4 产 `mov 0x8,%eax; mov 0x3c(%eax),%edx; mov 0x10,%eax; cmp %eax,%edx`
    //   （仍是两寄存器比较，135 条不变），ORIG 0x8337192 为
    //   `mov 0x8(%ebp),%eax; mov 0x3c(%eax),%eax; cmp 0x10(%ebp),%eax`（mem 直比）
    //   ⇒ P209：4.4 对 unsigned 形参（enum CREATURE_STATE）比较恒载寄存器，
    //   两向书写序皆不可造 mem 直比 ⇒ 封顶（回滚）。
    // [2026-09-11 旋钮③-负结论] enum 同型比较（`state != (CREATURE_STATE)m_state`）
    //   实测仍为 `mov 0x10(%ebp),%edx; mov 0x8(%ebp),%eax; mov 0x3c(%eax),%eax;
    //   cmp %eax,%edx`（135 条不变）；4.4 对 enum(unsigned 底层) 形参参与的任何
    //   比较恒载寄存器 ⇒ 两形 + 交换书写序三试皆不可造 ORIG 的
    //   `cmp 0x10(%ebp),%eax` mem 直比（P209/N18b）⇒ 封顶。
    if (state != m_state && (int)state == 1 && m_isGrow != 0)
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
        if (state != m_state && (int)state == 3 && m_isGrow != 0)
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
    // [2026-09-11 旋钮②-负结论] 尾部 store 装载序：ORIG 0x8337318 为 LHS-first
    //   `mov 0x8(%ebp),%eax; mov 0x10(%ebp),%edx; mov %edx,0x3c(%eax)`。
    //   P211 同型视图形 `m_state = *reinterpret_cast<int*>(&state);` 实测
    //   LHS-first 得手但**多 5 条**（`lea 0x10(%ebp),%eax; mov (%eax),%edx` 取址链，
    //   135→140）⇒ 净负回滚。
    // [2026-09-11 enum 化落地 · 收口判词] 字段改 `CREATURE_STATE`（CGameManager.h:365
    //   + :166 C++98 宽度锁）+ 消费侧同型化（比较去 (int)、store 去 (int)）后本件
    //   **133 = 133**（原 135/DIFF ⇒ NEAR）：
    //   ① 首比较 0x8337192 逐条复现 `mov 0x8(%ebp),%eax; mov 0x3c(%eax),%eax;
    //      cmp 0x10(%ebp),%eax`（mem 直比）；② 尾 store 0x8337318 复现 LHS-first
    //      `mov 0x8(%ebp),%eax; mov 0x10(%ebp),%edx; mov %edx,0x3c(%eax)`；
    //   ③ 残余唯一 = callee-saved 角色互换（ORIG getCurCharacNo→%esi / GetUID→%ebx，
    //      4.4.7 相反；get_unique_id 两侧均 %edi）⇒ N7/N109 族（P248 条件② 反例；
    //      asm 绑定全域禁、裸 register 条件②「4.4 不自升格」不满足）⇒ NEAR 封顶。
    //   实证探针：/tmp/probe_enum.cpp（同路由旗标，int 字段产两寄存器比较+RHS-first，
    //   enum 字段产 ORIG 两形）、/tmp/probe_enum2.cpp（另 5 个消费形 int↔enum 逐字节相同）。
    // [C4A0Nine 2026-09-11 复验] n1_v=NEAR 133/133 gap=0，唯一残差仍为 callee-saved
    //   角色互换（ORIG 0x083371dc `mov %esi`←getCurCharacNo / 0x083371f7 `mov %ebx`←GetUID；
    //   4.4.7 相反，%edi 两侧一致）⇒ N7/N109 封顶不变（本波未再试形，前波三旋钮已穷举）。
    m_state = state;
    m_saveFlag = 1;
}
