// [b28d-solo-CCargo] N 封顶（fresh 184/185；唯余 1 条 mov 中转）——已翻正：
// ①删 buf 指针局部（P183）；②删 world 局部 + IsEnchantRevisionChannel 正极性
// （ORIG `call; test %al,%al; je <m_fieldd>`）；③GetItemCount 保持 int（LHS-first 直存）。
// 残差 = GetCapacity 调用点：ORIG `mov 0xc(%ebp),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)`
// vs 4.4 `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x4(%esp); mov %eax,(%esp)`。
// 归因 P248 档①两端冲突：callee 返回类型是唯一旋钮——改 int 可翻正此点（同函数
// GetItemCount int 处实测 LHS-first；unsigned 化后两处同变 RHS-first，已回退），但
// CUser.cpp:10586 `sig->m_cargoCapacity = GetCapacity()`（SigTypes m_cargoCapacity=
// unsigned int，+0xa8e0）需 unsigned 才同型直存（b28d-solo-CUser 定案）⇒ 禁单段翻正。
// 实测 (int) / (int)(unsigned int) / P226 语句表达式三形均无效。语义全等，byte 层 N 封顶。
bool CCargo::MakeItemList(PacketGuard* packet) const
{
    // [L5 R2 2026-09-11] 返回类型 int→bool 已落（Main 授权）：头 CUserCharacInfo.h:308 +
    // 本件签名 + 尾 `return true;`。直证：ORIG 定义 0x0850bee2 尾 `mov $0x1,%eax`（无 setcc）；
    // 调用点 CUser::send_itemspace 0x0865dc1a `call` → 0x0865dc1f `xor $0x1,%eax;
    // test %al,%al; je`（P292）。返回类型不入 mangled、sizeof/offsetof 零影响。
    // 本体残余：orig 184 / our 185（gap +1）——唯一多出条为 0x0850bf41 处
    // `mov %eax,%edx`（value 中转）：ORIG `mov 0xc(%ebp),%edx; mov %eax,0x4(%esp);
    // mov %edx,(%esp)`，4.4 对 `put_short(GetCapacity())`（unsigned 返回值 × int 形参）
    // 走「值先入 edx」；同函数 `put_short(GetItemCount())`（int 返回值）与 ORIG 逐条同形。
    // GetCapacity 不可改 int（CUser.cpp:10586 `sig->m_cargoCapacity=...` 需 unsigned 同型直存）。
    // 已试 3 形无效：(int) cast / (int)(unsigned int) / P226 语句表达式 ⇒ P321 T5 记 N。
    // [b28d-solo-CCargo] P183：ORIG 无 buf 具名指针（每处 `mov 0xc(%ebp),%eax` 直用
    // 形参）；P0：无 world 局部（`call G_GameWorld; mov %eax,(%esp); call IsEnchant…`
    // 右值内联），且正极性 `if (IsEnchantRevisionChannel())`（ORIG `test; je <m_fieldd>`）。
    ((InterfacePacketBuf*)packet)->put_header(0, 0xd);
    ((InterfacePacketBuf*)packet)->put_byte(2);
    // [b28d-solo-CCargo] ORIG 0x0850bf41 `mov 0xc(%ebp),%edx; mov %eax,0x4(%esp);
    // mov %edx,(%esp)`（receiver 入 edx、值留 eax）；4.4 恒 `mov %eax,%edx;
    // mov 0xc(%ebp),%eax`（值入 edx）⇒ 唯余 1 条 N（N7/N18a 族寄存器角色）。
    // P248 档①两端冲突：callee 返回类型是唯一旋钮——改 int 可翻正此点（同函数
    // GetItemCount int 处实测 LHS-first），但 CUser.cpp:10586
    // `sig->m_cargoCapacity = GetCapacity()`（SigTypes m_cargoCapacity=unsigned int）
    // 需 unsigned 才同型直存（b28d-solo-CUser 定案）⇒ 禁单段翻正，封 N。
    // 实测：(int) cast / (int)(unsigned int) / P226 语句表达式三形均无效；
    // GetItemCount unsigned 化后两处同变 RHS-first（已回退）。
    // [PoolH 2026-09-11 · Main 新范式（接收者去 cast）实测**反劣回滚**] 本件接收者虽为
    // cast 表达式（`((InterfacePacketBuf*)packet)->`），但 ORIG 侧本点**无具名局部**
    // （每处 `mov 0xc(%ebp),%eax` 直用形参，P183 已证）⇒ 改「具名类型化局部」
    // （`InterfacePacketBuf* capBuf = …; capBuf->put_short(GetCapacity());`）后
    // 184/186（+2：`mov %eax,-0x10(%ebp)` 槽物化）且 this 序仍非 ORIG 形 ⇒ 立即回滚。
    // 范式适用画像（ORIG 有 const-first + 共具名局部）与本点不符，维持 P248 档①/P321-T5 封顶。
    ((InterfacePacketBuf*)packet)->put_short(GetCapacity());
    ((InterfacePacketBuf*)packet)->put_short(GetItemCount());
    for (int i = 0; i < 0x98; ++i) {
        if (m_pItems[i].m_addInfo != 0) {
            ((InterfacePacketBuf*)packet)->put_short(i);
            ((InterfacePacketBuf*)packet)->put_int(m_pItems[i].m_addInfo);
            ((InterfacePacketBuf*)packet)->put_int(m_pItems[i].m_addInfo2);
            ((InterfacePacketBuf*)packet)->put_byte(
                (int)GetIntegratedPvPItemAttr(m_pItems[i]) & 0xff);
            ((InterfacePacketBuf*)packet)->put_short(m_pItems[i].m_endurance);
            ((InterfacePacketBuf*)packet)->put_byte(m_pItems[i].m_seal);
            if (G_GameWorld()->IsEnchantRevisionChannel())
                ((InterfacePacketBuf*)packet)->put_int(0);
            else
                ((InterfacePacketBuf*)packet)->put_int(m_pItems[i].m_extendInfo);
            ((InterfacePacketBuf*)packet)->put_byte(
                m_pItems[i].m_amp.getAbilityType() & 0xff);
            ((InterfacePacketBuf*)packet)->put_short(
                m_pItems[i].m_amp.getAbilityValue() & 0xffff);
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, m_pItems[i], packet);
            ((InterfacePacketBuf*)packet)->put_packet(m_pItems[i]);
        }
    }
    ((InterfacePacketBuf*)packet)->finalize(true);
    return true;
}