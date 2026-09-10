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

    // xor $1/test/je：checkAnswer()==1 → 成功分支（asm +0x293）。
    if ((int)user->getPad()->checkAnswer(answer) == 1)
    {
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)1);
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x23u);
        ok = true;   // asm +0x2d9 movb $1

        PacketGuard packet;
        packet.put_header(1, 0xa7);
        packet.put_byte(2);
        Sanicova::CPad* padA = user->getPad();
        packet.put_byte(padA->getAdvideChangePwd() & 0xff);
        packet.finalize(true);
        user->Send(packet);

        Sanicova::CPad* padB = user->getPad();
        if (padB->getFailCnt() > 0)
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

    // 失败分支（fallthrough）：SecuService(0,0) + incFailCnt 后按新计数分流。
    user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                   (ENUM_SECU_SERVICE_ACTION)0);
    Sanicova::CPad* pad = user->getPad();
    int newFailCnt = pad->incFailCnt();   // ORIG W 0x082652d8
    if (newFailCnt > 2)
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

        Sanicova::CPad* padC = user->getPad();   // P48：第二次独立 getPad()
        if (padC->getFailCnt() == 1)
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
