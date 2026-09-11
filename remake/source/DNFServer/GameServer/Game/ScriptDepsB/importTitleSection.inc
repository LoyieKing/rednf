// ============================================================================
// importTitleSection —— ORIG 0x08aa8d65
// 报告 _lt_global_gt_/importTitleSection.md。三个函数级静态（ORIG 经
// __cxa_guard 初始化）：__lookup 键表 / __init_lock 自旋锁 / __inited 标志。
// 首个令牌触发级联登记（[/section]0x155→[/reward type]0x158→[section name]
// 0x15c→[section icon]0x15f→[frame type]0x166→[lock message]0x169→[name]
// 0x16e→[image path]0x172→[effect path]0x179），置位 __inited 后以当前令牌
// 重新查表派发。查表命中标签则解析对应字段；未命中即奖励行 → importRewards。
// 返回：ScanType 耗尽→true；[/reward type]→true；[/section]→false。
// ============================================================================
// [b28d-solo-ScriptDepsB: switch 驱动级联注册链（P177 static 三件套落循环体内、
//  P163 case 0 撑宽、P192/P315 value_type 直构、P96/P270 const_iterator）；
//  查表段单点共享。ORIG 0x08aa8d65]
bool importTitleSection(stTitleSection* section, int idx)
