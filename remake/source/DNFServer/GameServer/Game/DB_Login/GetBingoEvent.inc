// [b28d-solo-DB_Login] verdict=DIFF ndiff=2 gap=-1（N107：成员常量偏移取址 ORIG
// `lea 0x413c(%eax),%edx` vs 4.4 `add+mov`；P0 resetBase unsigned 化修装载时刻
// （5→2）。p167 CLEAR）。
// S216：union{unsigned int;int} 同址 int 视图实测仍产 add+mov（探针
// /tmp/s216_probe2.cpp fA/fB）⇒ 维持 N107（成员常量取址 lea-vs-add）封顶。
// [Lane26 复核] 再探具名数组成员取址 `arr[i].m_id`、union 数组视角 `u.m_ids[0]`
// （/tmp/l26p/pc.cpp t2/t3）实测仍 `add $0x413c,%eax; mov %eax,%edx`，4.4 对成员
// 取址恒 add、无 lea 路径 ⇒ 维持 N107；jcc/jmp 目标下标本件 bmm=8 全为位移差。
bool DB_Login::GetBingoEvent(SIG_LOGIN_DATA* sig)
