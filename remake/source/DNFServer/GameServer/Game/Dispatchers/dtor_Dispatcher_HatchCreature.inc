Dispatcher_HatchCreature::~Dispatcher_HatchCreature() {}
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081cd5aa
//   [N82→P0/P200] 根因=slot/invenType 具名局部 + pMsg 声明位错 + 裸偏移 add 形；
//   改 pMsg 提前声明（secu 前，N98 首次使用序）、slot 表达式直传、具名成员视图
//   （disp 折叠）⇒ 归零。
