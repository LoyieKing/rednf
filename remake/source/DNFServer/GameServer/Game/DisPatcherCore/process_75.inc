int DisPatcher_EventDungeon_DestoryObject::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // ORIG 0x0812bd8c：组件视图单指令 disp 访存（P1 范式），
    // 写入序 +0x4←+0xe（m_point ← m_roomIndex）。
    CMDPacketStruct::_STReqEventDungeonDestoryObject* rMsg =
        reinterpret_cast<CMDPacketStruct::_STReqEventDungeonDestoryObject*>(&msg);   // ORIG -0x8(%ebp)
    CMDPacketStruct::_STResEventDungeonDestoryObject* rParam =
        reinterpret_cast<CMDPacketStruct::_STResEventDungeonDestoryObject*>(&param);  // ORIG -0x4(%ebp)
    rParam->m_point = rMsg->m_roomIndex;
    check_error(user, *rMsg);   // ORIG：经局部槽 -0x8 reload 传参（非直读 0x10(%ebp)）
    return 0;
}
