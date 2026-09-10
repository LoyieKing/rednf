Dispatcher_EventCreateDnfRequest::~Dispatcher_EventCreateDnfRequest() {}
// ORIG 0x81a0362（ARAD__DISPATCHER__Dispatcher_EventCreateDnfRequest/dispatch_sig.md 全文转写）：
// packet==0→0x7b3；getCurCharacR==0→0；state∉[3,4]→0；s_event_manager->GetRepeatEvent(0xa0)
// 取 CEventCreateDnf；事件未进行（event==0 或 !IsEventing(0)）→log(0x7c1)+0；进行中且
// （GetEventCreateDnfReward==1 或 !IsRewardCondition）→SendCmdErrorPacket(0x258,1)+log(0x7d6)；
// 否则 SetEventCreateDnfReward(true)+DBInsertCreateDnfUserInfo::makeRequest+SendReward。
// 末尾统一发 0x22d 响应（put_header(0,0x22d)+put_short(GetRate)+put_byte(0)）并返 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081a0362 orig/ours=183/188 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x30,%esp | mov    &_ZN10GlobalData15s_event_managerE,%eax | mov    %eax,-0xc(%ebp)
