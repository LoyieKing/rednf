DisPatcher_EventDungeon_ClearRoom::~DisPatcher_EventDungeon_ClearRoom() {}

// ORIG 0x085957e8 W _ZN33DisPatcher_EventDungeon_ClearRoomC1Ev（无报告，objdump 转写）：
//   调 IPacketDispatcher<CMDPacketStruct::_STReqEventDungeonClearRoom,
//                        CMDPacketStruct::_STResEventDungeonClearRoom, ch_state3>::C1
//   后写 vptr=0x8b66068。基类实参已按 ORIG mangled 统一（OrigPktDisp 批），
//   空体 ctor 由编译器生成基类构造+vptr 写入，与 ORIG 逐指令等价。

// ORIG 0x0812beb0 T _ZN33DisPatcher_EventDungeon_ClearRoom11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/check_error.md 全文转写：
//   push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret）
