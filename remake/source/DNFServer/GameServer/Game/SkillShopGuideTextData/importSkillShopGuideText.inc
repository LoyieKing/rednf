// SkillShopGuideTextData 四件套（ORIG W 0x0891cb4c / 0x0891cb62 / 0x08932804 /
//   0x089328e6）已按判据㉓ 移入 SkillShopGuideTextData.h 类内 inline 定义
//   （ORIG 全 W ⇒ 头内定义由 ODR 使用点发射；.cpp 外联定义发射 T 是绑定缺口）。
// ============================================================================
// importSkillShopGuideText —— ORIG T 0x08914483
// 报告 docs/class_func_reports/_lt_global_gt_/importSkillShopGuideText.md
// 返回 int：0=失败（文件加载失败/段内 ScanStr 失败），1=成功。
// 静态局部：__lookup（map<const char*,int,less_TCHAR>，惰性 guard+atexit）、
//   __init_lock（SpinLock，惰性 guard+atexit）、__inited（bool，SpinLock 保护）。
// 控制流（汇编 LAB_08914553 do 循环）：__inited==0 时持锁注册全部段键后置 1，
//   回 do 头以 find 路径重处理当前 token；__inited==1 直接 find 分派。
// ORIG 三个 static 的 guard 检查位于 do 头（08914553/089145cf/0891464b，每次
//   迭代经 guard 跳过初始化），故声明须在循环体内。
// 注册链按 ORIG fall-through：case 按数值升序，__inited==0 时逐段注册后落入
// 下一 case；段键注册用直接 pair 构造（ORIG pair<char_const(&)[N],int> 模板实例）。
// ---------------------------------------------------------------------------
// b28d-solo-SkillShopGuideTextData 判词：语义完成（p167 CLEAR 76=76；帧 0x14c、
//   槽位 key/-0x104、data/-0x114、vec/-0x120、token/-0x124、type/-0x1c、
//   jobStr/-0x12c、growStr/-0x130 与 ORIG 全等；修真语义 bug：case "[/text info]"
//   原缺 vec.push_back(data)；case 0x3334 的 ok 走寄存器消 4B 槽，帧/槽全等）。
// b28d-solo-SkillShopGuideTextData 二轮（本轮）：
//   ① 循环尾 `if(__inited) continue;` → 负极性 `if(!__inited){leave;inited=true;goto loop;}`
//      （ORIG 0x08914d14 `movzbl __inited; xor $0x1,%eax; test %al,%al; je`；P168a/P194）：
//      gap 10→9，jcc 序列不变。
//   ② case 0x3334 的 ScanStr 检查块试过负极性形 `if(!ScanStr(...)){ok=0;return 0;}` +
//      ok 裸声明 + jobStr/growStr 嵌套作用域（三种 ok 类型组合）：可复现 ORIG 0x08914911
//      的 `call; xor $0x1,%eax; test %al,%al; je` 条件形，但 4.4 为 `ok=0/1` 额外引入
//      %esi/%edi 双寄存器 bool 归一化（净 delta +9→−7，代价是多出 1 个 `jne`：jcc 39≠38）
//      ⇒ 按「jcc 逐位一致优先」回退原形（保 jcc 38=38）。
//   读数（fresh check_tu_routed）：identical/ae/near/diff = 94/143/0/3（3 = 本件 +
//      importSkillShopTree + N123 static_init 撞名），missing_in_game=62 与基线一致；
//      gap：orig 551 / our 542（+9；残余 = ScanStr 失败块 ORIG `mov $0,%ebx`/
//      `mov $0,%esi`/`jmp` 物化 ×2 组共 6 条 + guard catch `mov %esi,%eax` 2 条 +
//      循环尾 `xor` 后 4.4 折掉 `test %al,%al` 1 条 + D 段 2 条，减循环入口/尾
//      `jmp+nop` 2 条）。
//   byte 层 N 封顶（N7 跨版本寄存器分配 / N18a 单段 regmove / P164 循环旋转 /
//   N122 `xor` 后 test 折叠 / 对齐 nop 岛）；源码形不可择，不再耗轮次。
// ============================================================================
int importSkillShopGuideText(
