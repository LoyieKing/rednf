void APSystem::CUserProc::SendGetRewardItem(CUser& user,
                                            _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward,
                                            bool flag)
{
    PacketGuard guard;
    ((InterfacePacketBuf*)&guard)->put_header(1, 0x229);

    UserProcRewardPayload data;
    data.itemIdx = reward.m_itemIdx;
    data.flag = flag;
    ((InterfacePacketBuf*)&guard)->put_binary((char*)&data, 5);
    ((InterfacePacketBuf*)&guard)->finalize(true);
    user.Send(guard);
