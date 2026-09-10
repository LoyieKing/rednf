DisPatcher_SetArea::~DisPatcher_SetArea() {}
// ORIG 0x081c93e8 T _ZN18DisPatcher_SetArea11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/check_error.md 汇编+反编译 C 全文转写）：
// state!=3 → 0x7fffffff；IsBusyDisjointStore → param.m_8=req.m_0d, 0xbf；
// IsBusyPrivateStore → param.m_8=req.m_0d, 0x3c；IsCurCharacGhost → param.m_8=req.m_0d,
// 0x13；IsForbiddenMove(name) 且剩余时间!=0 → param.m_9=req.m_0e, 0x44；时间==0 则
// EnableMove(name)；否则 0。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
