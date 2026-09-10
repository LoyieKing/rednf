bool CCreatureMgr::RenameCreature(const char* name, int creatureIdx, int slot)
{
    CPacketRespondent respondent(m_user, 1, 0x67);
    // ORIG 0x8339bb4 `movl $0x0,-0xc(%ebp)`：useRet 函数头预清（P198①）。
    int useRet = 0;
    if (m_equippedCreature == 0)
    {
        respondent.SetError(4);
        return false;
    }
    // ORIG 0x8339bc4 段 getCurCharacInvenW 结果直用（无具名 inven 槽）⇒ 内联链。
    useRet = m_user->getCurCharacInvenW()->use_item(creatureIdx, 1, slot);
    // [P225 试形] ORIG 0x8339bf4 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`：
    // 4.1 槽比零 bool 物化，4.4 对 if 恒折 cmp/jne 直跳 ⇒ while+return 形。
    while (useRet != 0)
    {
        respondent.SetError(useRet);
        return false;
    }
    m_equippedCreature->RenameCreature(name);
    // P0/P183：ORIG 的 len/area/vill/uid 均无槽往返（Ghidra sVar2/iVar3/cVar1
    // 复用寄存器）⇒ 表达式直传，禁具名中间局部。
    respondent.MakeNotipacketRenameCreature(name, (int)strlen(name));
    respondent.Broadcast((int)(char)m_user->getCurCharacVill(),
                         m_user->get_area(false));
    CDBMsgSender sender((ENUM_INTERNALPACKET)0x8a, m_user);
    // ORIG 0x8339ccd 段 `xor $1;test;je <成功>`：失败块 fall-through、判定内联。
    if (!sender.RenameCreatureItem(
            reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetUid(), name))
    {
        cMyTrace(
            "bool user_creature::CCreatureMgr::RenameCreature(const char*, int, int)",
            0xaa0, 5)("cannot write db uid(%d)\n",
                      m_user->get_unique_id() & 0xffff);
        respondent.SetError(2);
        return false;
    }
    sender.Send();
    respondent.MakeCmdpacketRenameCreature(creatureIdx, slot);
    return true;
}
