int DisPatcher_DeleteItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stDeleteItemParam& out = static_cast<stDeleteItemParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0xb8e, __PRETTY_FUNCTION__, out.m_4, user->get_acc_id());
    }

    if (exercise_mode(user, msg, param))
    {
        return 0;
    }

    // [G5-ProcB] ORIG 0x081c1e94 `mov 0x10(%ebp),%eax; mov %eax,-0x34(%ebp)`：
    // msg 视图具名局部在 exercise_mode 之后才初始化（非函数头）。函数头声明会多
    // 一条 store 并把 -0x3c 槽基整体下移 4（我方帧 0x110 vs ORIG 0x100）。
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);

    INVEN_TYPE invenKind = (INVEN_TYPE)6;
    if ((signed char)req.m_itemSpace == 1)
    {
        invenKind = (INVEN_TYPE)2;
    }
    else if ((signed char)req.m_itemSpace == 3)
    {
        invenKind = (INVEN_TYPE)0;
    }
    if (invenKind == (INVEN_TYPE)2)
    {
        out.m_4 = 0x17;   // 装备栏禁止该删除类型
        out.m_8 = req.m_itemSpace;
        return 0;
    }

    bool needRestricted = false;
    for (int i = 0; i < (int)(signed char)req.m_count; ++i)
    {
        if (req.m_opType[i] <= 1)
        {
            needRestricted = true;
            break;
        }
    }
    if (needRestricted)
    {
        out.m_4 = restricted_check(user, msg);
        if (out.m_4 != 0)
        {
            out.m_8 = req.m_itemSpace;
            return 0;
        }
    }

    const int count = (int)(signed char)req.m_count;
    for (int i = 0; i < count; ++i)
    {
        out.m_4 = condition_check(user, msg, invenKind, i);
        if (out.m_4 != 0)
        {
            out.m_8 = req.m_itemSpace;
            return 0;
        }

        int delKind = 3;
        if (req.m_opType[i] == 1)
        {
            delKind = 5;
            if (user->get_state() != 3)
            {
                delKind = 6;
            }
        }

        const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_itemSpace);
        // ORIG +0x25e~+0x327：GetInvenSlot 先 sret 到匿名槽(-0xa9)，再逐字段
        // 拷入具名局部 item(-0xe6)；4.4 直构缺 17 条拷贝 ⇒ 两段式复现。
        Inven_Item slotItem = ((CInventory*)user->getCurCharacInvenR())
                                  ->GetInvenSlot(invenType, (int)req.m_position[i]);
        Inven_Item item = slotItem;

        if (delKind == 3)
        {
            Global::CBossTowerFunc::UseItem(user, item);   // BLOCKED 符号见文档
        }
        if (item.m_addInfo == 0xbdd)
        {
            ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
                user, (HeroMissionCondition::MissionNo::T)3,
                (unsigned int)req.m_countArr[i]);
        }

        if (user->get_state() == 3 && delKind == 5 && req.m_countArr[i] == 0)
        {
            const int secu = GlobalData::s_pSecuProtectionField->Check(
                user, (SECURITY_PROTCTION)0x46);
            if (secu != 0)
            {
                out.m_4 = secu;
                return 0;
            }
        }

        // ORIG ILP32 ABI（objdump 逐槽核对）：arg3(m)=itemIdx(@0x19f)、
        // arg4(int)=count(@0x32f)、arg5(enum)=delKind；callee 以 ebp+0x14=itemIdx、
        // ebp+0x18=count 消费。
        const int deleted =
            user->delete_item((char)req.m_itemSpace, (int)req.m_position[i],
                              (unsigned long)req.m_itemIdx[i], req.m_countArr[i],
                              (eItemDelReason)delKind);

        if (deleted > 0)
        {
            // 中途失败：先向客户端回滚已删条目，再发失败确认
            cMyTrace trace(__PRETTY_FUNCTION__, 0xc2a, 5);
            trace("delete_item failed in the middle of deleting items. %d/%d",
                  i, (int)(signed char)req.m_count);

            PacketGuard ack;
            if (i > 1)
            {
                PacketGuard rollback;
                rollback.put_header(1, 0x12);
                rollback.put_byte(1);
                rollback.put_byte((int)(char)req.m_itemSpace);
                rollback.put_byte(i - 1);
                for (int k = 0; k < i - 1; ++k)
                {
                    rollback.put_short((int)req.m_position[k]);
                    rollback.put_int(req.m_countArr[k]);
                    rollback.put_short((int)req.m_opType[k]);
                }
                rollback.finalize(true);
                user->Send(rollback);
            }
            ack.put_header(1, 0x12);
            ack.put_byte(0);
            ack.put_byte(deleted);
            ack.put_byte((int)(char)req.m_itemSpace);
            ack.finalize(true);
            user->Send(ack);
            return LineFunc(0xc48, __PRETTY_FUNCTION__, 0, user->get_acc_id());
        }

        if (!item.isEmpty() && delKind == 5)
        {
            CItem* script = G_CDataManager()->find_item(item.m_addInfo);
            if (script != 0)
            {
                if (script->is_stackable())
                {
                    item.m_addInfo2 = req.m_countArr[i];   // Inven_Item+0x07 计数字段
                }
                user->Add_RedeemInfo(item, 0, false);
            }
        }

        CItem* sellScript = G_CDataManager()->find_item(req.m_itemIdx[i]);
        if (sellScript != 0)
        {
            int price = 0;
            G_Store()->GetSellItemPrice(item, sellScript, (short)req.m_countArr[i],
                                        false, price);
            if (user->get_state() != 8)
            {
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)price);
            }
            if (user->get_state() == 5)
            {
                if (req.m_opType[i] == 2)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        sellScript, req.m_countArr[i], user, (CUBE_STATISTIC_FIELD)0x67);
                }
                else if (req.m_opType[i] == 3)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        sellScript, req.m_countArr[i], user, (CUBE_STATISTIC_FIELD)0x68);
                }
                else if (req.m_opType[i] == 4)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        sellScript, req.m_countArr[i], user, (CUBE_STATISTIC_FIELD)0x69);
                }
            }
        }
    }

    out.m_8 = req.m_itemSpace;
    out.m_9 = (unsigned char)req.m_count;
    for (int i = 0; i < count; ++i)
    {
        out.m_slotsD2[i] = req.m_position[i];
        out.m_counts19c[i] = req.m_countArr[i];
        out.m_slotsA[i] = req.m_opType[i];
    }
    return 0;
