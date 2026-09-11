// ============================================================================
// Arad_GiftItem_Set::open（ORIG 0x081a2f1e，_ZN17Arad_GiftItem_Set4openEP17STStackableScript）
// 语义：解析 ST_EVENT_GIFT_ITEM* 脚本，把可用的"礼物槽位"登记进 this+0x04 的
// map<long long, Data>。key = ((long long)kind << 32) | ((uint)field5c + (slot << 16))。
// 返回 0 表示成功；0x122 表示 key 已存在（重复登记）；脚本未启用/未知类型返回 0。
// 依据 docs/class_func_reports/Arad_GiftItem_Set/open.md 逐分支恢复。
// ============================================================================
// ============================================================================
// Arad_GiftItem_Set::Arad_GiftItem_Set（ORIG 0x081a2a68，247 insn）
// 语义：填 +0x1c 的 map<string,int>：ST_EVENT_GIFT_ITEM{,2..6} → kind 1..6。
//（依据 ctor 报告逐条 make_pair→insert 转写。）
// ============================================================================
// ORIG T 0x081a2a68（247 insn）：基类 Arad_ScriptLoader + vptr + 两 map 构造后，
// 6 条 "ST_EVENT_GIFT_ITEM"[2..6] → kind 1..6 登记（ctor 报告逐条实证）。
Arad_GiftItem_Set::Arad_GiftItem_Set()
    : m_map(), m_kindMap()
{
    // ORIG 形态：make_pair 直接收字符串字面量 + ST_ITEM_KIND 枚举（pair 转换构造
    // 生成 _ZSt9make_pairIRA19_KcN17Arad_GiftItem_Set12ST_ITEM_KINDEE… 与 ORIG 一致）。
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM", ST_ITEM_KIND_1));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM2", ST_ITEM_KIND_2));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM3", ST_ITEM_KIND_3));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM4", ST_ITEM_KIND_4));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM5", ST_ITEM_KIND_5));
    m_kindMap.insert(std::make_pair("ST_EVENT_GIFT_ITEM6", ST_ITEM_KIND_6));
}