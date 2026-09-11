// ===========================================================================
// importCancelSkillIndices —— ORIG T 0x08914dd1
// （docs/class_func_reports/_lt_global_gt_/importCancelSkillIndices.md）
// 真形（ORIG 0x08914dd1，同 importPremiumServiceData 双相派发族 P163/P177）：
// 外层 for(;;)+ScanType；每轮穿 static 三件套（__lookup map<char const*,int,
// less_TCHAR> + __init_lock SpinLock + __inited，声明位于 redo_lookup 标签后）；
// 未建表 enter 双检 id=0 进 switch（case 0 与 0x3384 共 leaf），自
// "[cancel skill]"0x3384 → "[/cancel skill]"0x3388 → "[character job]"0x338c →
// "[/character job]"0x33aa 逐 case 构造 value_type(键,字面量) + insert 后
// **贯穿**下一 leaf，公共尾 leave+__inited=1 重查派发。建表后仅 0x338c 有
// parse：jobName ScanStr 失败 return 0；key.first=getCharacterJob(c_str)；
// growName ScanStr 失败 return 0；key.second=getGrowTypeIndex；ScanInt 循环
// 进 skills，失败轮 out.insert(value_type(key,skills)) 直构形（无 make_pair
// 转换临时，OnlyOrigBatch14 口径）+ key=(0xb,-1) 重置 + skills.clear()。
// "[cancel skill]"/"[/cancel skill]"/"[/character job]" 命中均无动作（仅登记）。
// loadRDARScriptFile 失败记日志（0x3372）返 0，成功返 1。
// 验收：gap 52→1（413/412）；P167 差集 58/58 归零；残余=槽号平移+ctor/dtor
// 块序（N82/EH-merge 族）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95/N7：ORIG 0x08914dd1；
// char[4] framePad 使槽基精确对齐 ORIG（key -0xb4/-0xb0、skills -0xc8）；
// 残余 338 条 = ①循环头 ORIG 条件置顶直落 vs 我方前置 `jmp+nop`（while 形实测
// 反劣 345 ⇒ 保留 for(;;)）；②立即数压栈与 callee-saved 角色差（4.1 vs 4.4，N7）
// 及 token 槽内偏移。p167 CLEAR（58/58），语义闭合。
int importCancelSkillIndices(
