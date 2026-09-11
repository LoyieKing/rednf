// ============================================================================
// TowerOfDespairMgr::CheckEnterable —— ORIG T 0x086440ee（报告全文转写）：
// [b23S3C] N：帧重排+跨 call 寄存器缓存（N82/N18a 族），语义等价已核（详见 b22_s3_verdicts.tsv）
//   user 空 → 0x15；level < m_minLevel(+0x34e) → 0xe；
//   TOD_UserState(getCharacExpandData 0xd).getEnterCount()>0 时：
//     非 GM 且 !CheckDailyScheduleTime(env.m_fatigueTime(+0x37c),
//     lastClearTime, curSec) → 0xf5；
//     check_item_exist(requiredItem1.m_0)==-1 → 0xf5；
//     GetInvenSlot(1,slot).get_add_info()(+0x07) < requiredItem1.m_4 → 0x11；
//   party 空且未走上方分支 → 0x15；member_count>1 → 0xab；否则 0。
//   （env+0x37c = Server_Envir::m_fatigueTime，CEnvironment.h 权威布局。）
// ============================================================================
// [b28d-L8] N82 | 帧差 + 参数/字段装载序（ORIG 0x08xxxxxx）
int TowerOfDespairMgr::CheckEnterable(CUser* user, const CDungeon* dungeon)
{
    if (user == 0)
    {
        return 0x15;
    }

    if (user->get_charac_level() < static_cast<int>(m_minLevel))
    {
        return 0xe;
    }

    TOD_UserState* state = reinterpret_cast<TOD_UserState*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
    if (state->getEnterCount() > 0)
    {
        if (!user->isGMUser())
        {
            // ORIG：两值跨后续调用保活于 callee-saved（%esi/%ebx，无栈槽）⇒
            // 按实参右→左求值内联书写（P0/P52），勿落具名局部（旧形两槽
            // ⇒ 帧 0x60→0x78 且缺 push %esi/%ebx）。
            if (!CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                        state->getLastClearTime(),
                                        GlobalData::s_systemTime_.getCurSec()))
            {
                return 0xf5;
            }
        }

        // ORIG 无 inven 具名槽（两次独立 `getCurCharacInvenR()` 内联调用，
        // P0/P48）⇒ 内联成员链。ORIG 也无 Inven_Item 具名对象（直接对
        // GetInvenSlot 的匿名 sret 结果读 +0x7 = m_addInfo2）。
        int slot = user->getCurCharacInvenR()->check_item_exist(
            dungeon->m_entryRequiredItem.m_itemIdx);
        if (slot == -1)
        {
            return 0xf5;
        }

        // [M3S5 2026-09-11 · P0 具名对象推翻旧判词] ORIG 0x08644214 直接
        //   `mov -0x4a(%ebp),%edx`（sret 目标 -0x51 的对象字段 +0x7）后
        //   `cmp %eax,%edx; jge +0x13d` —— **直跳**（无 setcc 物化）。
        //   4.4 对「调用返回的**临时对象**成员」参与比较恒物化 `setl;test;je`
        //   （/tmp/p_m3s5.cc 探针 f2：setl;test;je），而对**具名对象**字段比较
        //   直出 `jge`（探针 f8：call;mov -0x14(%ebp),%edx;cmp;jge，无额外槽）。
        //   ⇒ 原码是具名 Inven_Item 接收 GetInvenSlot 结果（sret 直写该对象，
        //   故 ORIG 无拷贝构造调用）。
        Inven_Item slotItem = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        if (slotItem.m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
        {
            return 0x11;
        }
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    // [M3S5 2026-09-11 · P222/P59 块序·落地] ORIG 0x0864423d `je 0864425b`（party==0
    //   → `mov $0x15` 块）、0x08644262 `mov $0x0` 是**最后一块且 fall-through 到出口**；
    //   早退形（`if (party == 0) return 0x15;`）生成相反块序 ⇒ 改「if (party != 0)
    //   {…} else { return 0x15; } return 0;」后 CheckEnterable **DIFF → AE**
    //   （112 = 112；TU 四数 ae 4→5、diff 3→2、near 1→0）。
    if (party != 0)
    {
        if (party->get_member_count() > 1)
        {
            return 0xab;
        }
    }
    else
    {
        return 0x15;
    }
    return 0;
}