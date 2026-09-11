// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082064d6 orig/ours=274/288 ndiff-blocks=35
//   帧差 sub $48 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x48,%esp | movl   $&_ZZN23Dispatcher_HumanCertify12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | cmp    $0x251b,%eax
int Dispatcher_HumanCertify::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082064d6: state > 2 才放行
    if (user->get_state() <= 2)
    {
        return LineFunc(0xa007, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x04c: M-CAP 点数过高时概率踢线
    // ORIG 段直证：`cmp $0x251b,%eax; seta %al; test %al,%al; je`（值不落槽、4.1 物化）
    // ⇒ 删 capPoint 具名量并改 (bool) 强制物化形（技巧9）。
    if ((bool)(user->m_mcap.getPoint() > 0x251b))
    {
        int randVal = get_rand_int(100);
        if (randVal <= 10)
        {
            user->m_historyLog.RequestCleanPad(user->m_mcap.getPoint(), '\x02');

            user->DisConnSig(static_cast<DISCONN_SIG>(0x35), true, 0);
        }
    }

    // +0x0c5: 读认证号（1..10）
    int certifyNo = 0;
    int certStep = 10;
    // ORIG 段直证：`movl $0x0,-0x22; movl $0x0,-0x1e; movw $0x0,-0x1a`（10B 聚合零
    // 初始化内联；原逐元素 5 条 movw 形令清零序列形态不符）。
    unsigned short answer[5] = { 0 };
    if (!(packet.get_int(certifyNo)) || certifyNo > 10)
    {
        return LineFunc(0xa023, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x156: 读 10 字节答案
    if (!packet.get_binary(reinterpret_cast<char*>(answer), 10))
    {
        return LineFunc(0xa024, __PRETTY_FUNCTION__, 0, 0);
    }

    char certValid = 1;
    unsigned short verifyData = 0;
    if (!packet.get_short(verifyData))
    {
        return LineFunc(0xa02f, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x21d: 读认证类型
    unsigned char certifyType = 0;
    if (!packet.get_byte(certifyType))
    {
        return LineFunc(0xa032, __PRETTY_FUNCTION__, 0, 0);
    }

    if (certifyType == 2)
    {
        // +0x233: 类型 2 = 已通过 → 落实状态
        user->setHumanCertified(true);
        user->SendMail(false);
        user->resetCleanpadFailCnt();
        user->resetHumanCertifyData();
    }
    else
    {
        // +0x27a: 校验答案
        if (!user->checkHumanCertify(certifyNo, answer))
        {
            // +0x295: 失败 → 记日志、标记无效
            user->m_historyLog.RequestCleanPad(user->m_mcap.getPoint(), '\x02');
            certValid = 0;
            // +0x2c1: 未达失败上限
            // [DispG1/34] P191/P59：ORIG 0x0820679c `test %al,%al; je 0x08206834`
            //   ⇒ 正向 if（上限分支 fallthrough，未达分支在后）
            if (user->isCleanPadVerifyLimit())
            {
                if (certifyType == 1)
                {
                    user->SendMail(false);
                    user->resetHumanCertifyData();
                    user->resetCleanpadFailCnt();
                }
                else
                {
                    user->DisConnSig(static_cast<DISCONN_SIG>(0x1f), true, 0);
                }
            }
            else
            {
                if (certifyType == 1)
                {
                    // 邮件认证重发（ORIG 0x08206806 `call; xor $0x1; test; je 尾`）
                    if (user->reqSendMailCertify() != 1)
                    {
                        user->SendMail(true);
                    }
                }
                else
                {
                    // +0x36c: 地图清除认证
                    user->setHumanCertified(true);
                    user->reqHumanCertify4ClearMap(false);
                }
            }
        }
        else
        {
            // +0x28f: 通过 → 记日志、短时重放检测
            if (certifyType == 1)
            {
                user->SendMail(true);
            }
            user->resetCleanpadFailCnt();
            // ORIG 0820654b 段直证：`call m_mcap.getPoint; mov %eax,0x4(%esp); call
            // RequestCleanPad`（实参内联，无具名 capPoint 槽）；tick 差同为内联表达式
            //（P329 删具名中间量）。
            user->m_historyLog.RequestCleanPad(user->m_mcap.getPoint(), '\x01');
            if (static_cast<unsigned int>(GlobalData::s_systemTime_.getCurTickCount() -
                                      user->m_mcap.m_capchaReqTick) < 2000)
            {
            // ORIG：`call getHackAnalyzer; ...; call addServerHackCnt`（receiver 链式，
            // 无具名 hackAnalyzer 槽；P52/P329）。
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0x341), 1, 0, 0);
            }
        }
    }

    // +0x44a: 验证数据异常 → hack 计数
    if (user->VerifyCleanPadVeriData(static_cast<unsigned int>(verifyData)))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x337), 1, 0, 0);
    }

    // +0x48d: 认证有效则清数据
    if (certValid != 0)
    {
        user->resetHumanCertifyData();
    }
    return 0;
}