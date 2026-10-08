// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x08202cda orig/ours=430/395 ndiff-blocks=64
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 7 个，4.4 libstdc++ 全内联不可复现
//   首差: push   %edi | sub    $0xbc,%esp | jne    08202d0c <+0x32>
// [SweepDispatchers 2026-09-09] 重载选型真缺口修复：dollSlot 读取 ORIG 0x0820324c 调
//   get_short(unsigned short&)（Rt），旧码 `(short&)dollSlot` 误选 Rs ⇒ p167 MISSING/EXTRA
//   各 1；字段本型直传后该对清零（残余为 N89 STL 内联族 + C1/C2 别名）。
// [Q4DispatchBig 2026-09-11 留痕] 本件在 call 轴上仍缺 1 条 `_ZNSsC1EPKcRKSaIcE`
//   （ORIG 0x08202ea8 该函数内有该 call，属**函数级**真缺口）；因「首用处拷贝构造形」
//   （`std::string nameStr(subject);` 下移到 +0x1af）实测 ours 424→438（gap −7→−8）
//   劣化、栈槽/临时链被打乱，按 codegen 轴闸门保留「入口声明 + 赋值」现状。
//   后续审计勿当新发现（p167 该件另 2 条 `_Unwind_Resume`/`_ZNSaIcED*Ev` 为伪符号族假缺口）。
int Dispatcher_CompleteDisplay::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08202cda（docs/class_func_reports/Dispatcher_CompleteDisplay/dispatch_sig.md）
    // [P4 负结论 2026-09-11 实测回滚] 把 nameStr 改为「首用处拷贝构造」实测 ours 423→438 条
    //   （⇒ gap −7→−8 劣化，栈槽/临时链被打乱）⇒ 保留入口声明 + 赋值形。
    std::string nameStr;                      // local_44
    private_store::ISeller* seller = 0;       // local_3c
    private_store::PrivateStoreItem item;     // local_bc
    unsigned char itemCount = 0;              // local_36 -0x32
    unsigned short dollSlot;                  // local_46 -0x42
    int slotIdx = 0;                          // local_24 -0x20
    int errDoll;                              // local_1c -0x1c
    bool fail;                                // bVar8
    unsigned char state;

    // +0x00 state/charac 校验：!(state==3 && getCurCharacR()!=0) → fail
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
        fail = false;
    else
        fail = true;
    if (fail)
        return LineFunc(0x9984,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x66：ORIG 0x08202d?? 对 subject（-0x6b）0x29 字节为 4.1 内联对齐剥离慢路；
    // 旧码 memset call。slotIdx/itemCount/seller 的初始化在 ORIG 中位于清零之后。
    char subject[0x29] = {0};                 // -0x6b：开店名称（get_str 缓冲）
    // [G1-Dispatchers] 删显式 `item.Reset()`：本仓 PrivateStoreItem ctor 体即 `Reset()`
    // （CPrivateStoreMgr/PrivateStoreItem_ctor.inc）⇒ 该调用冗余；ORIG 该点只有 ctor 内联体
    // （0x0823457c/0x0823459a），无独立 Reset call（p167 EXTRA PrivateStoreItem::Reset ×1）。

    // +0xe5：读名称长度
    if (!packet.get_int((int&)slotIdx))
        return LineFunc(0x998e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (slotIdx > 0x28)
        return LineFunc(0x9990,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x15c：读名称串
    if (!packet.get_str(subject, 0x29, slotIdx))
        return LineFunc(0x9993,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x1af：string(subject) → HasSlang / hasSpecialCharacter 检查
    nameStr = subject;
    if (CSyncSlangFilter::GetInstance()->HasSlang(nameStr)
        || G_CDataManager()->hasSpecialCharacter(subject))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, 0x9f);
        return 0;
    }

    // +0x260：取卖家私商
    private_store::GetInstancePrivateStoreMgr()->GetStoreSeller(&seller, user);
    // +0x27b：读 itemCount
    if (!packet.get_byte(itemCount))
        return LineFunc(0x99a4,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x2bf：GetState 调试输出（cMyTrace + printf）
    state = ((private_store::CPrivateStore*)seller)->GetState();
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x99a6, 0);
        trace("(private_store::CPrivateStore*)seller)->GetState()=%d", (unsigned int)state & 0xff);
    }
    printf("(private_store::CPrivateStore*)seller)->GetState()=%d",
           (unsigned int)((private_store::CPrivateStore*)seller)->GetState() & 0xff);

    // +0x32b：state != 1 → return 0
    if (((private_store::CPrivateStore*)seller)->GetState() != 1)
        return 0;

    // +0x343：itemCount == 0 → LineFunc 0x99ad
    if (itemCount == 0)
        return LineFunc(0x99ad,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x376：逐条目读取（i: local_24，setg 无符号比较 itemCnt > i）
    for (slotIdx = 0; slotIdx < (int)(unsigned int)itemCount; slotIdx = slotIdx + 1)
    {
        // +0x382：5 连读，全部成功才通过
        if (!(packet.get_short((short&)item.m_field44)
              && packet.get_int(item.m_field40)
              && packet.get_byte((char&)item.m_field46)
              && packet.get_short((short&)item.m_field48)
              && packet.get_int(item.m_item.m_addInfo2)))
            return LineFunc(0x99bd,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x458：attr != 1 且 count < 1 → 拒绝
        if (item.m_field46 != 1 && item.m_item.m_addInfo2 < 1)
            return LineFunc(0x99c0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x495：CheckItemLock(user, GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)attr), slot)
        if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)item.m_field46),
                                // [DNF-FIX 2026-10-08 c31] ORIG 0x0820316f 读 item+0x48（第 4 个 get_short 的槽位），非 +0x44
                                (int)(short)item.m_field48))
        {
            ((private_store::CPrivateStore*)seller)->ResetItems();
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, 0xd5);
            return 0;
        }
        // +0x4f2：ISeller 虚槽 +0x10 AddItem(item, prevCountSlot)
        if (!seller->AddItem(item, slotIdx))
        {
            ((private_store::CPrivateStore*)seller)->ResetItems();
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, 0x11);
            return 0;
        }
    }

    // +0x565：读 dollSlot（ORIG 0x0820324c 调 get_short(unsigned short&)，字段本型直传）
    if (!packet.get_short(dollSlot))
        return LineFunc(0x99e9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x5a9：doll 检查
    if (((private_store::CPrivateStore*)seller)->isDollCreated())
    {
        errDoll = ((private_store::CPrivateStore*)seller)->CheckPrivateStoreDoll(dollSlot);
        if (errDoll != 0)
        {
            ((private_store::CPrivateStore*)seller)->ResetItems();
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, (unsigned char)(errDoll & 0xff));
            return 0;
        }
    }
    // +0x606：ISeller 虚槽 +0x08 CompleteDisplay(subject, dollSlot)；失败 → ResetItems
    if (!seller->CompleteDisplay(subject, dollSlot))
        ((private_store::CPrivateStore*)seller)->ResetItems();
    return 0;
}