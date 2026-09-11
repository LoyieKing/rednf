// 0x0867dba0：指定物品清理编排（deleteSpecificItem.md）：关私摊 → 五域清理 →
// 邮箱超时/匹配/无效物品移除（header 0x62 包）→ 无效物品邮件补偿 → redeem 清理。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0867dba0 | _ZN5CUser18deleteSpecificItemERKSt6vectorISt4pairIiiESaIS2_EERS4_
// 定义于 CDataManagerScripts.cpp（ORIG 0x867cf8d T）；deleteSpecificItem 邮箱段先用。
// b28d-PoolCUser 档①跨 TU 同步（2026-09-09）：定义端已 bool（CDataManagerScripts.cpp:869，
//   Main P292）；ORIG 本 TU 调用点 `call; test %al,%al; je`（0x0867d472 族）无 setne
//   ⇒ 本镜像声明同步 bool，否则 5 调用点卡 setne 归一段。
bool _checkTimeoutItem(const Inven_Item& item);
