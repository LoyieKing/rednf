// ============================================================================
// GmReduceEndurance —— ORIG T 0x081911b6 _ZN...17GmReduceEnduranceEP5CUserss
// ============================================================================
void AvatarRechargeServer::GmReduceEndurance(CUser* user, short slot,
                                             short delta)
{
    if (slot < 0 || slot > 9)
        return;
    // [b23S3C] P48 去缓存：ORIG getCurCharacInvenW 结果 eax 直落实参槽
    // （0x32 mov %eax,(%esp); call GetInvenRef），无 invenW 槽 ⇒ 右值直调。
    Inven_Item* item = user->getCurCharacInvenW()->GetInvenRef(0, (int)slot);
    if (item == 0 || item->isEmpty())
        return;

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                item->m_addInfo2);
    // [b23S3C] ORIG 无 sockets/limit 槽（getSocketCount→%ebx 跨 call 缓存 +
    // getLimit 返回 eax 直 imul），仅乘积落 -0x14 ⇒ 合并为单局部；调用顺序
    // getSocketCount 先、Singleton::Get 后（左→右求值同 ORIG）。
    const int maxEndurance =
        getSocketCount(socketData) *
        ARAD::Singleton<AvatarRechargeServer>::Get()
            ->getAvatarRecharageLimit();

    int newEndur = (int)item->m_fieldSplit.m_avatarEndur + (int)delta;
    if (newEndur < 0)
        newEndur = 0;
    else if (newEndur > maxEndurance)
        newEndur = maxEndurance;
    item->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;

    // [b23S3C] ORIG 0x111 直压 3 参（target/user/slot）无 Get() call ⇒ static 调。
    UpdateAvatarEndurance((CUser::eSendTarget)1, user, slot);
    SaveAvatarEnduranceUnit(item->m_addInfo2,
                            (int)(unsigned short)item->m_fieldSplit.m_avatarEndur);

    // [b23S3C] ORIG -0xc 槽存串指针两条（movl $str; mov →0x4 esp）⇒ char*
    // 具名局部（P0 槽往返），非 char 数组逐字节初始化。
    char* message = (char*)"succ avatar Endurance change";
    ARAD::notifySystemMessage(user, message);
}