// SetTodayAction —— 报告汇编+反编译全文转写。ORIG T 0x0812161c。
// [b28d-solo-APSystemOnlyOrig] ORIG 0x0812161c（Ghidra C + objdump 逐段）：
// ①time_t 单局部 -0x4c（旧版 now/tt 双局部）；②`it != table.end()`（ne，非 eq）
// 且 end 走具名临时 -0x4c 槽；③`list` 为具名指针（ptEv 结果）；④candidates 为
// **空 vector + 独立建表循环 `push_back(&(*list)[pass])`（prvalue ⇒ OS3_ 右值重载）**
// ——旧版预尺寸 C1EjRKS3_RKS4_ 缺 C1Ev/push_back(&&)；
// ⑤过滤循环用 `candidates[i]=0` 标记 + goto next（每字段重调 operator[]，共 11 次）；
// ⑥group 判定链顺序 0x1d/0x1e/0x1f/**0x2d**/0x20（P46 case 序）；
// [b28d-solo-APSystemOnlyOrig] 判词：**gap 10→2**（本轮还原：标准 `for(;row<300 && it!=end; it++)` 循环旋转布局（4.1 init 后 jmp 底部条件）、P223 fA/fH goto 空标签岛补回 3 处 nop+jmp 岛、lo/hi 表达式直读；370 vs 368）。残差 = N18b（weekday 比较 2 条 mov 中转）+ N107（memcpy dest `add $0x15` vs `lea`）。p167 CLEAR 35/35。
// ⑦循环尾 `it++`（ppEi 后置，P28 增量侧）。
// [S5-2026-09-11 §6.5 核查] 本件**零字符串引用**（报告汇编 `$"…"` 抽取 = 0，与我方
//   .o .rodata 一致）：仅 SQL 家族外的纯逻辑 ⇒ 无 §6.5 风险；enc_kr_scan 0 mismatch。
void CActionPointManager::SetTodayAction(int level, unsigned int characNo,
