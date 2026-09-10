void StageControl::sendStageInfo()
{
    CUser* user = reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return;
    }

    // ORIG 用 _NotiAdvanceAltarStageInfo（memset 0,8 的 ctor + 两 int 直赋），
    // 栈上两 int 连续成 8B payload；P48 去 body[]+memcpy 的二次拷贝形。
    NOTIPacketStruct::_NotiAdvanceAltarStageInfo body;
    body.m_stageIndex =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageIndex();
    body.m_stageDifficulty =
        reinterpret_cast<ProcStage*>(m_procStage)->getStageDifficulty();

    PacketGuard guard;
    guard.put_header(0, 0x209);
    guard.put_binary(reinterpret_cast<char*>(&body), 8);
    guard.finalize(true);
    user->Send(guard);
