// [b28d-lane2] verdict=DIFF ndiff=75（ORIG 0x084cf6ec）：槽序/帧差(N82/P212/P221)+混合残差；gap+8。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=72（ORIG 0x84cf6ec）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 8→4（84 vs 80，TU 121/1330/5/134 不回退）：真修——
//   删 key/itemIdx/charId/mgrR/mgrW 具名局部改链式内联（P89/P183/P329），pPkt 声明移到 get_state 检查后（P0 声明位置）。
//   残余=N18a×4（ORIG `mov mem,%eax; mov %eax,%ebx/%edi/%esi` 两段升格，4.4 恒折单段 `mov mem,%reg`）；语句表达式/字段类型旋钮实测无效。
int Inter_AvatarChangeStat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    Inter_AvatarChangeStatPktView* pPkt = reinterpret_cast<Inter_AvatarChangeStatPktView*>(data);
    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetExpireDate(pPkt->m_0) == -1)
    {
        user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(
            pPkt->m_0,
            pPkt->m_e,
            pPkt->m_a,
            reinterpret_cast<char*>(pPkt) + 0x12,
            *reinterpret_cast<const stAvatarEmblemInfo_t*>(reinterpret_cast<char*>(pPkt) + 0x2f),
            *reinterpret_cast<const stAvatarExpansionInfo_t*>(reinterpret_cast<char*>(pPkt) + 0x4d),
            false);
    }
    user->SendUpdateItemList(
        (CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
        (int)pPkt->m_8 - 10);
    return 0;
}