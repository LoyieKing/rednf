// ===========================================================================
// 装备部件套装（ORIG "../RDARScriptEtcParameter.cpp" 域）——本 TU 不再持有。
//
// 唯一定义归 InitScriptProducers_2.cpp（importEquipmentPartSetInfo 生产 TU，
// DataManagerInitDepends.h 权威声明指定）：STEquipmentPartSetData 类视图 +
// C1 0x0891b002 W / D1 0x08907348 T + g_equipmentPartSetMap_（B 0x094f7130）。
// 本文件此前与该 TU 三重重复（wave22 链接 multiple definition），已删除；
// 本 TU 无任何消费点，无需前向声明。
// ===========================================================================
// importPremiumServiceData —— ORIG T 0x08912e47
// （docs/class_func_reports/_lt_global_gt_/importPremiumServiceData.md）
// 真形（ORIG 0x08912e47 汇编+Ghidra C 双段转写，P163/P165/P177/P178 族）：
// 外层字符串匹配 "[premium service]"，内层 while(ScanType) 每轮穿
// static 三件套（__lookup map<char const*,int,less_TCHAR> + __init_lock
// SpinLock + __inited，guard 声明位于重派发标签后）；未建表持锁 id=0 进
// switch，自 case 0（与 0x3059 共 leaf，P163）起逐 case 构造
// value_type(键, nId 具名槽) + insert 后**贯穿**下一 leaf，公共尾
// leave+__inited=1 后 jmp guard 以同 token 重查派发；建表后
// c_str→find→!=end 三目单点写 id，switch(id) 派发：
//   [type]0x3059→+0x00、[icon index]0x305c→+0x10、[icon explain]0x305f→
//   ScanStr(+0x14)、[add equipment list]0x3062→循环 push_back(+0x18)、
//   [main premium image index]0x306e→+0x04/+0x08 双 ScanInt、
//   [additional premium image index]0x3072→+0x0c、
//   [add selectAble equipment list]0x3076→双 ScanInt + insert(make_pair(a,
//     make_pair(false,b))) 嵌套实参形（+0x24 map）、
//   [/premium service]0x3088→out.push_back(data)+data.clear()。
// loadRDARScriptFile 失败记日志（0x3044）返 0，成功返 1。
// 验收（§6.2）：gap 158→1；P167 调用点差集 64/64 全命中；残余 1 insn =
// 4.4 在 token== 匹配后发射 `jmp <内层检查>` 中继 vs 4.1 fall-through
// （跨版本 nop;jmp 岛 vs jump-threading，N 类豁免）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95/N7：ORIG 0x08912e47；
// char[4] framePad 对齐 token 槽（-0xfc，与 ORIG 同址）；残余 413 条 =
// ①data 槽深 4（我方 -0x154 vs ORIG -0x150，槽间距 0x58 vs 0x54：data 与 token
// 之间的编译器临时集合差，sizeof(PremiumServiceData)=0x3c 已 static_assert 锁定）；
// ②循环头 `jmp+nop` 与 XOR 形循环头位置差；③立即数压栈/callee-saved 角色差
// （4.1 vs 4.4，N7）不可翻。485=485 指令数全等。p167 CLEAR（64/64），语义闭合。
int importPremiumServiceData(std::vector<PremiumServiceData>& out)
