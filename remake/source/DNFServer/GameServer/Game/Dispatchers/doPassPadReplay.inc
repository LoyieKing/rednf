// ORIG T 0x082633c6：密保卡 pad 答案复核链（shake 减扰 + checkAnswer + 计数/DB 上报）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082633c6 orig/ours=279/286 ndiff-blocks=22
//   帧差 sub $50 vs $60 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x50,%esp | mov    %al,-0x2c(%ebp) | movl   $0x0,-0xc(%ebp)
void Dispatcher_MouseRegister::doPassPadReplay(CUser* user, char* answer,
                                               char answerType, bool& ok,
                                               unsigned char& errCode)
{
    ok = false;   // ORIG *param_4=false
    errCode = 0;  // ORIG *param_5='\0'

    for (int i = 0; i <= 3; ++i)   // setle 循环（P51/P2）
    {
        answer[i] -= user->getPad()->GetShakeData(i);
    }

    // [L7DispG2] ORIG 0x08263447 `xor $1,%eax; test %al,%al; je +0x293`（bool 取反 +
    //   跳转成功路径）⇒ 原码形 = `if (!checkAnswer())`：失败路径 fallthrough 在前
    //   （0x08c..0x250，含公共尾 DB_PassPadUpdateFailCnt），成功路径在 +0x293。
    //   旧形 `(int)checkAnswer(...)==1` 产 `test %al,%al; je` 且两块顺序相反。
    if (!user->getPad()->checkAnswer(answer))
    {
        // 失败分支（fallthrough）：SecuService(0,0) + incFailCnt 后按新计数分流。
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)0);
        // [L7DispG2] P0：ORIG 0x08263485 段无 pad/newFailCnt 具名槽（getPad 结果直传）。
        if ((int)user->getPad()->incFailCnt() > 2)
        {
            errCode = 0x88;
            static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
                (XNuclear::ENUM_HADES_SECURITY)1, 0x21u);
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x342, 1u, 0u, 0u);
        }
        else
        {
            PacketGuard packet;
            packet.put_header(1, 0xa7);
            packet.put_byte(0);
            packet.put_byte(0x87);
            packet.put_byte(user->getPad()->getFailCnt());   // P48：第二次独立 getPad()
            packet.finalize(true);
            user->Send(packet);

        // [L7DispG2] P0：ORIG 无 padC 槽（P48「第二次独立 getPad」保持）。
            if (user->getPad()->getFailCnt() == 1)
                static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
                    (XNuclear::ENUM_HADES_SECURITY)1, 0x1fu);
            else
                static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
                    (XNuclear::ENUM_HADES_SECURITY)1, 0x20u);
        }
        // 公共尾 +0x251：按当前计数上报失败次数。
        DB_PassPadUpdateFailCnt::makeRequest(user->get_acc_id(),
                                             (unsigned int)user->getPad()->getFailCnt(),
                                             (char*)user->getWebAddress());
        return;
    }
    user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                   (ENUM_SECU_SERVICE_ACTION)1);
    static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
        (XNuclear::ENUM_HADES_SECURITY)1, 0x23u);
    ok = true;   // asm +0x2d9 movb $1

    PacketGuard packet;
    packet.put_header(1, 0xa7);
    packet.put_byte(2);
    // [L7DispG2] P0：ORIG 无 padA 槽（getPad 结果直传）。
    // [Q4DispatchBig 2026-09-11] ORIG 该点 `movzbl %al,%eax`（截断形）⇒ 用
    //   (unsigned char) cast 取代 `& 0xff`（4.4 对掩码产 `and $0xff,%eax`）。
    packet.put_byte(static_cast<int>(
        static_cast<unsigned char>(user->getPad()->getAdvideChangePwd())));
    packet.finalize(true);
    user->Send(packet);

    // [L7DispG2] P0：ORIG 无 padB 槽（getPad 结果直传）。
    if (user->getPad()->getFailCnt() > 0)
    {
        DB_PassPadUpdateFailCnt::makeRequest(user->get_acc_id(), 0,
                                             (char*)user->getWebAddress());
    }
    user->getPad()->setCertified(true);   // ORIG T 0x0826530e（push $1）
    user->getPad()->resetFailCnt();       // ORIG W 0x082652f2
    DB_GoblinPadUpdateReward::makeRequest(user->get_acc_id(), answerType,
                                          false, (char*)user->getWebAddress());
    return;
}