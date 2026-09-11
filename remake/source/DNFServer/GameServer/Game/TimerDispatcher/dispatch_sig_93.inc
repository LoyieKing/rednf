// ORIG T 0x08637aac (_ZN21TimerFinishItemEffect12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：槽位差 0xc + 帧差（N82/N93 族），ORIG 0x08637aac。
// [b28d-solo-TimerDispatcher] 判词 N82/N93 封顶：P157 去 entry 缓存已修（const
// operator[] ×2 补齐，195/195）；残差 = 槽位差 0xc + 帧差。
// [SweepTimerDispatcher2 2026-09-09] P329/P0 修复：InsertTimer 的 uid/characNo 具名局部
// 改内联实参（ORIG characNo→%edi、uid→%esi 无槽）⇒ 全帧槽号回 ORIG 形、ndiff 135→25；
// 残余 = ①`!=` 物化 setne/jne vs ORIG sete/je（P196/N18b，已试否定式回滚）
// ②`return user->Send(packet)` 的 movzbl（Send 返回 bool 的 4.4 扩展）
// ③characNo/uid 的 edi/esi 角色互换（N7）。
// [TimerDispatcher-3 2026-09-10 判据①] 22 条 jcc 目标核对：i63 `je/sete`↔`jne/setne`
// 同目标（N18b 折痕，语义等价）；i147 `jmp` 目标差 9 = ORIG `mov $0x1,%esi` 中继块位置
// 差（N104 EH 尾块序）；我方多出 `jmp <next>;nop` 属 4.4 epilogue-threading 岛（N121）。
// [Main P164 回扫 2026-09-10] /tmp/td_entryjmp.py 全 TU 扫描（`jmp L`+nop 填充+L 恰为
// 填充之后）：本件 OURS hits=1 / ORIG hits=0（其余 2 件两侧一致）⇒ 确有我方多余跳板。
// 按 P164 修法试「continue 移至循环体末」形（反条件 if/else 保块序）实测跳板仍在
// （仅位移 i177→i178）、ndiff 25 不变、四数不变 ⇒ **非 P164 族**（本件无后跳 goto，
// 系 PacketGuard 析构 esi 分流后的 latch 跳板）⇒ 回滚，维持 N121 判词。
bool TimerFinishItemEffect::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0x3);
    if (user == NULL)
        return 0;
    if (user->get_state() <= 2)          // ORIG setle: state<=2 → return 0
        return 0;
    if (p3 != 0 &&
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
        return 0;

    const unsigned int curSec =
        static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
    const std::vector<ContinuousItemInfo>* items =
        user->GetCurCharacEffectItemListR();
    if (items == NULL)
        return 0;

    // [LeaTimer28Run 2026-09-11] P248 档②·局部 unsigned 镜像视图：ORIG 0x08637b7e 段
    // `mov (%eax),%edx; mov 0x10(%ebp),%eax; cmp %eax,%edx; sete %al` 要求字段侧
    // **unsigned**（4.4 对 int 字段 + int 形参恒折单段内存直比 `cmp 0x10(%ebp),%eax`）。
    // 探针 /tmp/lea_probe2.cpp f_m 实证：`reinterpret_cast<const ContU*>(&v[i])->m_itemIdx
    // == (unsigned)p2` 逐条复现 ORIG 形、零额外指令；m_itemIdx 偏移 +0x00（头定义不变）。
    struct ContinuousItemIdxView
    {
        unsigned int m_itemIdx;     // +0x00（ContinuousItemInfo::m_itemIdx 的 unsigned 视角）
        unsigned int m_expireTime;  // +0x04
    };

    // 循环条件每次迭代重新取 size()（ORIG +0x290 seta 无符号比较）。
    // [b28d-solo-TimerDispatcher] P157：p167 差集 MISSING = const operator[] ×2
    // （ORIG 对 m_itemIdx/m_expireTime 每次重发 `(*items)[i]`，无 entry 引用缓存）。
    for (unsigned int i = 0; i < static_cast<unsigned int>(items->size()); ++i)
    {
        // [TD-2026-09-11 判据/探针] ORIG 08637b7e `mov (%eax),%edx; mov 0x10(%ebp),%eax;
        // cmp %eax,%edx; sete %al; test; je <i++ 尾部>` = `if (idx == p2) { body }`
        // **正向包裹**原形（Ghidra C 同形：`if (*piVar4 == param_2) {…}`）。旧码
        // `if (idx != p2) continue;` 产 `setne; jne` + 单段内存直比（ndiff 5 行）。
        // 探针 /tmp/td_p4.cpp：a1(`!=` 早返)⇒setne/jne；a2(`==` 正向包裹)⇒sete/je。
        if (reinterpret_cast<const ContinuousItemIdxView*>(&(*items)[i])->m_itemIdx ==
            static_cast<unsigned int>(p2))
        {
            PacketGuard packet;              // ORIG try 区 0x8637bb1..0x8637cfd
            packet.put_header(0, 0xae);
            packet.put_byte(1);
            packet.put_int(p2);
            packet.put_int(0);
            packet.finalize(true);

            if ((*items)[i].m_expireTime <= curSec)
            {
                // 已到期：立即结算并返回 Send 结果（ORIG esi=0 直通尾部返回 ebx）。
                _processSepecialEffectContinuousItem(user, p2);
                return user->Send(packet);
            }

            const unsigned int remain = (*items)[i].m_expireTime - curSec;
            if (!((remain < 6u) && (0x3cu < remain)))
            {
                // ORIG +0x1d2 重排程：OBJ_TYPE 0 / key=GetUID / msg 0x76 / delay=remain / p2 / characNo
                // [SweepTimerDispatcher2] P329/P0：ORIG 无 uid/characNo 具名槽——characNo 先求值
                // 驻 %edi、uid 驻 %esi（实参右→左求值），我方具名局部使全帧槽深 +8（-0x2c→-0x34）。
                // 改内联实参后槽序应回 ORIG 形。
                G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                            static_cast<int>(user->GetUID()),
                                            (TIMER_MESSAGE)0x76,
                                            static_cast<int>(remain),
                                            p2,
                                            static_cast<unsigned int>(user->getCurCharacNo()));
                continue;                    // esi=1 → 回到 i++ 尾部继续遍历
            }

            // ORIG +0x227 死分支（remain<6 且 >60 同时成立，恒不可达，按汇编原样保留）。
            _processSepecialEffectContinuousItem(user, p2);
            return user->Send(packet);
        }
    }
    return 1;                            // ORIG 遍历结束 ebx=1
}