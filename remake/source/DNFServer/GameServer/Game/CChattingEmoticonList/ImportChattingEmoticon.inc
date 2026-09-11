// ============================================================================
// ImportChattingEmoticon —— ORIG T 0x88e31b4（报告
// docs/class_func_reports/_lt_global_gt_/ImportChattingEmoticon.md）。
// RDAR 双相派发族（P163/P164/P165/P177，与 importCraneMinigameScript
// 同构，见 CraneMinigameManager.cpp）：函数级静态 __lookup
// (map<const char*,int,less_TCHAR>，guard+atexit)/__init_lock(SpinLock)/
// __inited(bool) 三件套位于重派发标签后，每轮穿 guard；注册轮持锁 id=0 →
// case 0 与 case 0x2f 共 leaf（+0x275）→ 贯穿 insert "[type]"0x2f →
// "[ani]"0x40 → "[macro]"0x46（+0x2c1/+0x39e jmp），公共尾 +0x430
// leave+置位+jmp +0x80 以当前 token 重查派发。建表后派发：
// [type]：ScanStr(value) → "EVENT"→m_type=1 / "CASH"→2 / 其它→4，随后
//         ScanInt(&m_param)（失败 return false）；
// [ani]/[macro]：ScanStr 直写 script+0xc / +0x10。
// +0x5b `movl $0x0,-0x20(%ebp)` 为只写死局部，照抄（P171）。
// 收敛口径（§6.2 三件套）：帧 `sub $0x7c` + push edi/esi/ebx 全等；id/死槽
// -0x1c/-0x20 与 ORIG 同槽；P167 调用点差集 42/42 全命中（唯一差集为
// __cxa_atexit @plt 显示）。**本波已归零**（DIFF→AE）：P164 显式回边标签
// 取代 for(;;)+break，消掉 4.4 的入口旋转岛；`token.c_str()` 内联进 find
// 调用（去掉具名 key 局部）使 string/迭代器槽与 ORIG 全等。
// 全函数逐条全等（仅 .o 未重定位的数据地址呈 AE）。
// ============================================================================
bool ImportChattingEmoticon(EmoticonType::EmoticonScript& script,
