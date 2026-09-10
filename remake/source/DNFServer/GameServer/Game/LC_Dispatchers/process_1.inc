int Dispatcher_RegisterToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRegisterToBlackListParam& out = static_cast<stRegisterToBlackListParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
        return 0;
    if (out.m_4 >= 0)
    {
        Packet_Register_To_BlackList packet;
        packet.m_accId = user->get_acc_id();

        // strncpy(msg+0x14, len=msg+0x10)（MSG_REGISTER_TO_BLACKLIST 名字字段）
        MSG_REGISTER_TO_BLACKLIST& req =
            reinterpret_cast<MSG_REGISTER_TO_BLACKLIST&>(msg);

        // MSG_REGISTER_TO_BLACKLIST 布局未还原（PacketTypes.h 占位 m_pad）：
        // ORIG 读 +0x10（u32 len）、+0x14（名字首址）——保持偏移访问并标注。
        strncpy(packet.m_name,
                reinterpret_cast<char*>(&req) + 0x14,
                *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&req) + 0x10));
        // ORIG 韩服判定：G_CEnvironment+0x378 == 3（serverGroup==3）才做 GM 名单比较。
        // ORIG 直接读 m_serverEnvir.m_serverGroup（+0x378 内联读取，无 get_server_group 调用）。
        if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 3)
        {
            // DAT_08bc17d6 / DAT_08bc17dd：EUC-KR GM 名串区（0x1d 字节精确比较）
            static const char kGmName1[0x1d] = {
                '\xbb', '\xfe', '\xbf', '\xc0', '\xc0', '\xaf', '\x00',
                '\xbb', '\xfe', '\xc0', '\xaf', '\xb8', '\xae', '\x00',
                'S', 'K', 'I', 'L', 'L', '_', 'C', 'O', 'M', 'M', 'A', 'N', 'D', '_', 'L'
            };
            static const char kGmName2[0x1d] = {
                '\xbb', '\xfe', '\xc0', '\xaf', '\xb8', '\xae', '\x00',
                'S', 'K', 'I', 'L', 'L', '_', 'C', 'O', 'M', 'M', 'A', 'N', 'D', '_', 'L', 'O', 'G', ' ', ':', ' ', 'C', 'U'
            };
            if (strncmp(packet.m_name, kGmName1, 0x1d) == 0 ||
                strncmp(packet.m_name, kGmName2, 0x1d) == 0)
            {
                out.m_4 = static_cast<int>(0x7fffffff);
                return 0;
            }
        }

        // [b23S3C] P48：ORIG 两代理点各调 user->GetServerGroup() 一次（p167
        // MISSING -1 即此），无 group 槽。
        CMonitorServerProxy* monitor =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitor->SendPacket(reinterpret_cast<char*>(&packet), 0x2c);
        CGuildServerProxy* guild =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        guild->SendPacket(reinterpret_cast<char*>(&packet), 0x2c);
        return 0;
    }

    unsigned int accId = user->get_acc_id();
    LineFunc(0x269f,
             __PRETTY_FUNCTION__,
             out.m_4, accId);
    return 0;
