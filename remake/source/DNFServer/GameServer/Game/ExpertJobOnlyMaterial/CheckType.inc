// ORIG _ZN10expert_job17ExpertJobMaterial9CheckTypeEbi @0x0849bfd0：
// 非 stackable 材料（equipment 附魔卷轴类）只要求 IsEnchantalbe()
// （vtable slot7 @+0x1c）；stackable 材料要求 GetItemType()==itemType。
int ExpertJobMaterial::CheckType(bool checkStackable, int itemType)
{
    if (Empty())
        return 0x11;

    // b28d-36（修法，P0）：ORIG 0x0849bfd0 每次访问均重发 `mov 0x8(%ebp),%eax;
    // mov 0x4(%eax),%eax` 直传 is_stackable/GetItemType/IsEnchantalbe（无 item
    // 具名槽往返）⇒ 删局部、逐点内联 reinterpret_cast<CItem*>(m_item)。
    if (reinterpret_cast<CItem*>(m_item)->is_stackable() != checkStackable)
        return 0x17;

    if (checkStackable)
    {
        if (reinterpret_cast<CItem*>(m_item)->GetItemType() != itemType)
            return 0x17;
    }
    else if (!reinterpret_cast<CItem*>(m_item)->IsEnchantalbe())
    {
        return 0x17;
    }
    return 0;
}