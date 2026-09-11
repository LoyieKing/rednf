// ============================================================================
// CheckHaveHellPartyPassItem —— ORIG T 0x085a7b8a（报告 CParty/CheckHaveHellPartyPassItem.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 负极性短路单 return `!A || B`（P214①/P258）
// + 成功分支正向 if（P59/P222）+ callee CheckDestroyConditionSealDoor 返回 bool
// （P292 2 条形）+ m_sealDoorFlag bool + 本函数返回 bool
// （P9 movzbl 直返；.h:493 已由父泳道 R2 落 bool，本 TU .inc 同步）。
// [b28d-round5 残余 1 条 · .h 类型轴已裁决落地] ORIG 0x085a7d48 段
// `mov 0x8(%ebp),%eax; movzbl 0x1865(%eax),%ebx`（单条直入返回值寄存器 %ebx）；
// 我方 4.4.7 恒经 %eax 中转（多 1 条 `mov %eax,%ebx`）。探针 /tmp/probe_party/
// s.cpp（带 EH 局部对象）四形实证：h4 bool 成员+bool 返回 ⇒ 单条；h1 bool 成员+
// unsigned char 返回 ⇒ 两条；h2 unsigned char 成员+uchar 返回 ⇒ 单条；h5
// `return p->f != 0` ⇒ 两条。⇒ 正解是本函数**返回类型 bool**：
// CParty.h:493 `unsigned char CheckHaveHellPartyPassItem(CDungeon const*,
// ENUM_DUNGEON_TYPE)` → `bool`（父泳道已落地；本 TU 定义侧同步）。成员侧保持 bool 不动
// （BattleDungeonMapA2.cpp:82 ④ 以 ORIG `movzbl;xor $1;test;je` 取反形直证
// m_sealDoorFlag 是 bool，改 unsigned char 会回退该 TU ⇒ 冲突项，仅返回类型可动）。
// 返回类型不入 mangled；调用点 dungeon_start 为 `if (CheckHaveHellPartyPassItem
// (dungeon, type))`，bool/uchar 的 movzbl+test 形态相同（零回退预期）。
// ============================================================================
// [b28d-round5] 返回类型 unsigned char→bool 与 CParty.h:493 同步（父泳道 R2 落地）：
// ORIG 0x085a7d4b `movzbl 0x1865(%eax),%ebx` 单条直落 ⇒ bool 成员+bool 返回无转换
// 临时；探针 /tmp/probe_party/s.cpp h4（bool 返回，单条）vs h1/h5（uchar 返回，两条）。
bool CParty::CheckHaveHellPartyPassItem(CDungeon const* dungeon,
                                                ENUM_DUNGEON_TYPE type)
{
    if (m_hellWorldMap == 0)
    {
        return true;
    }
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7bba 段：`call; xor $0x1; test %al;
    // jne <mov $1 岛>` + `cmpl $0x1,0x10(%ebp); je <mov $0 岛>` ⇒ 原码是负极性
    // 短路单 return `!A || B`（P214①/P258），非具名 bool skip——我们的
    // -0x9 槽 + movb 写即该多出的局部（P0 槽-局部律）。
    if (!m_hellWorldMap->IsInHellDungeon() || type != 1)
    {
        return true;
    }

    std::vector<int> sealCondItems;
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c15 `call; test %al,%al;
    // je <packet 块>` ⇒ 成功分支（m_sealDoorFlag=1）在 then、packet 块为
    // 直落 else（P59/P222 正向布局），勿写 `if (!call())`。
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c15 `call; test %al,%al; je`
    // （2 条，无 setne 物化）⇒ 被调返回 bool（P292），头已同步改 bool。
    if (CheckDestroyConditionSealDoor(dungeon, sealCondItems,
                                      m_hellWorldMap->m_field34))
    {
        m_sealDoorFlag = 1;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_byte(0x1b);
        packet.put_byte(static_cast<int>(sealCondItems.size()));
        // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c8e 段 begin/end/operator!= 均
        // 非 const 实例（无转换构造 call）⇒ P96/P270 用非 const iterator。
        std::vector<int>::iterator it = sealCondItems.begin();
        while (it != sealCondItems.end())
        {
            packet.put_short(*it);
            ++it;
        }
        packet.finalize(true);
        send_to_party(packet);
        m_sealDoorFlag = 0;
    }
    return m_sealDoorFlag;
}