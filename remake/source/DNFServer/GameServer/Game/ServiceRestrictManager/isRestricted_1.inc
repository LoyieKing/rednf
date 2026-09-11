// ORIG 0x0816e75e（WarField 用）：map 无该类别→true；否则策略虚调用。
bool ServiceRestrictManager::isRestricted(RestrictCategory::Enum category, int v)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    // [b28c-L15 P220] ORIG 0x0816e7a9：`test %al; je 尾 mov$1` + fall-through 虚调用
    //   ⇒ 正极性迭代器比较（同 isApply 形），false 侧归共享尾。
    if (it != m_policyMap.end())
    {
        // ORIG 0x0816e7ca：虚调用 vtable+0x14（槽5）= isApply(int)。
        return it->second->isApply(v);
    }
    return true;
}