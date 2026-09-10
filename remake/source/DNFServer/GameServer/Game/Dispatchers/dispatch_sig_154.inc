int Dispatcher_CheckEnableServerMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821df6a（PacketDispatcher_Impl_1.cpp 0xd236 区）
    if (user->get_state() != 3)
    {
        char* idStr = NumberToString(user->get_acc_id(), 0);
        int state = user->get_state();
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xd236, 0);
        trace("ServerMessage : Dispatcher_CheckEnableServerMessage => User State : %d, m_id : %s",
              state, idStr);
        return LineFunc(0xd237,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char msgIdx = 0;
    if (!packet.get_byte(msgIdx))
    {
        return LineFunc(0xd23d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    PacketGuard guard;
    guard.put_header(1, 0xde);
    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        int cond = 0;
        if (!mgr->CheckUserCondition(user, (int)msgIdx, cond))
        {
            guard.put_byte(0);
            if (cond != 0)
            {
                guard.put_byte(cond);
            }
            else
            {
                guard.put_byte(3);
            }
        }
        else if (!mgr->IsChangableMessage((int)msgIdx, curSec))
        {
            guard.put_byte(0);
            guard.put_byte(1);
        }
        else
        {
            guard.put_byte(1);
        }
    }
    guard.put_byte((int)msgIdx);
    guard.finalize(true);
    user->Send(guard);
    return 0;
