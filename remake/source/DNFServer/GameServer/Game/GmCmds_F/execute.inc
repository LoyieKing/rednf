// ORIG execute 0x084a97f4：按角色名查目标用户，存在则以 GM 发起者名义请求
// 拆解（DisPatcher_DisJointItem::_disjoint(user, 9, 0, 0x1c, target, 0xffff)；
// 9=请求来源 GM（推断），0x1c=CMD 包类型（对应 dispatchers[28]），
// 0xffff=目标区域通配（推断））。
void CRequestDisjointCmd::execute()
{
    register char const* name = m_str0.c_str();   // ORIG ebx 缓存跨 G_GameWorld
    CUser* target = G_GameWorld()->find_user_by_charac_name(name);
    if (target != 0)
    {
        int type = 9;
        DisPatcher_DisJointItem::_disjoint(GetUser(), type, 0,
                                           static_cast<ENUM_CMDPACKET>(0x1c),
                                           target, 0xffff);
    }
    else
    {
        return;
    }
}