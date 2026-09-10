// [b28d-GrpD 判词·N] ORIG 0x08651322：残差全是**同址等价形式差** ——
//   ① ORIG `imul $0x3d; add $0xe0; add $0x16`（=0xf6）vs 我方折叠 `add $0xf6`；
//   ② ORIG `add $0xea,%edx; mov 0xe(%eax,%edx,8)` vs 我方 `shl $3; add $0x75e; add`
//      （同址：0xe+0xea*8 = 0x75e，副偏移 0x12 = 0xe+4、0x14 = 0xe+6）；
//   ③ 0x75e/0x762/0x764 与 ORIG 逐字节相等 ⇒ 字段偏移/极性/符号性全一致，无 bug。
void CUser::updateCharacViewVisibleData(const _Charac_info& info,
                                        struct CHARAC_LOGIN_DATA& data)
{
    memset(data.m_avatarSlots, 0, 0x60);
    if ((getCurCharacR() != 0 &&
         getCurCharacR()->m_characNo ==
             reinterpret_cast<const CharacListElemView&>(info).m_characNo)
            ? true : false)
    {
        data.m_renameOrViewState = getRequestRenameState();

        // 槽 0..8：avatar 装备槽（有物品且扩展信息存在时写索引/颜色对）。
        for (int i = 0; i < 9; ++i)
        {
            const AvatarSlotSrcView* slot =
                reinterpret_cast<const AvatarSlotSrcView*>(
                    i * 0x3d + 0xe0 + reinterpret_cast<const char*>(&info) +
                    0x16);
            if (slot->m_itemIndex != 0)
            {
                const void* expansion =
                    reinterpret_cast<CInventory*>(
                        reinterpret_cast<char*>(const_cast<_Charac_info*>(&info)) +
                        0xda)
                        ->GetAvatarItemMgrR()
                        ->GetExpansionInfo(slot->m_expansionKey);
                if (expansion != 0)
                {
                    if (reinterpret_cast<CInventory*>(
                            reinterpret_cast<char*>(
                                const_cast<_Charac_info*>(&info)) + 0xda)
                            ->GetClearAvatar(i) != 0)
                        data.m_avatarSlots[i].m_itemIndex =
                            reinterpret_cast<CInventory*>(
                                reinterpret_cast<char*>(
                                    const_cast<_Charac_info*>(&info)) + 0xda)
                                ->GetClearAvatar(i);
                    else
                        data.m_avatarSlots[i].m_itemIndex = slot->m_itemIndex;
                    data.m_avatarSlots[i].m_expansionA =
                        reinterpret_cast<const ExpansionPair*>(expansion)->m_a;
                    data.m_avatarSlots[i].m_expansionB =
                        reinterpret_cast<const ExpansionPair*>(expansion)->m_b;
                }
            }
        }

        // 槽 9..11：租赁位（任一租期类型命中则整槽清零，否则仅写索引）。
        for (int i = 9; i < 12; ++i)
        {
            if (checkRentedItem(
                    (unsigned int)reinterpret_cast<const CharacListElemView&>(info)
                        .m_characNo,
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex,
                    1) ||
                checkRentedItem(
                    (unsigned int)reinterpret_cast<const CharacListElemView&>(info)
                        .m_characNo,
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex,
                    2))
            {
                memset(&data.m_avatarSlots[i], 0, 8);
            }
            else
            {
                data.m_avatarSlots[i].m_itemIndex =
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex;
                data.m_avatarSlots[i].m_expansionA = 0;
                data.m_avatarSlots[i].m_expansionB = 0;
            }
        }
    }
    else
    {
        // 非当前角色：从可见缓存表（info + k*8 + 0x75e）12 槽直拷。
        for (int k = 0; k < 12; ++k)
        {
            data.m_avatarSlots[k].m_itemIndex =
                (reinterpret_cast<const VisibleCachePair8*>(
                     reinterpret_cast<const char*>(&info) + 0xe) + k + 0xea)
                    ->m_itemIndex;
            data.m_avatarSlots[k].m_expansionA =
                (reinterpret_cast<const VisibleCachePair8*>(
                     reinterpret_cast<const char*>(&info) + 0xe) + k + 0xea)
                    ->m_fieldA;
            data.m_avatarSlots[k].m_expansionB =
                (reinterpret_cast<const VisibleCachePair8*>(
                     reinterpret_cast<const char*>(&info) + 0xe) + k + 0xea)
                    ->m_fieldB;
        }
        data.m_renameOrViewState =
            reinterpret_cast<const CharacListElemView&>(info).m_viewState;
    }
