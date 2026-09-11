// ============================================================================
// RenameCreature @ 0x08339b6e（RenameCreature.md）
//   装备宠物非空 → use_item 消耗改名书（creatureIdx×1，slot 空间）→ 宠物改名 +
//   改名通知广播 → CDBMsgSender::RenameCreatureItem 落库 → 成功 Send+回执包，
//   失败 trace+SetError(2)。use_item 失败按返回错误码 SetError。
// ============================================================================
// [b28c-30 SIGFIX] void→bool（P174 串定 + Ghidra C `return bVar5`）：失败路径
// 返 false、成功路径返 RenameCreatureItem 的 ok；mangled 不含返回类型。
// [b28d-solo-CCreatureMgr N] ndiff=2：已翻正 ①useRet 函数头预清（P198①）；
// ②len/area/vill/uid/inven 五具名局部全部内联（P0/P183，帧 0x70→0x50 与 ORIG 一致）；
// ③RenameCreatureItem 内联判定+早返（P194）、cMyTrace 临时链（P227）。
// 唯一残余 = ORIG 对槽比 `useRet != 0` 多 `setne %al; test %al,%al` 物化岛
// （4.4 恒直 je，setcc-fold 族 N；探针 `if(useRet)`/`(bool)useRet` 同形）。
// [b28d-solo-CCreatureMgr AE] P225 while+return 形复现 4.1 槽比零 bool 物化
// （ORIG 0x8339bf4 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`）；
// 162=162，compare_tu DIFF→AE。ORIG 0x08339b6e
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
        // [R2PrettAClass 2026-09-11 · prett A 类落地] 手写串 → __PRETTY_FUNCTION__
        //   （ORIG .rodata @0x08c2f260 size 0x48 = "bool user_creature::CCreatureMgr::"
        //   "RenameCreature(const char*, int, int)"）。
        cMyTrace(
            __PRETTY_FUNCTION__,
            0xaa0, 5)("cannot write db uid(%d)\n",
                      m_user->get_unique_id() & 0xffff);
        respondent.SetError(2);
        return false;
    }
    sender.Send();
    respondent.MakeCmdpacketRenameCreature(creatureIdx, slot);
    return true;
}
