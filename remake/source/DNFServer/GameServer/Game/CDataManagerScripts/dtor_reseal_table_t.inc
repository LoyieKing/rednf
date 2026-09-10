reseal_table_t::~reseal_table_t() {}

// ORIG get_reseal_cost @0x08900b02（T）。见 CDataManager.h 声明注释。
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（3/3）。有符号除 10 已翻正
// （`int idx=(param_2-1)/10` ⇒ ORIG magic 0x66666667）；本轮交换第二项乘法操作数
// ⇒ 消 push %ebx、帧 0x38=ORIG、idx 槽 -0xc=ORIG（判词旧读数 -0x10 作废）；
// 残余 2 条 = ①`cmp -0xc(%ebp),%eax` mem 直比 vs 4.4 `mov -0xc,%edx; cmp`（P209）
// （P209 unsigned 比较恒载寄存器）②`mov %eax,%ecx` 单段 regmove（N18a）；ORIG 0x08900b02。
// [F2d-C05② 甄别] 非命中形：比较对象之一是跨调用返回值 `m_vec.size()`（非两具名局部）；
//   S1 一次性旋钮本件已用尽（idx 改 unsigned 实测劣化回滚）；show_func_diff orig=69 our=71
//   gap=-2 ndiff=8 OURS-only={mov×2}。判 N18b/P209 + N18a。
