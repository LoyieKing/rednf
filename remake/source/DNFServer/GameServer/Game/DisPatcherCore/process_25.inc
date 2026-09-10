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
    // [G5-DisPatcherCore] P52/P183：ORIG 0x081c61xx 段先把 3 个 msg 字段读入
    // %edi/%esi/%ebx 跨 `call G_Store` 保活，receiver 表达式最后求值 ⇒ 原码无具名
    // `Store* store` 缓存（我方原形把 G_Store 提到语句前，实参退化为 caller-saved）。
    out.m_errNo = G_Store()->repair_equip(user, (char)req.m_0d, req.m_0e, req.m_10,
                                          out.m_c);  // 出参 ushort& @ param+0xc（ORIG asm -0x2c(%ebp)）
    if (out.m_errNo == 0)
    {
        out.m_8 = req.m_0d;                    // movzbl 0xd(%eax) -> mov %dl,0x8
        // [G5-DisPatcherCore] 尝试去掉 (unsigned short) cast：ORIG `movzwl 0xe(%eax),%edx`
        // 单步 vs 我方 `movzwl …,%eax; mov %eax,%edx` 两段；实测两种写法产物全同
        // ⇒ 属 N18a（单段 regmove 4.4 恒折/反向不可造），维持原 cast 形并封顶。
        out.m_a = (unsigned short)req.m_0e;    // movzwl 0xe(%eax) -> mov %dx,0xa（原位型回显）
    }
    return 0;
