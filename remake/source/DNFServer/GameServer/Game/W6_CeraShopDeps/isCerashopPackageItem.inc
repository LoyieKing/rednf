// ---------------------------------------------------------------------------
// ORIG W 0x08179fb4 _ZNK5CItem21isCerashopPackageItemEv：逐指令转写。
// 【翻案 b25c】旧 N 标注（setcc-fold）过时：单 return 短路链（P45）探针
// /tmp/probe_short.cpp q2 定论 4.4 与 ORIG 逐字同构（cmp;je 共享真岛 +
// cmp;jne + mov$1/mov$0），if-return 链才折 setcc。26/26 全等。
// 【b28c-36 复验】fresh ndiff=16 / NEAR（26=26 指令数）：残差 2 处全为
// **N7 间接 call 目标寄存器**——ORIG `mov (%eax),%edx; mov this,%eax; call *%edx`
// （目标 %edx/this %eax），4.4.7 恒 `mov (%eax),%eax; mov this,%edx; call *%eax`
// （目标 %eax/this %edx）。P230 补：4.4.6 后端间接 call 目标用 %edx（与 ORIG 同形），
// 本波禁自切路由，待 Main 三态后端 sweep 后按 N7 敏感族复测。源码形无解。
// 经 vtable 槽 +0x0c 虚方法（子类型索引，与 CI_SubType 先例同型）连续两次
// 调用，返回值 ==0xe 或 ==0xf 判定礼包类目 → true，否则 false。
// ---------------------------------------------------------------------------
bool CItem::isCerashopPackageItem() const
{
    // vtable +0x0c = GetItemType()（_GetItemType 批次实证 make_item@+0x08 后继槽）。
    return GetItemType() == 0x0e || GetItemType() == 0x0f;
}