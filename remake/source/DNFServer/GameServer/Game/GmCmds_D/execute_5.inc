// ORIG execute 0x084b0a08：event_script_mng->test_process_change_day(day)。
void CInGameEventSetDay::execute()
{
    // ORIG：this+8 物化 %ebx 跨 2 call；mng 结果 eax 直通 receiver，无具名槽。
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->test_process_change_day(((const GmDFld<int>*)this)->f08);
}