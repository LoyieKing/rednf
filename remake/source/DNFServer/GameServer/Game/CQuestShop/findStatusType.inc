int CQuestShop::findStatusType(unsigned char status)
{
    // ORIG 0x85eee6a：map 拷贝 + 非 const find/end；value+0x1c = second.m_field18。
    // 经镜像顶层 stQuestShopStatus map 拷贝（与 ORIG 顶层模板同名）。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0);
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(static_cast<int>(status));     // b28c-L10：同上（ORIG 0x85eee6a 槽序）
    if (it != table.end())                        // ORIG ne 比较
        return it->second.m_field18;              // mov 0x1c(%eax)（value+0x1c）
    return 0x4d;                                  // +0x85
