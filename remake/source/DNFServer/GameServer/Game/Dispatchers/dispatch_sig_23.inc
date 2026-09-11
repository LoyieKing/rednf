int Dispatcher_TryEnchant::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820ae84：按 msg+0xd 类别走 Enchanter 附魔请求链。
    MSG_STATIC_COMMAND* msg = reinterpret_cast<MSG_STATIC_COMMAND*>(packet.get_packet(0));
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa70b, __PRETTY_FUNCTION__, 0, 0);
    }
    int ret = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1f);
    if (ret != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac, (unsigned char)(ret & 0xff));
        return 0;
    }
    switch (msg->m_type0d)
    {
    case 0:
    {
        expert_job::CExpertJob* job = GlobalData::s_ExpertJobMgr->AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)1);
        if (job == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac, 0x16);
            return 0;
        }
        job->OnStartMission(user, user);
        break;
    }
    case 1:
        if (user->GetCurCharacExpertJob() == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac, 0x16);
            return 0;
        }
        break;
    case 2:
        if (user->GetCurCharacExpertJob() == 0)
        {
            return 0;
        }
        break;
    default:
        return LineFunc(0xa732, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->GetCurCharacExpertJob()->GetType() != 1)      // P48 无 CSE：GetType 前新 call
    {
        return LineFunc(0xa737, __PRETTY_FUNCTION__, 0, 0);
    }
    static_cast<expert_job::CEnchanter*>(user->GetCurCharacExpertJob())->OnTryEnchant(user, msg);
    return 0;
}