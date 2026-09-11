// ORIG 0x0812beba T _ZN33DisPatcher_EventDungeon_ClearRoom7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/process.md 全文转写，
//   含汇编写入次序：param+0x4←msg+0xe、param+0x8←msg+0x12、param+0x6←msg+0x10）；check_error 恒返 0。
int DisPatcher_EventDungeon_ClearRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // ORIG 0x0812beba：组件视图单指令 disp 访存（P1 范式），
    // 写入序 +0x4←+0xe、+0x8←+0x12、+0x6←+0x10（roomIndex/monsterCount/monsterIndex）。
    CMDPacketStruct::_STReqEventDungeonClearRoom* rMsg =
        reinterpret_cast<CMDPacketStruct::_STReqEventDungeonClearRoom*>(&msg);    // ORIG -0x8(%ebp)
    CMDPacketStruct::_STResEventDungeonClearRoom* rParam =
        reinterpret_cast<CMDPacketStruct::_STResEventDungeonClearRoom*>(&param);  // ORIG -0x4(%ebp)
    rParam->m_roomIndex = rMsg->m_roomIndex;
    rParam->m_monsterCount = rMsg->m_monsterCount;
    rParam->m_monsterIndex = rMsg->m_monsterIndex;
    check_error(user, *rMsg);   // ORIG：-0x8 槽 reload（非直读 0x10(%ebp)）
    return 0;
}
