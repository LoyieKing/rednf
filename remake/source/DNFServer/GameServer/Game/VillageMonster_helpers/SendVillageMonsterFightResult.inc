// ============================================================================
// CVillageMonster::SendVillageMonsterFightResult(CUser*,bool)
//   —— ORIG 0x86b330a
//   objdump: user==0 -> return; party=user->GetParty(); party==0 -> return;
//            for i in 0..3: member=party->get_user(i); member!=0 时:
//               pkt.m_uids[i]=member->GetUID();
//               pkt.m_characNos[i]=member->get_charac_no(-1);
//               if (s_villageMonsterMgr->OnEvent() && flag)
//                   member->IncVillageAttackCount();
//            pkt.m_flag=flag;
//            s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
//                ->SendTcpPacket((char*)&pkt, 0x2b);
//            PacketGuard pg: clear; put_header(0,0xf9); put_byte(flag);
//                finalize(true); party->send_to_party(pg);
// [B24D判词 N] 语义正确：pkt 全部清零 + header 0x1772/0x2b + 循环填 uid/characNo(4) +
//   OnEvent()&&flag→IncVillageAttackCount + m_flag=flag + SendTcpPacket 0x2b + PacketGuard
//   0xf9 发送。差异=ORIG 用 ctor W 0x86b4b84 一次性 memset 全结构，我方显式
//   memset(m_pad4)+双循环清0（多 ~27 条），纯 lowering 表示差异，语义等价。
// [M2S4 2026-09-11 本波归零] 114/114（ORIG 114，逐条同形；p167 19/19，余 C1↔C2/
//   D1↔D2 同址别名对）。两处真因：
//   ① 具名 `CMonitorServerProxy* monitorProxy` 缓存（P0/P183）——ORIG 0x086b33e4/
//      0x86b33f0 结果经 %eax 直传实参位，无槽；删除后整帧槽归位（-0x4b/-0x41/-0x21）。
//   ② 第二个早退 `if (party == 0) return;` → 改正向包裹 `if (party != 0) {...}`：
//      ORIG 0x086b3334 `je 086b3498`（party==0 直落 epilogue）+ 0x086b331c `je
//      086b3497`（user==0 落 nop 垫）；否定早返形让 4.4 多 emit `nop;jmp` 尾垫
//      （探针 /tmp/m2s4_tail.cpp：fA 4 条尾垫 vs fB 2 条尾垫）。
// ============================================================================
// [round2-L30 复核·N/B24D] 残余 3 条 + 整帧平移 4：ORIG ctor W 0x86b4b84 一次性 memset
//   全结构 vs 我方显式清零 lowering（语义等价）；ORIG 0x086b330a。
void CVillageMonster::SendVillageMonsterFightResult(CUser* user, bool flag)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    if (user == 0)
        return;
    CParty* party = (CParty*)user->GetParty();
    // [P222/尾岛] ORIG 0x086b3334 `je 086b3498`（party==0 直落 epilogue）+
    //   0x086b331c `je 086b3497`（user==0 落 nop 垫）—— 第二个早退须写
    //   **正向包裹**：否定早返形 `if (party == 0) return;` 让 4.4 多 emit
    //   `nop;jmp`（探针 /tmp/m2s4_tail.cpp fA 4 条尾垫 vs fB 2 条尾垫，
    //   116 vs ORIG 114 的最后 2 条即此）。
    if (party != 0)
    {
        Packet_VillageMonsterFightResult pkt;
        // ORIG ctor 0x86b4b84：PacketHeader(0x1772,0x2b) + m_flag=0 + m_keys[8] 清零。

        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = party->get_user(i);
            if (member != 0)
            {
                pkt.m_uids[i] = member->GetUID();
                pkt.m_characNos[i] = member->get_charac_no(-1);
                if (GlobalData::s_villageMonsterMgr->OnEvent())
                    if (flag)
                    member->IncVillageAttackCount();
            }
        }
        pkt.m_flag = flag;

        // [P0/P183] ORIG 0x086b33e4..0x086b33ff：proxy 结果经 %eax 直传实参位，
        // 无具名局部槽 ⇒ 不得具名缓存（曾多 `mov %eax,-0x14` + `mov -0x14,%eax` 2 条
        // 并使整帧槽平移 4）。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendTcpPacket((char*)&pkt, 0x2b);

        PacketGuard pg;
        pg.clear();
        pg.put_header(0, 0xf9);
        pg.put_byte(flag);
        pg.finalize(true);
        party->send_to_party(pg);
    }
}