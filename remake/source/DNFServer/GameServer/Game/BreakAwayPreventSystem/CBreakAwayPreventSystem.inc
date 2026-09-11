// ---- 后续批次业务模块 ----
break_away_prevent::CBreakAwayPreventSystem::CBreakAwayPreventSystem()
{
    // ORIG 0x0831c3e0：SIG 基类构造；map 构造；5×STRewardDataKey 构造；
    // memset(+0x30b, 0, 0x1f)；map.clear()。基类/Key 内容未还原（推断：
    // 以清零等价，容器真实构造）。
    // ORIG 0x0831c3e0：m_keys 仅构造 4 项（mov $0x4 循环 STRewardDataKeyC1，
    // 每项 3 字节）；第 5 槽（+0x308..+0x30a）保持垃圾，不清零。
    // 基类 SIG ctor / m_keys[5] 循环构造由编译器自动生成
    // （ORIG 0x0831c3e8 call SIG C1、0x0831c412 4-计数 key 循环）。
    memset(m_tail, 0, sizeof(m_tail));
    m_rewardMap.clear();
}