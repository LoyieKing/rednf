DisPatcher_FinishLoad::~DisPatcher_FinishLoad() {}
// ORIG W 0x08265492 _ZN21DisPatcher_FinishLoad11check_stateEP5CUserR9PacketBuf
// （无独立报告；objdump 全文转写）。Get_State_Check(user->get_state()) 为真
// （state ∈ {5,6,12,13}）→ 0，否则 0x7fffffff——xor $1/test/je 分支骨架与
// 上方 check_error 注释一致。依赖 Global::CGlobalFunc::Get_State_Check
// （ORIG T 0x081466f0，声明 BossTowerFunc.h、定义 BossTowerFunc.cpp）。
