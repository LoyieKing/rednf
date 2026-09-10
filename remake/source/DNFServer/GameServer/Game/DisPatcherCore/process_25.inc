int DisPatcher_RepairEquip::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x19, (unsigned char)(errNo & 0xff));
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x1500,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    MSG_REPAIR_EQUIP& req = *reinterpret_cast<MSG_REPAIR_EQUIP*>(&msg);
    stRepairEquipParam& out = static_cast<stRepairEquipParam&>(param);
    Store* store = G_Store();  // ORIG thunk 0x8151add = _Z7G_Storev
    out.m_errNo = store->repair_equip(user, (char)req.m_0d, req.m_0e, req.m_10,
                                      out.m_c);  // 出参 ushort& @ param+0xc（ORIG asm -0x2c(%ebp)）
    if (out.m_errNo == 0)
    {
        out.m_8 = req.m_0d;                    // movzbl 0xd(%eax) -> mov %dl,0x8
        out.m_a = (unsigned short)req.m_0e;    // movzwl 0xe(%eax) -> mov %dx,0xa（原位型回显）
    }
    return 0;
