bool CUser::sendSecuRewardItem(Inven_Item* item, ENUM_SECURITY_REWARD_TYPE type,
                               int* outSlotVec)
{
    // b28d-21：p167 MISSING strncpy×2/strlen（ORIG 0x086884a6/0x086884c4 两次
    // strncpy 长度立即数 0x14/0xff）⇒ 还原栈缓冲形：body@-0x15f(0xff)、
    // local@-0x5f、title@-0x22(0x14)，ReqDBSendNewSystemMail 传 title/body。
    // b28d-W8: 声明序/语句序照 ORIG —— 0x08688347 先拷贝 Inven_Item 局部、
    // 0x086883aa 才判空（P171 原码遗留照抄）；failed/slot 的初始化在判空之后
    //（0x086883ba/be）。返回类型 void→bool（P159 尾 `mov $0/%1,%eax`）。
    char body[0x100];             // ORIG -0x15f..-0x60（256B，Ghidra local_163[256]）
    Inven_Item local(*item);      // ORIG 将 Inven_Item 原样拷贝入栈局部 -0x5f
    // b28d-W8b: ORIG -0x22..-0xe 共 0x15 字节（strncpy 长度 0x14 + 终止符 1）
    // ⇒ 数组 0x15 使 body 起始精确落 -0x15f（原 0x14 形整帧浅 1B）。
    char title[0x15];             // ORIG -0x22

    if (item == 0)
    {
        return false;
    }

    // b28d-W8: char 而非 bool —— ORIG 0x08688486 `cmpb $0x0,-0xd(%ebp); je`
    // 内存直比（bool 局部 4.4 恒 movzbl;xor $1;test 三段，P158/N122）。
    char failed = 0;
    int slot = -1;

    // b28d-W8: ORIG 0x086883d0 `call; test %al,%al; je <成功路径>`（无 xor）⇒ 正向
    // `if (CheckInTrade())` + else 成功路径（P292 表：bool callee 正向直判 2 条）。
    if (CheckInTrade())
    {
        failed = 1;
    }
    else
    {
        slot = getCurCharacInvenW()->insertItemIntoInventory(
            local, static_cast<eItemAddReason>(0xd), true, true);
        if (slot == -1)
        {
            failed = 1;
        }
    }

    // b28d-W8: P47/P214① —— ORIG 0x08688486 `cmpb $0; je <成功岛>` ⇒ 失败路径
    // fall-through、成功路径在 je 目标（块序与 `if(!failed) 成功` 相反）。
    if (failed != 0)
    {
        // ORIG rodata：DAT_08cf10ec（"아이템 지급" 类标题）/ DAT_08cf10fc（正文），
        // GBK/EUC 多字节原样字节保真转写（objdump -s .rodata 实证）。
        static const char kSenderTitle[] =
            "\260\355\272\355\270\260\300\307\040\274\261\271\260";
        static const char kFailMessage[] =
            "\260\355\270\260\300\307\040\274\261\271\260\300\304\314\040\277\354\306"
            "\355\300\270\267\316\040\265\265\302\360\307\317\277\264\275\300\264\317"
            "\264\231.";

        strncpy(title, kSenderTitle, 0x14);   // ORIG 0x08688490（长度 0x14）
        strncpy(body, kFailMessage, 0xff);    // ORIG 0x086884ab（长度 0xff）
        // b28d-W8: P52/P134 —— ORIG 0x086884cf/4df/4ec 三值入 %ebx/%esi/%eax 跨
        // 调用保活、装载段直读寄存器；具名局部引入 3 对 store/reload（P0 直传）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, local, 0, getCurCharacNo(), body,
            static_cast<int>(strlen(body)), 0,
            static_cast<ENUM_SERVER_GROUP>(GetServerGroup()), false, false);
        outSlotVec[static_cast<int>(type)] = -1;
        return false;
    }

    outSlotVec[static_cast<int>(type)] = slot;
    return true;
