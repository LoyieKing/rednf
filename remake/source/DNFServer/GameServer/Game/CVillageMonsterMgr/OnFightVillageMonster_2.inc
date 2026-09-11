// ============================================================================
// CVillageMonsterMgr::OnFightVillageMonster(CUser*,unsigned short) —— ORIG 0x86b4672
//   szZone 未命中或队伍缺失 → 0x15；区域 OnFight 失败原样返回；成功则把
//   怪物 m_field20/m_field24 写入队伍村战状态区（+0xcb8/+0xc58）、zon/ vill/area
//   落位（+0xc4c/+0xc50/+0xc54），并向队伍广播 0xf3，返回 0。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] N 封顶（ORIG 0x086b4672，N7/N82 族）：
// 去 z/area/vill 具名局部（P0 右值直传）+ 去 view 指针后槽集与 ORIG 全等、
// 帧 0x60=0x60；残余 = 两个 `return 0x15` 被 4.4 汇聚到 %ebx（ORIG 各自
// `mov $0x15,%eax`）+ party 二次使用被缓存 %ebx（ORIG 每次重载 -0x14），
// 共 4 条寄存器分配差。p167 CLEAR（19/19）。
// [M5E round6 判据 ③ 已改 · 旧判词 N7/N82 被推翻] 4 条「寄存器分配差」的真因是
//   **LHS 基址表达式的 tree 形**：逐句 `((View*)party)->m = rhs` 的 NOP_EXPR 会被
//   4.4 在 RHS 之前物化（旧码 +0x127/+0x146 `mov -0x14(%ebp),%ebx` 在 call 前），
//   而 ORIG 的基址是**普通具名局部**（+0x12b/+0x147/+0x153/+0x15f 四处
//   `mov -0x14(%ebp),%eax/%edx` 全在 call **之后**）⇒ ORIG 的 `party` 本身即视图
//   类型（cast 只在 GetParty 赋值处做一次）。探针实证：/tmp/m5e_lhs_probe.cpp
//   形 B（具名 `V* v = (V*)p;` 后 `v->m = call();`）= `call; mov -0xc(%ebp),%eax;
//   mov %edx,0xc50(%eax)`（RHS-first，与 ORIG 同形）；形 A（逐句 cast）= `call` 前
//   `mov 0x8(%ebp),%ebx`（旧码形）。改后槽集/帧不变（仍是 -0x14 一个 4B 局部）。
// [P6S5 2026-09-11 本波负结论] 末句 `((CParty*)party)->send_to_party(packet)` 的
//   4 行残差 = 「实参求值序」（ORIG 先 `lea -0x34(%ebp),%eax; mov %eax,0x4(%esp)`
//   求第 2 参，再 `mov -0x14(%ebp),%eax` 装 this；我方相反）。本波探针：
//   加具名 `CParty* partyObj = (CParty*)party; partyObj->send_to_party(packet);`
//   ⇒ 读数**劣化** 136→138（gap +0→−2）且槽漂移（-0x1a→-0x1e）⇒ 已回滚。
//   4.1 先实参 / 4.4 先 this 属寄存器分配族，源形不可造。
int CVillageMonsterMgr::OnFightVillageMonster(CUser* user, unsigned short zone)
{
    Zone findZone(user->getCurCharacVill(),
                  (unsigned char)user->get_area(false));
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(findZone);
    if (it == m_areas.end())
        return 0x15;
    CPartyVillageStateView* party =
        (CPartyVillageStateView*)user->GetParty();
    if (party == 0)
        return 0x15;
    CVillageMonsterArea* areaObj = &it->second;
    int out1 = 0;   // → monster.m_field20 → party+0xcb8
    int out2 = 0;   // → monster.m_field24 → party+0xc58
    int ret = areaObj->OnFightVillageMonster(user, zone, out1, out2);
    if (ret == 0)
    {
        party->m_villageMonsterZone = zone;
        party->m_villageVill = (int)(signed char)user->getCurCharacVill();
        party->m_villageArea = user->get_area(false);
        party->m_villageMonsterField24 = out2;
        party->m_villageMonsterField20 = out1;
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0xf3);
        packet.finalize(true);
        ((CParty*)party)->send_to_party(packet);
    }
    return ret;
}