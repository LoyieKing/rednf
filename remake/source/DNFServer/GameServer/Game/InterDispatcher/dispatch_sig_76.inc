int Inter_MailBox_Req_CheckReceiver::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // cmp $0x2,setle
        return 0;

    SIG_MAILBOX_CHECK_RECEIVER* sig =                  // -0x30
        reinterpret_cast<SIG_MAILBOX_CHECK_RECEIVER*>(data);

    // ORIG 0x084cf95a：cmd/type 判定读 sig+0x31（m_payload 首 int，邮件类型
    // 1=cera 直购/2=多附件），不是 m_resultFlag(+0x3fb9，DB 结果 0/0x4d)。
    const int mailType = *reinterpret_cast<const int*>(sig->m_payload);
    int cmd = 0x61;                                    // -0x2c
    if (mailType == 1)
        cmd = 0x43;
    else if (mailType == 2)
        cmd = 0x13c;

    if (sig->m_receiverCharacNo == 0)      // 收件角色号
    {
        SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 3);
        return 0;
    }
    if (sig->m_receiverDeleteFlag == 0)      // 附件标志：无附件走 GM 线
    {
        if (!GlobalData::s_pGMAccounts_->isGM(
                sig->m_receiverAccId))
        {
            if (sig->m_resultFlag == 0x4d)   // §4.3 字段化：SIG_MAILBOX_CHECK_RECEIVER::m_resultFlag @+0x3fb9
            {
                SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 0x4d);
                return 0;
            }

            unsigned int err = 0;                      // -0x28
            const int type = *reinterpret_cast<const int*>(sig->m_payload);
            if (type == 1)
            {   // cera 直购：sig+0x35 载荷按 MSG_BUY_CERASHOP_ITEM 视图
                *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x67) =
                    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x1d);
                *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x63) =
                    sig->m_receiverAccId;
                if (!GlobalData::s_pItemVendingMachine->BuyItem(
                        user, *reinterpret_cast<const MSG_BUY_CERASHOP_ITEM*>(
                                  reinterpret_cast<const char*>(sig) + 0x35)))
                    return 0x2d12;
                return 0;
            }
            if (type == 0)
            {
                const unsigned int itemIdx =           // -0x24（单附件）
                    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x6e);
                if (itemIdx != 0)
                {
                    CItem* ci = G_CDataManager()->find_item((int)itemIdx);  // -0x20
                    if (ci != 0)
                    {
                        if (GlobalData::s_rentalSystem->CheckRentableItem(1, itemIdx))
                            return 0x11;
                        if (GlobalData::s_rentalSystem->CheckRentableItem(2, itemIdx))
                            return 0x11;
                        if (ci->GetAttachType() == 5 &&
                            (int)user->get_acc_id() !=
                                *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x29))
                        {
                            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x61, 0xd8);
                            return 0;
                        }
                    }
                }
                if (*reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x63) != 0)
                {
                    int limit = 0x7fffffff;            // -0x1c
                    limit = G_CDataManager()->GetMoneyLimitPerLevel(
                        *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x21),
                        reinterpret_cast<const char*>(sig) + 0x46);
                    if ((unsigned int)limit <
                        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x63))
                    {
                        SendErrorPacket(user, (ENUM_CMDPACKET)0x61, 0xe);
                        return 0;
                    }
                }
                err = (unsigned int)WongWork::CMailBoxHelper::ReqDBSendNewMail(
                    user, reinterpret_cast<MSG_MAILBOX_SEND*>(
                              reinterpret_cast<char*>(sig) + 0x35),
                    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x1d), false,
                    sig->m_receiverAccId, 0);
                if (err == 0)
                {
                    PacketGuard packet;                // -0x3c
                    packet.put_header(1, cmd);
                    packet.put_byte(1);
                    packet.finalize(true);
                    user->Send(packet);
                }
                else
                {
                    SendErrorPacket(user, (ENUM_CMDPACKET)cmd,
                                    (unsigned short)(err & 0xffff));
                }
            }
            else if (type == 2)
            {   // 多附件邮件
                for (int i = 0;                                        // -0x18
                     i < (int)(unsigned int)*reinterpret_cast<unsigned char*>(
                             reinterpret_cast<char*>(sig) + 0x68);
                     ++i)
                {
                    const unsigned int itemIdx =       // -0x14
                        *reinterpret_cast<unsigned int*>(
                            reinterpret_cast<char*>(sig) + 0x6c + i * 0xb);
                    if (itemIdx != 0)
                    {
                        CItem* ci = G_CDataManager()->find_item((int)itemIdx);  // -0x10
                        if (ci != 0)
                        {
                            if (GlobalData::s_rentalSystem->CheckRentableItem(1, itemIdx))
                                return 0x11;
                            if (GlobalData::s_rentalSystem->CheckRentableItem(2, itemIdx))
                                return 0x11;
                            if (ci->GetAttachType() == 5 &&
                                (int)user->get_acc_id() !=
                                    *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x29))
                            {
                                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0xd8);
                                return 0;
                            }
                        }
                    }
                }
                if (*reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x64) != 0)
                {
                    int limit = 0x7fffffff;            // -0xc
                    limit = G_CDataManager()->GetMoneyLimitPerLevel(
                        *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x21),
                        reinterpret_cast<const char*>(sig) + 0x46);
                    if ((unsigned int)limit <
                        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x64))
                    {
                        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0xe);
                        return 0;
                    }
                }
                // b28c-l23 SIGFIX 消费点：bool callee ⇒ 裸形（P168a①）
                if (!SendDBThread_forchina(user, sig))
                    return 0;
            }
        }
        else
        {
            SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 0x5a);
        }
    }
    else
    {
        SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 0x18);
    }
    return 0;
