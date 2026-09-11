// ORIG 0x0812c1c6 W _ZN33DisPatcher_EventDungeon_ClearRoomD1Ev/D2Ev/D0Ev（三地址相同）
// objdump 实测 D1：写 vptr=0x8b66068 → 调 IPacketDispatcher<...>::D1 → 恒假 delete 守卫
// —— 即编译器生成的空析构，{} 与其逐指令等价。
DisPatcher_EventDungeon_ClearRoom::~DisPatcher_EventDungeon_ClearRoom() {}
