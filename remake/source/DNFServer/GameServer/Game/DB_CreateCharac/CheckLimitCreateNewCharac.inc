// ===========================================================================
// CheckLimitCreateNewCharac(SIG*) — 报告 @0x08401504
// 当日（本地零点起）已建 >1 角色拒建（rc=5）；exec 失败 rc=2。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08401504 111=111。
// 修法：mktime 结果写回 nowSec 同槽（-0x20）+ P102 同一 bool ok（声明在 db 前取 -0x19）
// + P225 拆嵌套 if 消 && 物化 + 比较写 nowSec<=createTime（unsigned long 语义）复现 ja。
bool DB_CreateCharac::CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig)
