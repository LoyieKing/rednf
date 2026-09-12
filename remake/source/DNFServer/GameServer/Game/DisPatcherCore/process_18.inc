// ORIG 0x081c1dda T process（process.md 全文转写）。
// §6.2 巨件核销：P167 61/61 [0缺0多]；gap 31→1；残差归因 N82/P112。
// 真修 1 条：GetInvenSlot 返回 4.4 直构缺 ORIG +0x25e~+0x327 的 17 条逐字段
// [b28d-29] N82/P211：帧 0x100 vs 0x110 + 参数副本槽 + store 装载序；bool 条件 4.4 test 直判 vs 4.1 cmpb，语义集合全等  (ORIG 0x081c1dda insn 651->653)
// 拷贝（sret 匿名槽→具名局部）→ 改两段式（slotItem 收 sret + 拷贝入 item）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 400（ORIG 651 / ours 653），残差=P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [H5DisPatcher 核账] 2026-09-11 实测 ours 653→654（+1）劣化 ⇒ 已回滚留 N。
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

    // [P5-DPC 2026-09-11] P183：ORIG 0x081c1e7x 传的是**已缓存**的 param 指针槽
    // （`mov -0x3c(%ebp),%eax; mov %eax,0xc(%esp)`，-0x3c = 头部 store 的
    // stDeleteItemParam 引用槽），我方旧形直接 `mov 0x14(%ebp),%eax` 重取实参
    // （diff `replace orig[39:40]`）。改传别名 `out`（同对象，零语义差）。
    if (exercise_mode(user, msg, out))
    {
        return 0;
    }

    // [G5-ProcB] ORIG 0x081c1e94 `mov 0x10(%ebp),%eax; mov %eax,-0x34(%ebp)`：
    // msg 视图具名局部在 exercise_mode 之后才初始化（非函数头）。函数头声明会多
    // 一条 store 并把 -0x3c 槽基整体下移 4（我方帧 0x110 vs ORIG 0x100）。
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);

    INVEN_TYPE invenKind = (INVEN_TYPE)6;
    // [P5-DPC 2026-09-11] P251/P261 真 switch 轴：ORIG 0x081c1e97..0x081c1ec4 为 -O0
    // switch 决策树（`movl $0x6,-0x50`；`mov -0x34,%eax; movzbl 0xd(%eax),%eax;
    // movsbl %al,%eax`；`cmp $0x1,%eax; je L1` / `cmp $0x3,%eax; je L3` / `jmp Lend`，
    // 各 case 落 `movl $K,-0x50; jmp Lend`），我方旧 if/else-if 令 4.4 出
    // `cmp $0x1,%al; jne` 字节比较（diff `replace orig[58:63]`）⇒ 改真 switch。
    switch (static_cast<int>(static_cast<signed char>(req.m_itemSpace)))
    {
    case 1:
        invenKind = (INVEN_TYPE)2;
        break;
    case 3:
        invenKind = (INVEN_TYPE)0;
        break;
    default:
        break;
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

    for (int i = 0; i < (int)(signed char)req.m_count; ++i)
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

        // [V331b10 核验修复 2026-09-12] ORIG 0x081c217d `add $0x64,%edx` 直证：本条件
        // 读的是 m_itemIdx[i]（(i+100)*4+0xf = +0x19f），非 m_countArr[i]（(i+200)*4+0xf
        // = +0x32f）。同函数 addInfo 段与 delete_item arg4 用 +0xc8（=countArr）双方一致；
        // ORIG 本处用 +0x64 而我方曾写 +0xc8 ⇒ 保护分支进入条件错。
        if (user->get_state() == 3 && delKind == 5 && req.m_itemIdx[i] == 0)
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
            // [J5-ProcD 2026-09-11 ndiff 698→692 落地] P328⑩：ORIG 段求值序直证——先算实参
            // `mov -0x34(%ebp),%eax; movzbl 0xe(%eax),%eax; movsbl %al,%ebx`
            // （(int)(signed char)req.m_count 入 callee-saved 保活），**再**
            // `movl $0x5,0xc(%esp); movl $0xc2a,0x8(%esp); lea -0x4c(%ebp),%eax;
            // call cMyTrace::cMyTrace`，最后 `mov %ebx,0xc(%esp)` 入栈
            // ⇒ 原码是**单语句链式** `cMyTrace(...)(...)`（旧文两语句使 ctor 先于实参
            // 求值，实参退回 eax/具名槽）。父泳道核账：指令数不变但 ndiff **698→692**
            // （net −6）⇒ 保留本形。改前形：`cMyTrace trace(…); trace(…);`
            // 改后形：`cMyTrace(…)(…);`。
            cMyTrace(__PRETTY_FUNCTION__, 0xc2a, 5)(
                "delete_item failed in the middle of deleting items. %d/%d",
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
    // [Q5BigTURest.DisPatcherCore 2026-09-11 比较方向轴·落地] 本函数三处计数循环的
    // ORIG setcc 均为 `setg`（0x081c1f22 / 0x081c26ec / 0x081c277a：`movzbl 0xe(%eax);
    // movsbl %al,%eax; cmp <i槽>,%eax; setg`，LHS=每次重读的 msg+0xe、RHS=i）。我方原
    // `const int count` + `i < count` 令 4.4 产 `mov i,%eax; cmp count,%eax; setl`
    // （setcc2 直证 ours setl×2 vs ORIG setg×4）。删 count 具名、条件改为重读
    // `(int)(signed char)req.m_count` 后：ours setl 2→0、setg 2→4（= ORIG），
    // 本件 unified-diff 行数 204/202 → 123/121（verdict 仍 DIFF：残差=槽平移 4B 与
    // N18a store 序；TU 四数零劣化）。
    for (int i = 0; i < (int)(signed char)req.m_count; ++i)
    {
        out.m_slotsD2[i] = req.m_position[i];
        out.m_counts19c[i] = req.m_countArr[i];
        out.m_slotsA[i] = req.m_opType[i];
    }
    return 0;
}
