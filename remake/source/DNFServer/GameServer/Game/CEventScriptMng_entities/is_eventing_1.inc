bool CAttendance::is_eventing()
{
    // ORIG 0x810e75c：GetRepeatEvent(0x56) 判空后经 vptr+0x34 判 IsEventing(0)，
    // 结果与判空合成一个 bool 条件（双重归一化形态）。
    if (CEventEntity::is_eventing())
    {
        CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x56);
        // [b28d-solo-CEventScriptMng_entities] P226 扩展：ORIG 内层条件先物化到
        //   %eax（`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`）再判，属
        //   「零槽短路物化」——4.4 对裸 `if (evt!=0 && evt->IsEventing(0))` 恒
        //   jump-thread 成两条件共落 fail 出口（少 5 条）。用语句表达式包住整个
        //   条件（内不含具名变量）复现 ORIG 物化岛。
        if (({ evt != 0 && evt->IsEventing(0); }))
            return true;
    }
    return false;
