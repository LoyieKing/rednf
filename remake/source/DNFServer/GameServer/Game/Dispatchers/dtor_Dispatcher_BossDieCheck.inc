Dispatcher_BossDieCheck::~Dispatcher_BossDieCheck() {}
// 技能校验和比对（不等记 hack 0x25c）；从战场 m_apcList(party+0xc3c) 删除
// 上报 boss 序号(msg+0x10)，未命中 param=0x15；删空后记时刻、给队长挂 100ms
// 定时器(param3=characNo) 并置 param.m_8=1；msg+0xe==-1 记 hack 0xcb。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cd7a2 orig/ours=185/197 ndiff-blocks=25
//   帧差 sub $4c vs $6c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x4c,%esp | mov    %eax,-0x24(%ebp) | mov    %eax,%edx
