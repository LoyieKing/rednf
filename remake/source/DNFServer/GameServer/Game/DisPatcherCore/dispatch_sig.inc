int DisPatcher_CheckConn::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ea900 T（报告 DisPatcher_CheckConn/dispatch_sig.md 全文转写）：
    // decre_check_count → invenCk/skillCk 校验（hack 计数 0x25b/0x25c）→ CHades Send_ChangeGold。
    user->decre_check_count();
    int invenCk = 0;  // ORIG 0x081ea900+0x?: decre_check_count 后 movl $0x0,-0xc(%ebp)
    if (!packet.get_int(invenCk))
    {
        return LineFunc(0x6bf8, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacInvenCheckSum() != invenCk)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    int skillCk = 0;  // ORIG addServerHackCnt(0x25b) call 后 movl $0x0,-0x10(%ebp)
    if (!packet.get_int(skillCk))
    {
        return LineFunc(0x6bff, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacSkillCheckSum() != skillCk)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
    }
    static_cast<XNuclear::CHades*>(user->getHades())->Send_ChangeGold();
    return 0;
