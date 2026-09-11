// ==== Dispatcher_Teleport::check_error (ORIG 0x081d056c，三参形态) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d056c orig/ours=169/167 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: je     081d063c <+0xd0> | mov    -0x14(%ebp),%eax | mov    0x2(%eax),%eax
unsigned int Dispatcher_Teleport::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [b23S3C] ORIG 0x081d056c：w3 ||短路岛形（CallGuildAllMembers probe 同证）；去
    // slot/invenType/itemIdx/vill/state/teleport 值槽与 param 早缓存（P0：ORIG 槽仅
    // pm/ref/party/out），money 三比较与两实参处每点重取 ref->m_addInfo/req 字段
    // （P48）；party 段 teleport/state 双现取（ORIG 2 组 call）；out 声明置 party 块后。
    // 同口径 fresh .o 复验 163→106（HEAD 值槽版更差）。残余=4.1 &&链 stub 重复比较布局。
    MSG_TELEPORT& req = reinterpret_cast<MSG_TELEPORT&>(msg);
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return -1;
    if (user->get_state() > 4)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf1), 0x13);
        return -1;
    }
    Inven_Item* ref = const_cast<Inven_Item*>(
        user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<int>(req.m_field14)),
            (int)req.m_field0e));
    if (ref == 0)
    {
        return -2;
    }
    if (ref->m_addInfo != 0x27ac4e && ref->m_addInfo != 0x28e7d0 &&
        ref->m_addInfo != 0x29f288)
    {
        return -2;
    }
    if (static_cast<char>(req.m_field15) < 1)
    {
        return 0x7fffffff;
    }
    if (user->CheckMoveTown(static_cast<int>(static_cast<char>(req.m_field15))) != 0)
    {
        return 0x7fffffff;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        if (static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->get_teleport_state() >= 0 &&
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->get_teleport_state() <= 3)
        {
            return 0x9e;   // 组队传送进行中
        }
    }
    stTeleportParam* const out = static_cast<stTeleportParam*>(&param);
    // [G3 2026-09-11] P211 同型化：Inven_Item::m_addInfo 为 unsigned int（Inven_Item.h:269），
    //   stTeleportParam::m_14 为 int ⇒ 异型 store 多 `mov %eax,%edx` 中转。
    //   ORIG 0x081d071d `mov 0x2(%eax),%edx; mov -0xc(%ebp),%eax; mov %edx,0x14(%eax)` 单段直落。
    *reinterpret_cast<unsigned int*>(&out->m_14) = ref->m_addInfo;
    // ORIG 0x081d0726：use_stackable((short)m_field0e, (signed char)m_field14,
    // -1, -1)，失败返 0x11。
    if (!user->use_stackable((short)req.m_field0e,
            static_cast<int>(req.m_field14), -1, -1))
    {
        return 0x11;
    }
    // ORIG 0x081d0769：expert_job::CAlchemist::OnTeleportCharacter(user, (signed char)vill,
    // 0, 0, 0, true)，失败返 0x7fffffff。
    if (!expert_job::CAlchemist::OnTeleportCharacter(
            user, static_cast<int>(req.m_field15), 0, 0, 0, true))
    {
        return 0x7fffffff;
    }
    return 0;
}