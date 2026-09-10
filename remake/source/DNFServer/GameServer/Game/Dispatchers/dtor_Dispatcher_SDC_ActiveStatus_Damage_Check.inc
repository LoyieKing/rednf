Dispatcher_SDC_ActiveStatus_Damage_Check::~Dispatcher_SDC_ActiveStatus_Damage_Check() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ea16e orig/ours=309/262 ndiff-blocks=42
//   帧差 sub $47c vs $470 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x47c,%esp | movb   $0x0,-0x55(%ebp)
// [Lane1Dispatchers 2026-09-10] 判据①/jcc 目标下标比对 + P312/P311 形态收敛（详细见函数头后注）：
//   ④ 两缓冲前置 `char[1]` 数组死局部（P312 形态，probe1 cPad1 已证 4.4 可复现慢路）
//      ——**本件失效**：本函数局部众多，4.4 把 1/3 字节 pad 归入标量区、大数组仍从
//      4 对齐槽起（我方 -0x268/-0x45c vs ORIG -0x25d/-0x451），慢路未触发；
//      pad 尺寸 1↔3 实测同读数 ⇒ 余 ~58 条按 N78/N82 封顶（4.1 分配器行为）。
//   ⑤ `data` 改 pack(1) 视图指针 ⇒ `data+K`/`data[k]` 全部折成单条 disp 访存
//      （旧 char* + 常量偏移在 4.4 恒发 add 两步，ORIG 单条 `mov 0x3(%eax),%eax`）；
//   ⑥ 流血伤害缩放改直接 C cast（ORIG `fildl/flds/fmulp/fldcw/fistpl` 截断语义），
//      删旧「四舍五入」辅助形（fldz/fucomip/setae 共 9 条 ORIG 无）。
// [SweepDispatchers 2026-09-09] 真语义修复 + 形态收敛（262→269，ORIG 308；p167 双零）：
//   ① **删循环内 `destLen = 500;`**——ORIG 0x081ea373 段无重置（uncompress 后 destLen 保持
//      上次实际长度），旧码每轮重置改变第二次解压行为（真语义差）；
//   ② 补 ORIG -0x3c/-0x38 两个 dead 500 初始化与 srcLen(-0x34) 具名槽；
//   ③ 两缓冲改块内 `char src[500]={0}`（ORIG 为 4.1 内联对齐剥离慢路，我方 4.4 发简单
//      rep stos；P312 pad 试形未把数组顶到非 4 对齐槽 ⇒ 余 39 条按 N78 封顶）。
// pack(1) 视图：ORIG 对 -0x44 数据缓冲的 +0x03/+0x07/+0x25 非对齐访存为单条 disp；
// sizeof 0x29（typedef 校验在下方）。字段宽度按 ORIG movzbl/movsbl 链定。
