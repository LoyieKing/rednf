Dispatcher_SaveGameOption_ChattingEmoticon::~Dispatcher_SaveGameOption_ChattingEmoticon() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cf52a orig/ours=140/135 ndiff-blocks=31
//   帧差 sub $24c vs $250 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x24c,%esp | mov    %eax,-0x28(%ebp)
// [SweepDispatchers3 2026-09-09] 修真缺口：两次 << 前补 operator* 链（P132，
//   (*guard)->operator<<），p167 MISSING operator*×2 归零；config 拷贝保留 memcpy
//   （ORIG 是 rep movsl 0x84 dword；结构体赋值在 4.4 退化为逐成员 +42 条，已试并回退）。
//   gap 4→1（140 vs 139），残差 = rep movsl vs call memcpy（N78 族）。
