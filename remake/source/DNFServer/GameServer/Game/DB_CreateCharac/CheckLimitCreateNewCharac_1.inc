// ===========================================================================
// CheckLimitCreateNewCharac(SIG*, bool) — 报告 @0x08401922
// count>1 拒建并 trace [LIMIT NEW CHARAC MID]；bool 参不影响路径（ORIG 仅
// 存栈未读）。
// [b21 N·P102/P197-④] 语义已闭合：isGM 早返、GetDBHandle(1,0)+time+localtime_r 清
// 时、set_query(不查返)+exec、nRows==0 返 true、遍历 fetch/get_uint 双列、
// count>1→m_resultCode=5 + [LIMIT NEW CHARAC MID] trace(fmt,NumberToString,
// m_characNo,lastAccessTime,get_server_group) 拒建，调用链/常量/字段偏移逐条对齐。
// 残余：ORIG 把 fetch/get_uint 返回 materialize 到 `-0x29`（local_2d char 槽）再
// `movzbl;xor;test`（P102 同槽多查），4.4 对 `!a()||!b()||!c() break` 短路直跳；trace
// 实参寄存器缓存序 N82。无子母，封 N。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08401922 158=158。
// 修法：bool 形参空 if 体复现头部 spill（-0x7c）+ P102 同一 bool ok（-0x29）+ lastAccessTime
// 先声明（-0x44/-0x48）+ P232 dead 槽 -0x20 + P227 全表达式临时链（%esi/%edi/%ebx）。
bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig,
