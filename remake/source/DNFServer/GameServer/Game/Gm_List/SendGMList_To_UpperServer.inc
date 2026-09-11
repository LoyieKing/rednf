// 类声明权威点在 CGuildServerProxy.h；此处仅实现。
void Gm_List::SendGMList_To_UpperServer(int param)
{
    if (param != 0 && param != 1)
        return;

    // 探针实测（/tmp/pp6.cpp vs /tmp/pp7.cpp）：形参为 enum 的调用点，若实参处
    //   现场做 int→enum cast，4.4 发「实参先载」`mov slot,%edx; mov <A>,%eax`；
    //   实参本身即 enum 类型时发 ORIG 形「receiver 先载」`mov <A>,%eax;
    //   mov slot,%edx`（ORIG 0x08299e03/0x08299e33 段逐条同形）⇒ 局部声明为 enum。
    ENUM_SERVER_GROUP serverGroup =
        static_cast<ENUM_SERVER_GROUP>(G_CEnvironment()->get_server_group());
    Packet_Sync_GM_List packet;
    // P183/P0：ORIG 0x08299d8c 段为 `call GetGMList; mov %eax,-0x10(%ebp)` 单条
    //   指针落槽，全函数无 list 拷贝构造/D1 —— 原码是引用绑定，非按值拷贝
    //   （按值拷贝引入 C1ERKS_ + 2×D1Ev + _Unwind_Resume + push %esi，gap +20）。
    const std::list<WongWork::CGMAccounts::stGMInfo_t>& gmList =
        GlobalData::s_pGMAccounts_->GetGMList();
    std::list<WongWork::CGMAccounts::stGMInfo_t>::const_iterator it = gmList.begin();
    int count = 0;
    while (it != gmList.end())
    {
        packet.m_gmIds[count] = it->mid;
        packet.m_gmNames[count] = static_cast<char>(it->type);
        ++count;
        if (count == 0x14)
        {
            // b28d-L3 复测（P34/P52）：ORIG 0x08299df4 `mov -0xc(%ebp),%eax;
            //   mov %al,-0x81(%ebp)` ⇒ 原码用 count 变量（此时 ==0x14），非立即数。
            packet.m_gmCount = static_cast<unsigned char>(count);
            if (param == 1)
            {
                // ORIG 0x08299e03 段：proxy **无具名槽**（`call GetServerProxy;
                //   mov %eax,(%esp)` 直传）⇒ 删具名局部、改单语句链式。
                GlobalData::s_guild_proxy_mgr->GetServerProxy(serverGroup)
                    ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
            }
            else if (param == 0)
            {
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(serverGroup)
                    ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
            }
            ++packet.m_pageNo;
            count = 0;
        }
        ++it;
    }
    if (count != 0)
    {
        packet.m_gmCount = static_cast<unsigned char>(count);
        if (param == 1)
        {
            GlobalData::s_guild_proxy_mgr->GetServerProxy(serverGroup)
                ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
        }
        else if (param == 0)
        {
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(serverGroup)
                ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
        }
    }
}