bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);

// [b28d-PoolMisc2 GI-q06 纯移位] TU 首符号需为 TOD_UserState 构造体外联定义
// （ORIG T 0x08643460）⇒ 定义块整体上移至本 TU 第一个发射位置（类声明来自上方
// DataManagerInitDepends.h，空体无额外依赖）。file-scope 对象声明序一字未动。
// ORIG T 0x8643460：charac_expand::CData ctor 后写本类 vptr，再 call
// SIG_REQUEST_TOD_USER_STATE::C1(this+0x8)（0x08491f68）——其构造对数据区
// +0xc（即内嵌 m_data.m_todayLayer）以 TOD_Layer(0) 初始化，其余字段不动。
// 以真实内嵌成员 m_data 建模 ⇒ 编译器发 call SIG::C1，与 ORIG 汇编同构。
