bool ItemVendingMachine::BuyAuctionItem(CUser* user, AuctionNeedInfo& info,
                                        int /*a3*/, bool bPremiumFee, int refValue)
{
    // ---- 手续费基数（ORIG 0x8175c96..0x8175dc4）----
    // 具名局部集合照 ORIG 槽位（P0：有槽往返 ⇒ 原码有具名局部）：
    //   -0x44 nRef / -0x3d bPremium / -0x3c nFeeNormal(5) / -0x38 nFeePremium(2)
    //   / -0x34 feeBase / -0x30 price / -0x20 rate（double）。
    int  nRef        = refValue;
    bool bPremium    = bPremiumFee;
    int  nFeeNormal  = 5;
    int  nFeePremium = 2;
    int  feeBase     = 0;
    int  price       = info.m_price;
    if (nRef == 0)
    {
        // ORIG 0x08175ca4..0x08175cbb：两臂各自 `mov 常量槽,%eax; mov %eax,feeBase槽`
        // （三目在 4.4 会合并成共享 store ⇒ 少 1 条，故写 if/else）。
        if (bPremium)
            feeBase = nFeePremium;
        else
            feeBase = nFeeNormal;
    }
    else
    {
        // .rodata 实读（objdump -s -j .rodata 0x8b871a0 段）：
        // 0x08b871a8=0.5 / b0=1.5 / b8=1.6 / c0=1.7 / c8=1.8 ⇒ 分档边界是 **1.7**，
        // 判定链是嵌套短路对（0x08175cd0..0x08175d80 共 8 次 fucompp），每层写成
        // `(rate < Lo) || (Hi <= rate)`。ORIG 把商缓存进具名 double rate(-0x20) 后
        // 每次比较只 `fldl -0x20` 复用（0x08175cc8 fstpl -0x20 / 0x08175ccb fldl -0x20）。
        double rate = static_cast<double>(price) / static_cast<double>(nRef);
        if (rate < kRateDeny)
            return false;
        if (kRate150 <= rate)
        {
            if (rate < kRate150 || kRate160 <= rate)
            {
                if (rate < kRate160 || kRate170 <= rate)
                {
                    // ORIG 0x08175d8d..0x08175dc4 第 4 层（0x08175da4 比较 1.8）：
                    // rate∈[1.7,1.8) → 0x32，rate>=1.8 → 0x50。旧重建漏此层，
                    // 使 [1.7,1.8) 误发 0x50（真语义 bug，8 次 fucompp 只写 6 次）。
                    if (rate < kRate170 || kRate180 <= rate)
                    {
                        feeBase = 0x50;
                    }
                    else
                    {
                        feeBase = 0x32;
                    }
                }
                else
                {
                    feeBase = 0x32;
                }
            }
            else
            {
                feeBase = 0x32;
            }
        }
        else if (bPremium)
        {
            feeBase = nFeePremium;
        }
        else
        {
            feeBase = nFeeNormal;
        }
    }

    // ---- fee = trunc(price/100.0 * feeBase)（ORIG fldcw RC=0b11 后 fistpl）----
    // ORIG 0x08175dcb..0x08175dfa：先 `fildl price; fldl 100.0; fdivrp` 再 `fildl feeBase;
    // fmulp`（price/100.0 是左操作数），`fistpl -0x34` **写回 feeBase 槽**（无独立 fee
    // 局部），随后 `mov -0x34,%edx; mov %edx,0xc(%eax)` 直写 info.m_fee。
    feeBase = static_cast<int>(static_cast<double>(price) / kFeeDivisor *
                              static_cast<double>(feeBase));
    info.m_fee = feeBase;

    // ---- 审计流：put_binary(&info, 0x28)（ORIG 0x8175e09..0x8175e5a）----
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp", 0x7b),
        true);
    // 0x08175e51..0x08175e5a：`lea -0x50; call CStreamGuard::operator->`
    // 之后再 `call put_binary` ⇒ 原码写的是 guard->put_binary（P-B 代理链形）。
    guard->put_binary(&info, 0x28);

    // ---- 计费入参字符串（ORIG buf8/buf36/buf21 三缓冲 + atoi 回读）----
    // priceBuf ← "%ld" 99999（购买单价槽恒为 99999）；payBuf ← "%d"(price-fee 或 fee)；
    // uidBuf ← "%ld" itemUid（ORIG 压两个 dword：低 32 位 0xc(%esp)+高 32 位 0x10(%esp)）。
    // 槽位/声明序照 ORIG：payBuf(-0x8d,0x24) → uidBuf(-0x69,0x15) → priceBuf(-0x54,8)；
    // uidBuf 带 `= {0}`：ORIG 0x08175e5f..0x08175ebd 对其发 21 字节对齐感知清零展开
    // （P214③/P312），且清零发生在 put_binary 之后 ⇒ 声明点即此处。
    char payBuf[0x24];
    char uidBuf[0x15] = {0};
    char priceBuf[8];
    // ORIG 0x08175ec0 `movl $0x1869f,-0x2c(%ebp)` + 0x08175ec7 回读 ⇒ 具名 int 局部，
    // 两段 snprintf 共用（第二段 0x081760f1 直接读 -0x2c，无重赋值，P244）。
    int  nPrice = 99999;

    OS_API::snprintf(priceBuf, sizeof(priceBuf), "%ld", nPrice);
    OS_API::snprintf(payBuf, sizeof(payBuf), "%d", info.m_price - feeBase);
    OS_API::snprintf(uidBuf, sizeof(uidBuf), "%ld", info.m_itemUid);

    // 名字不缓存：ORIG 每笔计费前各自 call get_acc_name / getCurCharacName
    // （0x08175f6a、0x08175f7f 与第二笔同址各一次 ⇒ 共 4 次 call），
    // 缓存成局部会少 2 次 call（本函数 ORIG-only call 缺口的来源之一）。

    // 经 s_pIPGHelper 虚表 +0x3c 取 CIPGMgr*（ORIG 0x8175f46..0x8175f5a），再取计费句柄
    //（getBillingCash → *(mgr+0xec)，ORIG 0x0817a2ee）。虚表槽未建模，沿用工程内
    // CCeraShop_Special.cpp 的裸函数指针调用模式。【推断：等价 ABI】
    // ORIG 无函数指针具名槽（0x8175f46..0x8175f5a 直接 `mov (%eax),%edx; call *%edx`）
    // ⇒ 按 P0 把取槽表达式内联进调用点，勿缓存成局部。
    typedef void* (*GetIPGMgrFn)(void*);
    // ORIG 0x08175f3f `movl $0x0,-0x28(%ebp)` 先清零具名 mgr，0x08175f5c 再写回
    // （Ghidra `local_2c = 0; local_2c = (…)()` 两段）⇒ 保留两段赋值形。
    WongWork::IPG::CIPGMgr* mgr = 0;
    mgr = reinterpret_cast<WongWork::IPG::CIPGMgr*>(
            (*reinterpret_cast<GetIPGMgrFn*>(
                *reinterpret_cast<char**>(GlobalData::s_pIPGHelper) + 0x3c))(
                    GlobalData::s_pIPGHelper));
    // getBillingCash 不缓存：ORIG 两段计费各 call 一次
    // （0x08175f94 与第二笔 0x081760xx 同址），缓存会少 1 次 call。

    // ---- 第一段：主扣费（kind=0x3c，金额 price-fee）----
    int rc = mgr->getBillingCash()->purchase(
                        info.m_itemUid,
                        atoi(priceBuf), atoi(payBuf),
                        user->get_acc_name(), user->getCurCharacName(),
                        info.m_trans1, 0x3c);
    if (!CheckBillingErrorCode(rc))
    {
        user->m_historyLog.BillingFail(static_cast<ENUM_BILLING_TYPE>(1),
                                       info.m_trans1.m_tranNo,
                                       info.m_price, rc,
                                       const_cast<char*>("purchase"));
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp",
            __PRETTY_FUNCTION__,
            0xa5, "IPG_OUTPUT user: %u, charac: %s reason: %d",
            user->getCurCharacNo(), user->getCurCharacName(), rc);
        return false;
    }
    user->m_historyLog.BillingSuccess(static_cast<ENUM_BILLING_TYPE>(1),
                                      info.m_trans1.m_tranNo, info.m_price);

    // ---- 第二段：手续费（kind=0x3e，金额 fee）----
    OS_API::snprintf(priceBuf, sizeof(priceBuf), "%ld", nPrice);   // 0x081760f1 读 -0x2c
    OS_API::snprintf(payBuf, sizeof(payBuf), "%d", feeBase);
    OS_API::snprintf(uidBuf, sizeof(uidBuf), "%ld", info.m_itemUid);

    rc = mgr->getBillingCash()->purchase(
                        info.m_itemUid,
                        atoi(priceBuf), atoi(payBuf),
                        user->get_acc_name(), user->getCurCharacName(),
                        info.m_trans2, 0x3e);
    if (!CheckBillingErrorCode(rc))
    {
        user->m_historyLog.BillingFail(static_cast<ENUM_BILLING_TYPE>(1),
                                       info.m_trans2.m_tranNo,
                                       info.m_fee, rc,
                                       const_cast<char*>("purchase"));
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp",
            __PRETTY_FUNCTION__,
            0xe8, "IPG_OUTPUT user: %u, charac: %s reason: %d",
            user->getCurCharacNo(), user->getCurCharacName(), rc);
        return false;
    }
    user->m_historyLog.BillingSuccess(static_cast<ENUM_BILLING_TYPE>(1),
                                      info.m_trans2.m_tranNo, info.m_fee);

    return true;
