Inter_Disconnect::~Inter_Disconnect() {}
// ORIG 0x084beb3c T dispatch_sig —— 用户断开清理：状态门/扩展槽切换/付费税上报/各管理器离开通知/DB 收尾。
// 报告 docs/class_func_reports/Inter_Disconnect/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=176（ORIG 0x084beb3c）：寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=52（ORIG 0x84beb3c）：N18a 单段 regmove+N82/P112 帧槽平移。
// [lane2 2026-09-10] verdict=DIFF→AE：真 bug 修复 —— ① private_store vtable 槽错位（旧 `->Enter(user)`
//   走槽+1=0x85caa8a；ORIG 084beb3c 段内为槽+0=`CPrivateStoreMgr::Exit` 0x85ca994，nm T 直证）改 `->Exit(user)`；
//   ② SendMemberPayTax 后两实参（is_connect_upper_member/getCurCharacNo）由具名局部改内联实参
//   （ORIG %esi/%ebx 跨 GetServerGroup/GetServerProxy 保活，P52/P200）；③ assaultMgr 去具名局部。
