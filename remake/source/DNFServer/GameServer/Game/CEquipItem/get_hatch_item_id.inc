// ORIG _ZNK10CEquipItem17get_hatch_item_idEv W 0x0833eeda size 0xe（nm -S + objdump
// 实证：push ebp/mov esp,ebp/mov 0x8(%ebp),%eax/mov 0x164(%eax),%eax/pop ebp/ret），
// 即 m_randomTable.m_field1c（蛋孵化物品 id，set_item case 1 由脚本 m_764 写入）。
// 本 TU 提供该 W 符号的强定义；调用方 user_creature::CEgg::Hatch（0x833912a）
// 按 ORIG 形态外联调用它（非内联）。
int CEquipItem::get_hatch_item_id() const
{
    return m_randomTable.m_field1c;
}