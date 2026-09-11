// ORIG _ZN8WongWork18CDeathTowerRanking4initEj @ 0x084680a4（0x1ec 字节）。
// this = getRankTable(type) 表；对 5 座塔 × 槽位 1..5 重置 stBestRecord_t：
//   m_flag90(+0x90) 清字节（ORIG movb）；m_memberData[0](+0x04) memcpy 默认名
//   "AAAAAA"/.../"EEEEEE"（ORIG 0x8c63370..0x8c6338d，7 字节含 NUL）；
//   m_field94(+0x94)=槽序号、m_playTime(+0x8c)=0、m_stage(+0x88)=200-槽序号
//   （ORIG 三条 dword 写，顺序 0x94→0x8c→0x88）；
// （this=表对象，ORIG Init 0x082a1f69..0x082a1f75 实证 getRankTable 结果作
// this 传入；体内不调 getRankTable）最后按 ORIG 顺序 m_tableType(+0x70f0)=type、
// m_tableBest.m_stage(+0x70e0)=200（ORIG 0x1d3 先写 type、0x1dc 再写 200）。
void CDeathTowerRanking::init(unsigned int type)
{
    // [b28d-solo-CDeathTower] verdict=AE（orig 119 / our 119，P167 CLEAR 5/5）：修法 =
    // ①去外层 `register`（4.4 把 tower 放 %ebx；ORIG 0x084680ab `movl $0,-0x14` 占
    //   栈槽）+ 去 rec 引用缓存（P183：ORIG 0x084681d6/084681fb/08468220 三次独立
    //   imul 链）⇒ 槽序 tower/-0x14、slot1/-0x10、slot2/-0xc 与 ORIG 全等；
    // ②`200 - slot` 改 `200 - (unsigned int)slot`（P66：unsigned 右操作数触发 4.4
    //   「右先物化」形 `mov slot,%eax; mov $0xc8,%edx; sub %eax,%edx`；int 形恒折
    //   `mov $0xc8,%eax; sub slot,%eax` 3 条）。
    // 另：P2 循环变量 int（ORIG 0x084680c0 尾测 setle；unsigned 生 setbe）。
    for (int tower = 0; tower <= 4; ++tower)
    {
        for (int slot = 1; slot <= 5; ++slot)
            m_towers[tower][slot].m_flag90 = 0;
        // ORIG 0x8c63370 实证每名 6 字符 + NUL（"AAAAAA\0" 等，7 字节；
        // memcpy 7 字节 = 6 字母 + NUL，与 ORIG 逐字节一致）。槽起点 +0x04 =
        // m_memberData[0]（ORIG 五次独立 memcpy 调用）。
        std::memcpy(m_towers[tower][1].m_memberData[0], "AAAAAA", 7);
        std::memcpy(m_towers[tower][2].m_memberData[0], "BBBBBB", 7);
        std::memcpy(m_towers[tower][3].m_memberData[0], "CCCCCC", 7);
        std::memcpy(m_towers[tower][4].m_memberData[0], "DDDDDD", 7);
        std::memcpy(m_towers[tower][5].m_memberData[0], "EEEEEE", 7);
        for (int slot = 1; slot <= 5; ++slot)
        {
            // [b28d-solo-CDeathTower] P183 地址重物化：ORIG 0x084681d6/084681fb/
            // 08468220 三次独立 imul 链，无 rec 引用缓存槽。
            m_towers[tower][slot].m_field94 = static_cast<int>(slot);
            m_towers[tower][slot].m_playTime = 0;
            m_towers[tower][slot].m_stage = 200 - (unsigned int)slot;
        }
    }
    m_tableType = type;
    m_tableBest.m_stage = 200;
}