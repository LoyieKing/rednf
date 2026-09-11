// [b27 封顶] D1 残差（33 vs 43）= CStage 析构 EH 双份 epilogue（4.4 恒发
// ~CStage cleanup 块 + ebx/esi 保存，4.1 merge 为单块）——N91 同根（C1 同判
// 见 ctor 注），无源码解。
// [b28d-solo-CDeathTower] verdict=DIFF（orig 33 / our 41，P167：EXTRA CStage::D1 ×1）：
// 同 C1 根因（CPlayData dtor 未 throw()，定义在 WongWorkDeathTower_A.cpp）；
// throw() 实测负例已回退（见 C1 判词）⇒ 4.4 在 CPlayData D1 调用点布 CStage D1
// cleanup、ORIG 无 ⇒ N91 封顶。
CDeathTower::~CDeathTower() {}
