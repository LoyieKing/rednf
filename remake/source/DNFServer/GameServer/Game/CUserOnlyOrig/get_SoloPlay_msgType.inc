// ---------------------------------------------------------------------------
// CUser::get_SoloPlay_msgType(MSG_REQ_TO_PEER const&) T 0x0868dd76 sz0x4c
// 报告：docs/class_func_reports/CUser/get_SoloPlay_msgType.md
// 注：MSG_REQ_TO_PEER 当前仅有前向声明（ORIG 亦无完整布局符号），+0xf 为
// SoloPlay 请求子类型 byte（movzbl+movsbl 双指令形态，与 ORIG 一致）。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig IDENTICAL] ORIG 0x0868dd76：调用点/形态已对齐（本波修法见函数内注释）。
int CUser::get_SoloPlay_msgType(const MSG_REQ_TO_PEER& msg)
