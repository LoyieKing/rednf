// ORIG T 0x0860dccc sz 0x184（InsertCirculationStatistic.md 全文已读）：
// 遍历 m_data（map<ENUM_SERVER_GROUP,stCirculationStatistic>）；每条目 level 1..0x46
// 有任一 field 值非 0 → 构造 Packet_Circulation_Statistic（0xd0 字节）：
//   ctor(0x086100f8)→m_fieldCE=get_channel_no()→m_channelLevel=level→memcpy(数据, level*0xc0)
//   → GetServerProxy(group).SendPacket(buf,0xd0)；全部 level 发送完 → reset。
// 注意：局部包对象在栈上，ctor 置 +0x0a/+0xce 为 0（Packet_Circulation_Statistic.h
// 初始化列表已对位）。
// [M4FreeNext15C.M4S1 round5 · 语义轴 ③ 字段错位] ORIG 0x0860dd84 `mov %ax,-0x26(%ebp)`
// 落 **+0xce**（16 位）、ORIG 0x0860dd88 `mov -0x14(%ebp),%eax; mov %eax,-0xea(%ebp)`
// 落 **+0xa**（**32 位**）⇒ 原码是 `m_fieldCE = get_channel_no(); m_channelLevel = level;`，
// 旧我方两条 16 位 `fields.m_channel/.m_level` 写把 channel 错放到 +0xa、level 错放到 +0xc。
// [待 .h 裁定 · 签名轴] ORIG 0x0860de46 `mov $0x1,%eax`（for 退出路径、`mov -0x4(%ebp),%ebx`
// 之前）= **返回 1**。已落地：父泳道改 CCirculationStatistic.h 为 `bool`，本 TU
// 定义侧同步 `.inc` 签名 + 末尾 `return true;` ⇒ 单件 ndiff=0（该 +1 条补齐 108→109）。
bool CCirculationStatistic::InsertCirculationStatistic()
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0860dccc 槽组整体平移 4（-0xf4/-0x1c vs -0xf8/-0x20）。
    // [L6-S1 P0/P183 归位] 真因 = 具名 `CStatisticServerProxy* proxy` 多占 4B 槽（我方
    // 多 `mov %eax,-0x10(%ebp)` + 调用点回载两条）：ORIG 0x0860ddd2 后直接
    // `movl $0xd0,0x8(%esp); mov %ebx,0x4(%esp); mov %eax,(%esp); call SendPacket`
    // ⇒ 原码是 `GetServerProxy(group)->SendPacket(...)` 内联实参形（P134）。删具名后
    // 槽组整体回位（-0x28/-0x20/-0x1c/-0x18/-0xf8 → ORIG 的 -0x24/-0x1c/-0x18/-0x14/-0xf4）。
    // [M4FreeNext15C.M4S1 round5 归位 · P326] ORIG 0x0860dccc 的 for 骨架：
    // 函数头 `lea -0x24(%ebp); call begin` 后**直接 jmp +0x14b**，而尾块 +0x14b
    // `lookup end()`（`call 08611440`，写临时 -0x20）+ `operator!=(it,-0x20)` +
    // `jne 体` —— 即 end() 在**条件块内每次迭代现调**、临时返回槽 -0x20 复用，
    // 不是循环前缓存的第二个具名迭代器。旧我方 `it=begin(), endIt=end()` 使
    // begin/end 在序言里连调（-0x20/-0x24 顺序亦反）且条件块只剩 operator!=。
    for (std::map<ENUM_SERVER_GROUP, stCirculationStatistic>::iterator it =
             m_data.begin();
         it != m_data.end(); ++it)
    {
        const ENUM_SERVER_GROUP group = it->first;
        stCirculationStatistic& stat = it->second;
        for (int level = 1; level <= 0x46; ++level)
        {
            bool hasValue = false;
            for (int field = 0; field <= 0x2f; ++field)
            {
                if (stat.m_values[level][field] != 0)
                {
                    hasValue = true;
                    break;
                }
            }
            if (!hasValue)
                continue;

            Packet_Circulation_Statistic packet;
            // [M4FreeNext15C.M4S1 round5 · 语义轴 ③ 字段错位] ORIG 0x0860dd84
            // `mov %ax,-0x26(%ebp)` 落 **+0xce**（m_fieldCE，16 位），
            // ORIG 0x0860dd88 `mov -0x14(%ebp),%eax; mov %eax,-0xea(%ebp)` 落
            // **+0xa**（m_channelLevel，**32 位**）—— 即原码是
            // `m_fieldCE = get_channel_no(); m_channelLevel = level;`，
            // 不是 `fields.m_channel/fields.m_level` 两条 16 位写（后者发
            // `mov %ax,-0xea / mov %ax,-0xe8` 两条，字段错位到 +0xa/+0xc）。
            packet.m_fieldCE =
                static_cast<unsigned short>(G_CEnvironment()->get_channel_no());
            packet.m_channelLevel = level;
            memcpy(packet.m_data, stat.m_values[level],
                   0x30 * sizeof(int));
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(group)
                ->SendPacket(reinterpret_cast<char*>(&packet), 0xd0);
        }
        stat.reset();
    }
    // [M4FreeNext15C.M4S1 round5 · 签名轴落地] ORIG 0x0860de46 `mov $0x1,%eax` 位于
    // for 退出路径（`jne 体` 的失败边）与 `mov -0x4(%ebp),%ebx; leave; ret` 之间
    // ⇒ 原码为 `return true;`（.h 已由父泳道改 `bool InsertCirculationStatistic();`）。
    return true;
}