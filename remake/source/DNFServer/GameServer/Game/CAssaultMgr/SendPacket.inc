int CAssaultMgr::SendPacket(CUser* user, bool flag, PacketGuard* packet)
{
    // ORIG 0x82efb54：按用户所在袭击区域转发；flag=true 时限定队伍。
    // [b24F] DONE：返回类型 void→int + return 1（ORIG 尾部 mov $0x1,%eax）。
    // ORIG EH：catch(int)（landing 带类型过滤 + begin_catch 后 `mov (%eax),%eax`
    // 取值死槽，OnLeaveAssaultPlace/OnMoveArea 同款——此前 catch(...) 缺 6 条）。
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(
            (unsigned int)user->GetAssaultPlace());
        if (flag)
        {
            ENUM_ASSAULT_TEAM team;
            if (place->GetTeam(user, team))
                place->SendPacket(*packet, team);
        }
        else
        {
            place->SendPacket(*packet);
        }
    }
    // [R18exc 2026-09-13] ORIG LSDA 类型表 = _ZTI10ENUM_ERROR（旧 catch(int) 不匹配）。
    catch (ENUM_ERROR)
    {
    }
    return 1;
}