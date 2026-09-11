// ============================================================================
// CVillageMonsterArea::SendVillageMonsterList(CUser*) —— ORIG 0x86b37fa（0x168）
//   objdump: PacketGuard; clear; put_header(0,0xee); idx=get_index();
//            put_byte(0 占位); 遍历 m_monsters：MakeVillageMonsterListPacket
//            成功则 count++；put_byte(idx,count)；finalize(true)；
//            user==0 → SendPacket(packet)，否则 user->Send(packet)。
// ============================================================================
// [round2-L30 复核·N126] 残余=整帧槽号常数平移 4（ORIG packet -0x20/countIdx -0x24/
//   count -0x10 vs 我方 -0x1c/-0x20/-0xc，间距全等；帧同为 sub $0x30）⇒ 判据⑪ (b)
//   类，声明重排无效（ORIG 0x086b37fa）。已修：占位字节改传具名 count（ORIG
//   0x086b3814 `mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)`，非立即数 0）。
// [M2S4 2026-09-11 推翻旧 N126 判词] 108/108（ORIG 108，逐条同形；p167 18/18，
//   余 PacketGuard C1↔C2/D1↔D2 同址别名）。真因两处：
//   ① 缺具名 `CVillageMonster* monster = &it->second;`（P0：ORIG 0x086b387c/387f
//      `add $0x4,%eax; mov %eax,-0xc(%ebp)` + 0x086b3889 load 直证有真槽；链式写法
//      4.4 直传 `lea 0x4(%eax),%edx` 少 store/load 两条，且整帧平移 4）。
//   ② ORIG 0x086b3894..0x86b38a2 是 4.1 if-join 岛（`xor $1; test %al; jne <nop 岛>`
//      + 体尾 `jmp` + `nop`）⇒ 4.4 复现形=负条件 goto 尾空标签（探针 /tmp/m2s4_island.cpp
//      fC 实测同形；`if (!x) continue;` 恒无岛）。
void CVillageMonsterArea::SendVillageMonsterList(CUser* user)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xee);
    int countIdx = packet.get_index();
    int count = 0;
    // ORIG 0x086b37ff：占位字节传的是**具名 count 变量**（0x81 `mov -0x10(%ebp),%eax;
    //   mov %eax,0x4(%esp)`），非立即数 0 ⇒ 源形 `put_byte(count)`（此时 count==0）。
    packet.put_byte(count);
    // [P0] ORIG 0x086b387c/0x086b387f `add $0x4,%eax; mov %eax,-0xc(%ebp)` +
    //   0x086b3889 `mov -0xc(%ebp),%eax` —— monster 具名局部有真槽（链式写法 4.4
    //   直传 `lea 0x4(%eax),%edx`，少 store/load 两条）。
    // [P223] ORIG 0x086b3894..0x086b38a2 = 4.1 if-join 岛（`xor $1; test %al; jne
    //   <nop 岛>` + 体尾 `jmp` + `nop`）⇒ 4.4 复现形为负条件 goto 尾空标签
    //   （探针 /tmp/m2s4_island.cpp fC 实测同形；`if (!x) continue;` 恒无岛）。
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        CVillageMonster* monster = &it->second;
        if (!monster->MakeVillageMonsterListPacket(packet))
            goto next_monster;
        ++count;
    next_monster:;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    // [N1F 2026-09-11] 极性/叶块序：ORIG 0x086b3904 `cmpl $0x0,0xc(%ebp)` + 0x086b3908
    //   `je 0x086b391e` —— je 落点 = 0x086b391e 的 CVillageMonsterArea::SendPacket(this) 体，
    //   直落体 = 0x086b3911 `mov 0xc(%ebp),%eax` 的 CUser::Send(user) 体
    //   ⇒ 4.1 -O0「假则跳 else」布局：源形条件 = `user != 0`，else = SendPacket。
    //   语义等价（两态落点一致），纯布局轴。
    if (user != 0)
        user->Send(packet);
    else
        SendPacket(packet);
}