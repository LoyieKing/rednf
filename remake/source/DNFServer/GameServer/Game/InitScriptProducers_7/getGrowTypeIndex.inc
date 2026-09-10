int getGrowTypeIndex(const char* name);                                // 0x088be63d（InitScripts_6）

// （原 namespace 内 kInGameEventTokenIds/lookupTokenId 发明查找表已删：
//  importInGameEventInfoList 按 ORIG 函数级静态 __lookup 真形重写，见该函数。）

// ============================================================================
// importMercenarySystemEtcScript — ORIG 0x089f6c50（init.md +0x6b19，
// "Etc/mercenary.etc" → m_mercenarySystem）。返回 0=成功，否则错误码
// （9 装载失败 / 0xc 空指针 / 0x29..0xd1 各段 Scan 失败，见 case 注释）。
// ORIG 双相派发（函数级静态 __lookup/__init_lock/__inited 三件套，case 贯穿
// 注册链 0x25/0x32/0x46/0x57/0x69/0x7f）已按汇编逐段还原；错误路径累加/
// 检查次序（+0x96d 先 Σ groupId 后查 ok、+0xa61 先 Σ count）与
// [mob/item index] make_pair 实参序（+0xa72：first=count 槽 -0x144）逐条对齐。
// 残余 N 类豁免（orig=ours=838，助记符序列仅剩以下跨编译器结构差）：
//  - +0x3ce `mov -0x13c,%edx` 值直达 vs 4.4 `mov…,%eax; mov %eax,%edx` 两段
//    （N18a regmove 不可逆，+0x555 同理，2 处）；
//  - +0x545 16 位 jle vs 4.4 同操作数折为 jbe（setcc-fold 比较方向）；
//  - +0xa61/+0xb51 累加 4.1 `mov mem,%eax; mov %eax,%edx` 两段物化，4.4 恒折
//    单步（N18a 反向，2 处）；
//  - +0xbe2/+0xcd6 及组错误出口：4.1 将 group/area 析构 merge 为 esi 标志共享
//    epilogue（+0xc04/+0xcf5），4.4 生成中继 jmp（EH 双份 dtor epilogue vs
//    merge 跨版本差异）。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N18a/N7] ORIG 0x089f6c50
//  p167 102/102 CLEAR；orig/ours=838/838。残余 65 行归一化差异全部为 N18a
//  regmove（4.1 `mov mem,%edx` vs 4.4 `mov mem,%eax; mov %eax,%edx`，5 处）
//  + EH 尾岛 2 处（跨版本，语义完成）。
