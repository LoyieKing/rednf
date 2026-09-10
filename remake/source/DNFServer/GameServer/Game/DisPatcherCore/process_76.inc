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
