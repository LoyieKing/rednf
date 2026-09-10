int CMission_revenge::Update(MissionInfo& info,
                             const MissionClearCondition_Parameter& param) const
{
    switch ((int)info.m_streak)
    {
    case 0:
        if ((param.m_victory & 1) == 0) info.m_streak = 1;
        return 0;
    case 1:
        info.m_streak = 2;
        return 0;
    case 2:
        if ((param.m_victory & 1) != 0)
        {
            info.m_streak = 0;
            info.m_count += 1;
            return 1;
        }
        // [b28c-35] ORIG 0x085e3ac0 段：case2 的 else 与 default 共享函数尾
        // `mov $0,%eax`（ORIG `je <nop>; nop; mov $0`），独立 `return 0` 会多
        // `mov $0;jmp` 一对（61 vs 61 但块数 128→126）。
        break;
    default:
    {
        // b28c-lane12 P0 清绑：删 asm("ebx") 绑定与 register 关键字（C28L20A
        // 反例：裸 register 强制 push/pop + 帧破形）。ORIG 形为「变参在 ctor
        // call 前求值并跨 call 保活」（P87）⇒ 改 P227 全表达式临时链承载。
        cMyTraceNoop()("[@missionSystem] : revenge ??? : %d",
                       (int)info.m_streak);
        break;
    }
    }
    return 0;
