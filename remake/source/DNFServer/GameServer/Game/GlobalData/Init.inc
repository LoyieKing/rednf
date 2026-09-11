// ORIG T 0x084718bc：!InitTcp() 返 0，否则返 1（P22 bool 取反形态）。
// [G7-sub 2026-09-11 判词] CPCRoomServerProxy::Init（ORIG T 0x084718bc，`!InitTcp()`
// 返 0 否则返 1）：单函数报告 game_func_report 实测 ORIG/我方 **14/14 条**，逐条
// 规范化后**零差异**（形完全一致）；无调用点差。结论：**封顶，无改动**。
// 注：本件（GlobalData/Init.inc）只含 CPCRoomServerProxy::Init；`GlobalData::Init`
// （ORIG 0x08299fa0，3139 条）实际在 GlobalData/Init_3.inc，判词见该处/回执。
bool CPCRoomServerProxy::Init()
{
    if (!InitTcp()) return 0;
    return 1;
}