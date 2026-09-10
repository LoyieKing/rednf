int Dispatcher_ChangeCharacName::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() != 2)
        return LineFunc(0x9eb3,
                        __PRETTY_FUNCTION__,
                        0, 0);

    unsigned char slot = 0;                              // ORIG -0x65
    int newLen = 0;                                      // ORIG -0x6c
    int srcLen = 0;                                      // ORIG -0x70
    char srcName[0x1e] = { '\0' };                       // ORIG -0x8e（P4 零初始化展开）
    char newName[0x1e] = { '\0' };                       // ORIG -0xac
    if (!packet.get_byte(slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    bool srcOk = packet.get_int(srcLen);                 // ORIG local_25
    if (!srcOk)
    {
        // [SigB] P227 全表达式临时链：ORIG 0x08202xx 在 cMyTrace ctor **之前**求值
        // varargs 到 %edi/%esi/%ebx 并跨 ctor 保活（原两句式把 ctor 提前、varargs 求值
        // 推后，且不产生 ORIG 的 `push %edi`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x9ec7, 0)(
            "ting : m_id(%s) src char name length(%d), ret(%d)",
            NumberToString(user->get_acc_id(), 0), srcLen, (int)srcOk);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (!packet.get_str(srcName, 0x1e, srcLen))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    bool newOk = packet.get_int(newLen);                 // ORIG local_25（第二次）
    if (!newOk)
    {
        // [SigB] P227 同上（ORIG 在 ctor 前 `movzbl ..,%edi; mov ..,%esi; mov %eax,%ebx`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x9ed4, 0)(
            "ting : m_id(%s) char name length(%d), ret(%d)",
            NumberToString(user->get_acc_id(), 0), newLen, (int)newOk);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (!packet.get_str(newName, 0x1e, newLen))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (newLen < 4 || 0x12 < newLen)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 1);
        return 0;
    }

    const char* serverName = user->get_charac_name((int)slot);
    if (serverName == 0 || strncmp(srcName, serverName, 0x1d) != 0)
    {
        if (serverName == 0)
        {
            // [SigB] P227：ORIG 在 ctor 前 `mov %eax,%ebx`（NumberToString 结果保活）。
            cMyTrace(__PRETTY_FUNCTION__, 0x9ef6, 0)(
                "ting : m_id(%s) srcCharacName2 is null",
                NumberToString(user->get_acc_id(), 0));
        }
        else
        {
            // [SigB] P227：ORIG varargs (srcName, serverName) 于 ctor 前就绪。
            cMyTrace(__PRETTY_FUNCTION__, 0x9ef8, 0)(
                "ting : src char name(%s), src server char name(%s)",
                srcName, serverName);
        }
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }

    // [DPC-SigC 2026-09-11] P328⑤ 去 `=0`：ORIG 全函数无 `movb $0x0,-0xad(%ebp)`
    // （objdump 实证该槽仅出现 `lea -0xad(%ebp),%eax`@+0x10a 与
    // `movzbl -0xad(%ebp),%eax`@+0x11a，无零初始化）；我方零初始化多 1 条
    // （LCS insert OUR[266:267] `movb $0x0,-0xad(%ebp)`）。get_byte 失败路径
    // 直接 return LineFunc，不读该值 ⇒ 语义不变。预期 ours 421→420。
    unsigned char typeFlag;                              // ORIG -0xad（无初始化）
    if (!packet.get_byte(typeFlag))
        return LineFunc(0x9eff,
                        __PRETTY_FUNCTION__,
                        0, 0);

    int nameState;                                       // ORIG -0x1c
    if (typeFlag == 2)
    {
        // ORIG hasPreventPrefixCharacName(src, srcLen)（Ghidra 误抄参数，asm 实证）。
        // [Q5BigTURest.DisPatcherCore 2026-09-11 极性轴·落地] ORIG 0x0820598x 段为
        // `call; test %al,%al; je <失败块>` + **成功路 fall-through**
        // （`movl $0x2,-0x1c`）⇒ 原码是正向 if/成功先；我方原 `if (!f()) { 失败; return; }`
        // 令 4.4 产 `xor $0x1,%eax; test; je` + 失败块前置（本件 diff 块 4→3、
        // xor 计数 +1→0；TU 四数零劣化）。残差=槽 -0x21/-0x22 与 %esi 保活（N7）。
        if (G_CDataManager()->hasPreventPrefixCharacName(srcName, srcLen))
        {
            nameState = 2;
        }
        else
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
            return 0;
        }
    }
    else
    {
        if (G_CDataManager()->hasSpecialCharacter(serverName))
            nameState = 0;
        else if (G_CDataManager()->hasPreventString(serverName) ||
                 G_CDataManager()->hasPreventStringName(serverName))
            nameState = 1;
        else
        {
            if (!user->CheckPossibleRequestRenameState((int)slot))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
                return 0;
            }
            if (!checkValidCharacName(newName))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
                return 0;
            }
            user->SetCharacViewState((int)slot, 1);
            nameState = 3;
        }
    }

    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(newName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
        return 0;
    }
    if (!checkValidCharacName(newName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
        return 0;
    }
    DB_ReqChangeCharacName::makeRequest((int)user->GetUID(),
                                        user->get_charac_no((int)slot),
                                        newName, nameState);
    return 0;
}
