int Timer_EventManagement::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 读 CEnvironment +0x37c int（字段未具名，仓库既有 reinterpret_cast 风格同 APSystemOnlyOrig.cpp:237）
    // ORIG 0x0863ab92：mov 0x37c(%eax),%ebx（register 直落 %ebx）；
    // mng 无具名槽（链式）。
    register int dayValue = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    static_cast<EventClassify::CEventScriptMng*>(G_CDataManager()->get_event_script_mng())
        ->update_time_values(dayValue);
    RegistNextTimer(0x3c);
    return 1;
