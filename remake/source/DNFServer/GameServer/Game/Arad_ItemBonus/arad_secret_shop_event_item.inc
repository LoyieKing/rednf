// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_secret_shop_event_item —— ORIG 0x08196ed7（报告全文转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: P0 — ORIG 无 rabbits 具名引用（每使用点重算
// `stackable->m_vec20`（AradItemInfo 已为基类，[R18rti B1]））；box/it 均在循环外默认构造（+0x15a/+0x165），
// 循环内 `box = *it;`；lastSlot 分支 ORIG `setne;test;je <mail>`（P225 形）。
void ARAD_ITEM::arad_secret_shop_event_item(CUser* user, unsigned long param,
