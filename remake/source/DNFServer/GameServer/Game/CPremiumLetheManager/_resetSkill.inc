// [b28d-solo 2026-09-11 · 新口径 n1_v.py] ORIG 0x085c40aa | **AE orig 101 / our 103**
// （gap −2；旧 game_func_report 口径假 DIFF 101/102）。差异仅我方多 2 条：
// ORIG 0x085c41d1..0x085c41dc `call PacketGuard::~PacketGuard; jmp <epilogue>` 之后无填充，
// 我方同点多 1 条 jmp 后 `nop`（对齐填充；属 P6.1 nop 豁免族，但工具只剥 ret/call 之后者）。
// 条件段 `call isAffectedPremium; xor $1; test %al,%al; jne`（bool 消费侧）两侧全等；
// 无声明序/槽映射残差（P339/P331 不适用）⇒ N 封顶，源码零改动。
void CPremiumLetheManager::_resetSkill(CUser* user)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    unsigned char flag;
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        flag = user->GetOneDayLetheFlag();
    }
    else
    {
        flag = user->GetOneDayLetheFlag2ND();
    }
    if (flag != USE_LETHE_STATE)
        goto FAIL_TRACE;    // ORIG jne 尾置 else 块（4.4 threading 会前移，goto 阻断）
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
    }
    else
    {
        InitSkill(user, true,
                  (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex());
        PacketGuard guard;
        guard.put_header(1, 0xd5);
        guard.put_byte(1);
        guard.finalize(true);
        user->Send(guard);
    }
    return;
FAIL_TRACE:
    {
        // [b28c-L29] P221：具名 trace 对象使 PacketGuard(-0x28)/trace(-0x1c)
        // 的 sibling 块槽方向与 ORIG 相反；全表达式临时链复现 ORIG 槽深。
        cMyTrace(__PRETTY_FUNCTION__, 0x93, 0)("ONE_DAY_LETHE : CAN'T USE INIT STATE");
    }
}