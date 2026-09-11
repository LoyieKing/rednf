// ===========================================================================
// 安全奖励物品发放（sendSecuRewardItem）
// ORIG 0x0868833c：交易中或背包满 → 系统邮件退还；否则入背包并回填落位。
// ===========================================================================
// b28d-W8(2026-09-09): **NEAR 149/149**（DIFF→NEAR）— 本波 5 修：
// ①返回类型 void→bool（CUser.h:2976 同步，P159 尾 `mov $0/%1,%eax`）；
// ②语句序照 ORIG：先拷贝 Inven_Item 局部（0x8688347）再判空（0x86883aa，P171 照抄）；
// ③body 0x100（非 0xff）⇒ 帧/槽位与 ORIG 只差 1B；failed 改 char（`cmpb $0` 直比）；
// ④CheckInTrade 正向 `if(...)` + else 成功路径（`test %al,%al; je` 无 xor）；
// ⑤P47 块序：`if(failed){失败; return false}` + 成功 fall-through；ReqDBSend 内联实参。
// b28d-W8b: **NEAR→AE**（149/149）— title 数组 0x14→0x15（ORIG -0x22..-0xe 共
// 0x15 字节 = strncpy 长度 0x14 + 终止符）⇒ body 起始精确落 -0x15f，1B 槽平移消失。
// p167 CLEAR(9=9)，gap 0。ORIG 0x0868833c。
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
        // [S3-W8 2026-09-11 §6.5 真 bug] ORIG 0x8cf10fc 逐字节 = 40B
        //   b0 ed ba ed b8 b0 c0 c7 20 bc b1 b9 b0 c0 cc 20 bf ec c6 ed c0 b8
        //   b7 ce 20 b5 b5 c2 f8 c7 cf bf b4 bd c0 b4 cf b4 d9 2e
        //   （EUC-KR "고블린의 선물이 우편으로 도착하였습니다."）。
        //   旧码为八进制转义且错 4 处：缺 `\272\355`（"블"）、`\304` 多 1 字节、
        //   `\360`→`\370`（f0→f8）、`\231`→`\331`（99→d9）⇒ 二进制串与 ORIG 不符。
        //   改用 \xNN 十六进制逐字节保真，避免八进制歧义。

        // [W16C 符号名轴·2026-09-11] ORIG 无 `_ZZN5CUser18sendSecuRewardItem…E12kFailMessage`
        //   / `…E12kSenderTitle` 实体：ORIG 0x08688498/0x086884b3 直接
        //   `mov DWORD PTR [esp+0x4],0x8cf10ec` / `0x8cf10fc`（.rodata 匿名串、无符号）
        //   ⇒ 去两处 `static const char[]` 改内联字面量；OUR-only −2、四数不变（overlay 实证）。
        strncpy(title, "\260\355\272\355\270\260\300\307\040\274\261\271\260", 0x14);   // ORIG 0x08688490（长度 0x14）
        strncpy(body,
                "\xb0\xed\xba\xed\xb8\xb0\xc0\xc7\x20\xbc\xb1\xb9\xb0\xc0\xcc\x20"
                "\xbf\xec\xc6\xed\xc0\xb8\xb7\xce\x20\xb5\xb5\xc2\xf8\xc7\xcf\xbf"
                "\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", 0xff);    // ORIG 0x086884ab（长度 0xff）
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
}