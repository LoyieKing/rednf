// Update —— 报告全文转写。ORIG T 0x08124386。
// [b28d-27] DIFF N82/N89：ORIG 0x08124386；p167 CLEAR（22/22），残差为 SQL/STL
// [b28d-solo-APSystemOnlyOrig] 判词：**gap 0**（本轮还原 ORIG 0x08124472..d7 指令序：zipLen=0x400 → `cmp $0x7ff; jbe` → memset → compress_zip；旧版把检查放 compress 后、memset 放 zipLen 前 ⇒ 整段错位）。残差 = 实参装载时刻（ORIG 提前装 sig.m_apSum 到 %ebx）+ 寄存器分配（N7/N82）。p167 CLEAR 22/22。
// 实例与帧/槽分配，语义闭合。
// [S5-2026-09-11 §6.5 核查] 本件两条 SQL 串与 ORIG 逐字节命中（**TAB 敏感**比对：
//   `update charac_action_point set ap_sum=%d, is_reward_medal=%u, \t…\tis_reward_item_1=…`
//   与 `insert into charac_action_point\t…\t(occ_date, charac_no, ap_sum, …`）；变参顺序
//   按报告逐条对齐；enc_kr_scan = 0 mismatch（无 §6.5.1 编码盲区）；本件无 prett 引用
//   （ORIG 中不存在 `…DB_UpdateActionPoint::Update…` 的 __PRETTY_FUNCTION__ 串）。
bool DB_UpdateActionPoint::Update(bool bExpireDate, const _SIG_LOAD_ACTION_POINT& sig)
