// ORIG 0x0812bd8c T _ZN37DisPatcher_EventDungeon_DestoryObject7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_DestoryObject/process.md 全文转写：
//   局部引用槽 -0x8=msg、-0x4=param；*(u16*)(param+0x4)=*(u16*)(msg+0xe)；check_error；返回 0。
//   模板实例化中 ParamBase 栈对象仅 4 字节 vptr（ORIG dispatch_template 0x824aa8c 实证：
//   param@-0x1d 与 msg@-0x19 相邻），param+0x4 写入与相邻 msg 存储重叠为 ORIG 原始行为，忠实转写。）
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
