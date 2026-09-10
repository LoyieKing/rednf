bool Manager::finishLoad(CUser* user)  // ORIG T 0x08130eb2（返回值域仅 0/1，bool 化安全）
{
    StageControl* control = user->getAdvanceAltar();
    if (control != 0)
    {
        if (!user->m_advanceAltar.isAvailableTciket())
        {
            return 1;
        }

        control->getProcStage()->setState(ProcStage::State::S_N1);

        PacketGuard guard;
        guard.put_header(0, 0x1e);
        guard.put_int(0);
        guard.finalize(true);
        user->Send(guard);
        return 1;
    }
    return 0;
