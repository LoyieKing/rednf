// ORIG T _ZN16CItemMakingSkill23GetItemMakingSkillLevelEPK14CStackableItemP5CUser
// @0x08545cf4：GetSkillLevel(GetUpSkillType(item), user)。
int CItemMakingSkill::GetItemMakingSkillLevel(const CStackableItem* item, CUser* user)
{
    // P0：ORIG `call GetUpSkillType; mov %eax,-0xc` 具名局部槽；直传会丢 -0xc。
    ENUM_ITEM_MAKING_TYPE type =
        static_cast<ENUM_ITEM_MAKING_TYPE>(item->GetUpSkillType());
    return GetSkillLevel(type, user);
}