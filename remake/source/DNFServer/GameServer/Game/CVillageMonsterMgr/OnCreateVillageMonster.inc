// ============================================================================
// CVillageMonsterArea::OnCreateVillageMonster —— ORIG T 0x86b3bd4
//   m_field34 <= 当前怪物数 → 返回 0；随机 10000 取表项，构造怪物（slot =
//   size%3，m_field20/m_field24 取表 +0x14/+0x18），insert 后发 0xf6 新怪物包，
//   注册 0x86 生成定时器（param：m_field38/m_field28/m_field2c），返回 1。
// ============================================================================
// [M5E round6 · 旧判词 N82/N95/N104 全部推翻，213/214，仅余 .h 宽度轴]
//  ORIG 0x086b3bd4 三处直证 + 对应修正：
//  ① +0x03c `mov %eax,-0x20(%ebp)` / +0x03f `mov -0x20(%ebp),%eax`：ORIG 有具名
//     randIdx 局部（-0x20），我方旧码把 `get_rand_int(0x2710)` 直插实参缺 2 条；
//     补上后**全部槽位与 ORIG 逐个对齐**（attacked -0x1c、uid 形参 -0x22、monster
//     -0xb8、packet -0x90、pair -0x50/-0x7c/-0x84）——旧判「randIdx 具名反致帧
//     0xcc→0xdc」是当年**同时保留 uid 局部+cast 临时**时的读数，非 randIdx 之罪。
//  ② +0x0e3 `mov %ax,-0x22(%ebp)`（GetUID 结果直接落 make_pair 首参槽）⇒ ORIG 无
//     具名 uid、无 `(unsigned short)` cast 临时；旧码 `unsigned short uid` +
//     `make_pair((unsigned short)uid, monster)` 多 2 条（-0x1a 存 + 载）。改直传
//     `std::make_pair(monster.GetUID(), monster)` 后 p167 仍 CLEAR（make_pairIt
//     实例未失：符号集 delta 为空）。
//  ③ +0x2a6 `mov $0x1,%ebx`（return 1 求值）→ +0x2b4 ~PacketGuard → +0x2fa
//     ~CVillageMonster（正常路径尾块在函数末）⇒ ORIG 的 packet **不在内层块**，
//     其 dtor 晚于 InsertTimer；旧码内层 `{ }` 使正常路径多了 1 条 `jmp` 跳过
//     landing pad。去掉块后 215→214。
//  残余（唯一，.h 待父泳道裁决）：+0x0a3 / +0x1dc 两处 ORIG 为 `mov (%eax),%eax`
//     4 字节装载（ctor 实参处再 `movzwl %ax,%eax` 降位、put_short(int) 处不降位）
//     ⇒ ORIG 的 STAttackedMonster 偏移 0 字段**是 4 字节**；我方 .h 为
//     `unsigned short m_id; char m_pad02[2];` ⇒ 2×`movzwl (%eax),%eax` + 1 条多余
//     `movzwl %ax,%eax`（+1 条）。改 `unsigned int m_id;`（删 pad02，偏移
//     0x04/0x08/0x14/0x18 与 sizeof 0x20 全不变）⇒ 本件 213/213 逐条相同。
int CVillageMonsterArea::OnCreateVillageMonster()
{
    if ((unsigned int)m_field34 <= (unsigned int)m_monsters.size())
        return 0;
    int randIdx = get_rand_int(0x2710);
    STAttackedMonster* attacked = GetAttackedMonster(randIdx);
    if (attacked == 0)
        return 0;
    CVillageMonster monster(attacked->m_id,
                            attacked->m_field04, attacked->m_field08,
                            (int)(m_monsters.size() % 3), this,
                            attacked->m_field14, attacked->m_field18);
    m_monsters.insert(std::make_pair(monster.GetUID(), monster));
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf6);
    packet.put_short(monster.GetUID());
    packet.put_short(attacked->m_id);
    packet.put_byte(0);
    packet.put_short(attacked->m_field04);
    packet.put_short(attacked->m_field08);
    packet.finalize(true);
    SendPacket(packet);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x86,
                                m_field38, m_field28, m_field2c);
    return 1;
}