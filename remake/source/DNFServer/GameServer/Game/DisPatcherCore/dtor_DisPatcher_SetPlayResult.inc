DisPatcher_SetPlayResult::~DisPatcher_SetPlayResult() {}
// ============================================================================
// ORIG T 0x081f4ce6 _ZN24DisPatcher_SetPlayResult12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetPlayResult/dispatch_sig.md，1083 insn）
// state==5 → party 校验（state==2 直接 0 / m_battleField.m_bloodState==1 跳过）→ GameResultType
// 读包（Clear 后逐字段直写 party 结果区【wire 布局按 ORIG +0x17f4..0x1846】）→
// 地图清除日志/金币/经验比率反作弊（0x1fb、0x78、0x75）/疲劳特化（job 3,4 且
// growtype 2 → ×0.5 否则 ×0.8）/清副本 timer 注册/角色信息核对（0xe2、0xe3）/
// 使用技能材料比率（0xe5、0xe4）/尾巴成员药丸检查（0x1a2）→
// CheckRecvFromAllMember → SetPlayResult。
// ============================================================================
// SetPlayResult::dispatch_sig（ORIG 0x081f4ce6，报告 dispatch_sig.md 全读）。
// 残 gap≈260 归 N78（Main 批准 2026-09-05）：ORIG +0x7e0..+0x906（081f54c6/
// 081f5528/081f559e）4.1 将 memset 0x703 / memcpy 0x676 / memcpy 0x78 inline 成
// rep stos/rep movsl 逐段展开；4.4 O0 恒 `call memset/memcpy`（GDataC8 探针实证
// 同族 N78），单 TU 源码写法不可翻。+0x99..+0xc4 blood-dungeon 守卫缺段已补
// [b28d-29] P0 清绑（register asm("al") 已删）+ P167 修正 5 处 get_int(int&) 重载
// （ORIG 0x081f4f57/0x081f59cc.. 实证 ERi；coinSlot 0x081f50dd 保持 ERj）。
// 残差：CHARAC_DATA/TAG_CHARAC_DATA 本仓建模为 POD（ORIG 有 C1 call，P104 建模差）
// + 帧 0x1d0c vs 0xf54（N82 4.1 不复用大缓冲槽）+ setcc-fold。  (ORIG 0x081f4ce6)
// （语义修复保留）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 943（ORIG 1083 / ours 816），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] **N129 登记（Main 2026-09-09 裁定）：本件残留的 2 个 MISSING ctor 属
// 「ORIG 死复制/死构造区：缺 callee 但净效果为零 ⇒ 不补」，后来泳道请勿再试图补。**
// 逐地址证据（ORIG T 0x081f4ce6 内 0x081f5489..0x081f5650，0xe2 检查块；
// 0x081f56e8.. 的 0xe3 块同构）：
//   0x081f5489 lea -0xe9e(%ebp),%eax ; 0x081f5492 call _ZN11CHARAC_DATAC1Ev  (W 0x0822ec4c)
//   0x081f5497 lea -0x828(%ebp),%eax ; 0x081f54a0 call _ZN15TAG_CHARAC_DATAC1Ev (W 0x0822edae)
//   0x081f54ab..  rep stos  memset(-0x15bb, 0, 0x703)
//   0x081f5500..  rep movs  memcpy(dst=-0x15af  ← src=-0xe9e, 0x676)   ← CHARAC_DATA 全量
//   0x081f55fc    call _ZNK5CUser13GetCharacInfoEP15SIG_LOAD_CHARAC (0x0864bc04) ← 整体覆写 dst
//   0x081f5635    movl $0xe2 … addServerHackCnt
// ⇒ 被复制的 CHARAC_DATA 在块内**没有任何填充调用**（上述区间总共只有 4 个 call），
//   拷进去的是刚 Reset 的零对象，且目标缓冲随后立刻被 GetCharacInfo 全量覆写 ⇒ 净效果为零。
// ⇒ 若为消 MISSING 而补这两个对象，必须连带补 2 次 rep movs（4.4 O0 只会发 call memcpy，
//   ORIG 侧无 memcpy 符号）⇒ 净增 2 项 EXTRA，形态与语义双亏 ⇒ **判不修**。
// 本件其余部分已按 P183（ORIG 逐处重调、我方局部缓存）展开 5 个无副作用 getter：
//   实测 ndiff 772→726，P167 计数 87→92 与 ORIG 相等，行为等价。
// [BigSetPlayResult] 2026-09-09 复核（ORIG 1083 / ours 874，gap 209，P167 92/92）：
// 本件真缺口 2 类 4 处，均已修（全部在 N129 带之外的 0xe2/0xe3 检查块）：
//  ① 0xe2 块原读 `(char*)&sigCharac + sizeof(SIG_LOAD_CHARAC)` = +0x703 **越界**；
//     ORIG 0x081f5605 实证读 -0x158a = 缓冲 -0x15bb + 0x31
//     = m_characData(+0xc)::m_lev(+0x25)（pack(1)）。
//  ② 0xe3 块原读 `(char*)&sigChk + 0`（m_accId 低半字）；ORIG 0x081f583f 读 -0x1c8d
//     = 缓冲 -0x1cbe + 0x31，同为 m_characData.m_lev。
//  ③ 两块的 GetCharacInfo 返回值门禁缺失（原为逗号表达式 `(call(), cmp)`，返回值被丢弃）；
//     ORIG 0x081f5601/0x081f583b 均为 `test %al,%al; je` 先于字段读取（即 && 短路）。
// 形态收敛（P0 局部建模，语义不变）：
//  - 金币余数区改回 ORIG 的两个具名局部（over=-0x34 / rest=-0x30，均在
//    addServerHackCnt 之前算好；原写法 %10000 在 call 之后重读内存）。
//  - 疲劳比率区改为 if/else 两分支各自内联（ORIG 0x081f5202/0x081f526b），
//    消除 bool 槽 + double ratioDelta 局部；常量实证 .rodata 0x8bd6320=0.5 /
//    0x8bd6328=0.8 / 0x8bd6354=100.0f / 0x8bd6358=99.98f。
// 残差 = N129/N78 带 223 条（0x081f5489..0x081f5650 / 0x081f56eb.. 两段
// CHARAC_DATA/TAG ctor + memset/memcpy 内联展开；我方为 SIG_LOAD_CHARAC C1 +
// call memset）+ 其余 233 条未匹配全为形态（帧 0x1d0c vs 0xf54、槽平移、
// fucompp vs fucomip、局部缓存指针）。
