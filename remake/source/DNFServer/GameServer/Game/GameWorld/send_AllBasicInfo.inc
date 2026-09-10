void GameWorld::send_AllBasicInfo(CUser* user)
{
    PacketGuard packet;
    int countIndex = _makeBasicInfoHeader(packet);
    int count = 0;

    std::map<unsigned short, CUser*>::iterator it;
    CUserCharacInfo* info = NULL;
    if (m_UserInWorld.size() != 0)
    {
        it = m_UserInWorld.begin();
        goto COND;
    LOOP:
        info = it->second;
        // ORIG 0x086c848a `cmpl $0,-0x10; je 086c859d`（info==0 前向跳到函数后部的
        // trace 块）⇒ 正极性 `if (info != NULL) {visible 处理} else {trace}` 布局。
        if (info != NULL)
        {
            if (info->IsCurCharacVisible())
            {
                int index = packet.get_index();
                if (((CUser*)info)->make_basic_info((char*)&packet, 0) != 1)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xbe3, 5)(
                        "[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbe3);
                }
                if (packet.getLastError() != 0)
                {
                    packet.set_index(index);
                    packet.put_short(countIndex, count);
                    packet.finalize(true);
                    user->Send(packet);
                    countIndex = _makeBasicInfoHeader(packet);
                    count = 0;
                    goto COND;
                }
                else
                {
                    count++;
                }
            }
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbf6, 5)(
                "[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbf6);
        }
        ++it;
    COND:
        if (it != std::map<unsigned short, CUser*>::iterator(m_UserInWorld.end()))
        {
            goto LOOP;
        }
    }

    packet.put_short(countIndex, count);
    packet.finalize(true);
    user->Send(packet);
