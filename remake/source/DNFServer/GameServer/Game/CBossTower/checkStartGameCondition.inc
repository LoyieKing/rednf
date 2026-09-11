// ----------------------------------------------------------------------------
// ORIG T 0x08142fa6 _ZN8WongWork10CBossTower23checkStartGameConditionERhb：
//   dungeon 空/未开放 → trace "boss Tower dungeon is not open"(0xdb,5) +
//     包(1,0x10)/byte 0/int 0x15 → send_to_party → 0；
//   逐槽位：有效成员 charac_level < get_min_level → trace(0xf0,5) +
//     包 int 0xe → send → 0；
//   partyType(+0xcd8)==1 → 1；否则 CheckEnterAncientDungeon 失败 → 包 int 0x11 +
//     byte(flag) → send → 0；useItems 时 UseAncientDungeonItems 失败 →
//     包 int 1 → send → 0；通过 → 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08142fa6：大件帧/槽整体平移（N82）+ 实参装载序（P23）；
//   控制流与调用序列全等（341 vs 340）。
// [b28f-SweepCBossTower] NEAR（340:340）。bool 化已落地（ORIG 调用点
//   0x081435b8/0x081441e6 `xor $1`）；残余仅 itemParams 与首个 cMyTrace 的
//   槽互换——ORIG itemParams=-0x44（深）/trace=-0x34（浅）vs 我们
//   trace=-0x44/itemParams=-0x34（N82附：4.4 聚合对象优先占低槽、标量分区
//   排布；声明序反转试形无效）。items -0x168、各分支 PacketGuard -0x50/
//   -0x5c/-0x68/-0x74、-0x16c(useItems spill) 与 ORIG 全等。
// [L5Free49to60 2026-09-11] **NEAR→AE（340:340，bcmp NDIFF 归零）**：同 P221 形——
//   首个 cMyTrace 由具名对象改全表达式临时链 `cMyTrace(...)(...)`，4.4 的聚合对象
//   低槽占用消失，itemParams 回深槽 -0x44、trace 回浅槽 -0x34，与 ORIG 逐槽全等。
bool CBossTower::checkStartGameCondition(unsigned char& flag, bool useItems)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    Inven_Item items[4];                       // ebp-0x168 起 4*0x3d，整体清零 0xf4
    memset(items, 0, 0xf4);
    int itemParams[4];                         // ebp-0x44 起 0x10 字节清零
    memset(itemParams, 0, sizeof(itemParams));

    if (m_dungeon == 0 || !m_dungeon->IsOpenScheduleDungeon())
    {
        // [L5Free49to60 P221 试形] ORIG 该 cMyTrace 对象占浅槽 -0x34..-0x24（4.1
        // 互斥块后声明占浅），4.4 具名对象恒占低槽 -0x44（与 itemParams 互换）
        // ⇒ 改全表达式临时链复现 ORIG 槽方向（P37/P221）。
        cMyTrace(__PRETTY_FUNCTION__, 0xdb, 5)("boss Tower dungeon is not open");
        PacketGuard packet;
        packet.clear();              // ORIG 0x0814309a（本分支 C1 后显式 clear）
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x15);
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081431b6..+0x1e3：正向包裹（P214①）——`member != 0 &&
        // checkValidUser(i)` 产成功物化岛 `mov$1;jmp/mov$0 + test %al; je
        // <循环尾>`；`|| !f()` 的 continue 形在 4.4 多一条 `xor $1`。
        if (member != 0 && m_party->checkValidUser(i))
        {
            if (member->get_charac_level() < m_dungeon->get_min_level())
            {
                // P227 全表达式临时链：ORIG 0x081431e9..+0x209 在 ctor call 前
                // 右起求值 varargs（charac_level→%ebx、min_level→%esi 跨 ctor 保活）。
                cMyTrace(__PRETTY_FUNCTION__, 0xf0, 5)(
                    "dungeon level check charac_level : %d, dungeon_min_level : %d",
                    member->get_charac_level(), m_dungeon->get_min_level());
                PacketGuard packet;
                packet.clear();      // ORIG 0x08143229（本分支 C1 后显式 clear）
                packet.put_header(1, 0x10);
                packet.put_byte(0);
                packet.put_int(0xe);
                packet.finalize(true);
                m_party->send_to_party(packet);
                return 0;
            }
        }
    }

    if (m_party->m_battleField.m_bloodState == 1)
    {
        return 1;
    }

    // ORIG 0x08143328/0x0814342f：两处调用结果共用 -0x11 具名 bool 槽
    // （`mov %al,-0x11; movzbl; xor $1; test; je`）⇒ 源码是同一个 `bool ok`
    // 局部两次赋值（P158 形），第二次用 if(useItems) 包裹而非 `&&` 短路。
    bool ok = m_party->CheckEnterAncientDungeon(m_dungeon, flag, items, itemParams);
    if (!ok)
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x11);
        packet.put_byte(static_cast<unsigned char>(flag));
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    if (useItems)
    {
        ok = m_party->UseAncientDungeonItems(m_dungeon, items, itemParams);
        if (!ok)
        {
            PacketGuard packet;
            packet.put_header(1, 0x10);
            packet.put_byte(0);
            packet.put_int(1);
            packet.finalize(true);
            m_party->send_to_party(packet);
            return 0;
        }
    }
    return 1;
}