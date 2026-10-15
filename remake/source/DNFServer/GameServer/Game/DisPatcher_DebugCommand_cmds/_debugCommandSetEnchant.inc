// ===========================================================================
// 12. _debugCommandSetEnchant —— 0x0858fb4e
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] N104/N93附封顶（EH 帧/寄存器策略）：
// 删段实验证明——去掉 PacketGuard 段后 4.4 与 ORIG 完全同形（push %ebx/sub 0x24、
// invenType -0x10、slotInt→%ebx）；带 PacketGuard EH landing 时 4.4 恒多用
// %esi/%edi 并把标量槽加深 0x10（帧 0x3c vs ORIG 0x30）。已修：getCurCharacR
// `!=0` 正向包裹（P222/P214①）。P167 callsite CLEAR 16/16。
bool DisPatcher_DebugCommand::_debugCommandSetEnchant(CUser* user,
                                                      MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;
    // ORIG `test %eax,%eax; setne %al; test %al,%al; je <尾 mov $0>` ⇒ 正向包裹
    // （P222/P214①；负极性 `==0 return false` 反产 sete）。
    if (user->getCurCharacR() != 0)             // +0x2e getCurCharacR!=0 才续
    {
        int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)msg.m_arg);   // +0x42 itemspace→type
        Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(
            invenType, msg.payload.ench.slotInt);               // +0x6c GetInvenRef(space,slot)
        if (ref != 0)                                           // +0x74
        {
            ref->m_extendInfo = msg.payload.ench.enchantVal;        // +0x86 写 +0xd 整体 int

            PacketGuard packet;                                 // -0x1c
            packet.put_header(0, 0xe);                          // +0xad header(0,0xe)
            packet.put_byte(msg.m_arg);                         // +0xc2 put_byte(itemspace)
            packet.put_short(1);                                // +0xdf put_short(1)
            // [b28e] ORIG 0x0858fc4e 调用点为 `call MakeItemPacket; xor $0x1,%eax;
            // test %al,%al; je 0858fc61`（否定式直推导数，P292/P248 档①）且失败
            // 分支是 `mov $0x0,%ebx; jmp 0858fca8`（早返 false 经 ebx 汇聚到
            // 析构入口）⇒ 原码是 **`if (!MakeItemPacket(…)) return false;` 早返
            // 形**；写 `if (MakeItemPacket(…)) {…; return true;}` 会反产
            // `test; je <失败>` 并把返回值物化到第二个 callee-saved（%esi/%edi，
            // 帧 0x30→0x3c、多 3 条）。
            if (!user->getCurCharacInvenR()->MakeItemPacket(    // +0x100 MakeItemPacket
                    static_cast<INVEN_TYPE>(invenType),
                    msg.payload.ench.slotInt, packet))
            {
                return false;
            }
            packet.finalize(true);                              // +0x133 finalize
            user->Send(packet);                                 // +0x14d Send
            return true;
        }
    }
    return false;                                               // 失败析构后返 0
}