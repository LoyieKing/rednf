// [b28d-solo-W7_LinkTailGrowth 判词·DIFF/N104+N7] 语义正确：事件(0x5d)开启→
//   IsEventing 双调用→seria is_possible_lv→set_growth_power_exp_reward_right(true)
//   +breakAway section 判定；breakAway||seria→PacketGuard(0x178,3 byte)+Send；
//   未开启→breakAway 判定→同包(byte 1,1)+set_growth_power_exp_reward_right(true)。
//   本轮修：① 第二 IsEventing+is_possible_lv 两条件改 P226 语句表达式（复现 ORIG
//   +0x8b..+0x9b 两 `je` 同址 0x816 的共享短路物化岛）；② 尾部
//   set_growth_power_exp_reward(get_charac_level()) 拆两份物理独立块（P165，ORIG
//   +0x1ed/+0x32a 各一份）。修后 231/231 指令数全等、帧 0x40 与全部槽号一致、
//   p167 调用点差集 42/42 CLEAR；唯一残差 = 第二分支 PacketGuard::~D1 的相对序
//   rotate（ORIG 正常路径 D1 在 `jmp +0x32a` 之前；4.4 恒把该 D1 挪到 unwind
//   landing 之后，块内容逐条全等）⇒ N104 封顶；另 N7 虚调用目标寄存器
//   %eax↔%edx（ORIG %edx；P230=4.4.6 后端可修，路由表不动）。
void CUser::decide_growth_power_reward_system()
{
    bool breakAwayFlag = false;                     // -0xe（local_12）
    bool seriaFlag = false;                         // -0xd（local_11）

    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (ev->IsEventing(0))                          // vptr+0x34 @+0x20
    {
        // ORIG +0x4a..+0x9d：第二 IsEventing 与 is_possible_lv 两条件**共享**同一
        // bool 物化岛（`je 0x816` 同址 + mov $1/jmp/mov $0/test;je，值无槽往返）
        // ⇒ P226 扩展：语句表达式包住整个 `&&` 条件（嵌套 if 被 4.4 jump-thread 折掉）。
        if (({ ev->IsEventing(0) &&                 // vptr+0x34 @+0x4a（ORIG 双调用）
               G_CDataManager()->m_seriaBlessing.is_possible_lv(get_charac_level()); }))
        {
            // +0x6e..+0xa1：stSeriaBlessingScript::is_possible_lv(level)，
            // m_seriaBlessing 位于 CDataManager +0x5094。
            seriaFlag = true;
            set_growth_power_exp_reward_right(true);   // +0xb1
        }

        // +0xbc..+0xf5：breakAwaySection = STGrowthPowerData(+0x8710)
        //   .get_break_away_section_data(get_charac_level())；
        //   get_growth_power_reward() < section.m_field1 → breakAwayFlag=1。
        STBreakAwaySection section =
            G_CDataManager()->m_growthPower.get_break_away_section_data(
                get_charac_level());
        if ((unsigned char)get_growth_power_reward() <
            (unsigned char)section.m_field1)
        {
            breakAwayFlag = true;
        }

        if (breakAwayFlag || seriaFlag)             // +0x100..+0x10a
        {
            PacketGuard packet;
            packet.clear();                                    // +0x121
            packet.put_header(0, 0x178);                       // +0x13c
            packet.put_byte((int)(signed char)get_charac_level());  // +0x159 movsbl
            packet.put_byte(breakAwayFlag ? 1 : 0);            // +0x17a
            packet.put_byte(seriaFlag ? 1 : 0);                // +0x19b
            packet.finalize(true);                             // +0x1ae
            Send(packet);                                      // +0x1c0
        }

        // ORIG +0x1ed 尾部第一份（独立物理块，P165 两份不共享；+0x202 jmp 到 epilogue）
        set_growth_power_exp_reward(get_charac_level());
    }
    else                                            // +0x20c 分支（事件未开启）
    {
        // +0x232..+0x245：同上取 break away section 并比较。
        STBreakAwaySection section =
            G_CDataManager()->m_growthPower.get_break_away_section_data(
                get_charac_level());
        if ((unsigned char)get_growth_power_reward() <
            (unsigned char)section.m_field1)                    // setb @+0x24b
        {
            PacketGuard packet;
            packet.clear();                                     // +0x267
            packet.put_header(0, 0x178);                        // +0x282
            packet.put_byte((int)(signed char)get_charac_level());  // +0x29f
            packet.put_byte(1);                                 // +0x2b2 固定 1
            packet.put_byte(1);                                 // +0x2c2 固定 1
            packet.finalize(true);                              // +0x2d8
            Send(packet);                                       // +0x2ea
            set_growth_power_exp_reward_right(true);            // +0x2fd
        }

        // ORIG +0x32a 尾部第二份（fall-through 到 epilogue）
        set_growth_power_exp_reward(get_charac_level());
    }
}