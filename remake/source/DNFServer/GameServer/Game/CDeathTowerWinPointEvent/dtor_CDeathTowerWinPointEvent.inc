CDeathTowerWinPointEvent::~CDeathTowerWinPointEvent() {}
// 0x0811706e：m_ratio = param.m_duration 后虚调 StartEvent()。
// b28c-L20: N7 — 唯一残差为虚调 StartEvent() 的间接 call 目标寄存器：ORIG 0x0811706e
// `mov (%eax),%edx; mov this,%eax; mov %eax,(%esp); call *%edx`，我们
// `mov (%eax),%eax; mov this,%edx; mov %edx,(%esp); call *%eax`。4.4 间接 call 目标恒
// %eax（probe4b 七形：裸调用 / this-> / (*this). / 基类指针 / 具名指针局部 / 返回值 /
// 局部中转 全部产 eax 目标）⇒ N7 封顶，ndiff=8 仅寄存器互换。
// 4.4.6 后端实测（/tmp/tu_CDeathTowerWinPointEvent_446.o，cc1plus446 驱动）：本件 ndiff=0
// ——4.4.6 间接 call 目标用 %edx、this 用 %eax（与 ORIG 4.1.2 同形）⇒ 路由切 4.4.6 后可 AE。
