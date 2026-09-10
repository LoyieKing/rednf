void Dispatcher_RefundSkill::send(CUser* user, ParamBase& param)
{
    RefundParamView& v = *reinterpret_cast<RefundParamView*>(&param);
    // ORIG 0x081dabxx：`test %eax,%eax; jne <guard 段>` —— 正常（err==0）路径 fall-through，
    // SendCmdErrorPacket 上报体在**函数物理尾**（技巧17/23，同 SkillInit::send）。
    if (v.m_err == 0) {                                         // +0x18 错误码
        PacketGuard pg;
        pg.clear();
        pg.put_header(1, 0x1ab);
        pg.put_byte(1);                                         // 成功标志
        pg.put_byte(static_cast<int>(v.m_special));                    // +0x05
        pg.put_byte(static_cast<int>(v.m_treeIndex));               // +0x04
        pg.put_short(static_cast<int>(v.m_remain));             // +0x06
        pg.put_byte(static_cast<int>(static_cast<unsigned char>(v.m_result)));   // +0x08 低字节
        pg.put_byte(static_cast<int>(v.m_kind));                // +0x0c
        pg.put_byte(static_cast<int>(v.m_level));               // +0x0d
        pg.put_int(v.m_itemIdx);                                // +0x10
        pg.put_byte(static_cast<int>(v.m_count));               // +0x14
        pg.finalize(true);
        user->Send(pg);
    }
    else {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1ab),
                                 static_cast<unsigned char>(v.m_err & 0xff));
    }
