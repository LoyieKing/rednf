// ==== Dispatcher_Skill_Command_Customizing::process (ORIG 0x081d2b42) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d2b42 orig/ours=174/161 ndiff-blocks=42
//   帧差 sub $50 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
int Dispatcher_Skill_Command_Customizing::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [DispLaneB 2026-09-11] ORIG 0x081d2b4a 入口序 = `mov 0x14(%ebp),%eax; mov %eax,-0x20`
    //   （param 引用先声明、占更深槽）→ `mov 0x10(%ebp),%eax; mov %eax,-0x1c`（msg 指针）
    //   ⇒ 交换声明序（我方原 msg 先声明占 -0x20，与 ORIG 相反）。
    stSkillCommandParam& res = static_cast<stSkillCommandParam&>(param);
    MSG_SKILL_COMMAND& req = *reinterpret_cast<MSG_SKILL_COMMAND*>(&msg);

    const int commandCount = (int)(char)req.m_commandCount;
    if (commandCount > 0x31)
    {
        // ORIG:count>49 → err=0xfc 后直接返回(不经错误日志/default 分支)
        res.m_4 = 0xfc;
        return 0;
    }

    for (int i = 0; i < commandCount; ++i)
    {
        // STSkillCommandData ctor ORIG W 0x0822db66(CUser.h 内嵌定义已一致)
        STSkillCommandData data;
        data.m_command = req.m_entries[i].m_command;
        data.m_count   = req.m_entries[i].m_count;

        // [DispA6 P0] ORIG 0x081d2bb8 `call get_charac_job; mov %eax,%ebx`、
        //   0x081d2bc5 `call G_CDataManager; mov %eax,(%esp)` —— job/dm 结果均
        //   **直传**无具名槽（旧码 -0x1c/-0x18 两槽为发明）；ORIG 走 const 重载
        //   _ZNK12CDataManager10find_skillEii @0x0835fda2（经非 const 指针调用）。
        CSkill* skill = G_CDataManager()->find_skill(
            user->get_charac_job(), (int)(short)data.m_command);
        if (skill == 0)
        {
            res.m_4 = 0xfc;
            break;
        }
        if (skill->isCommandCustomizing() != 1)   // CSkill::isCommandCustomizing W 0x0822b5d2
        {
            res.m_4 = 0xf9;
            break;
        }

        // [DispA6 P0] ORIG 无 keyCount 具名槽：0x081d2c13 `movzbl -0x2a(%ebp),%eax;
        //   cmp $0x5,%al` 每使用点从 data.m_count 重读（-0x2a = data+2）。
        if ((char)data.m_count > 5 || (char)data.m_count < 1)
        {
            res.m_4 = 0xfb;
            break;
        }

        // [DNF-FIX-SKILLCMD 2026-10-07] ORIG 0x081d2b89 `lea -0x2c(%ebp)` 构造 data，
        //   0x081d2b9f `mov %ax,-0x2c`(m_command)、0x081d2bae `mov %al,-0x2a`(m_count)、
        //   0x081d2c55 `mov %dl,-0x29(%ebp,%eax,1)` ⇒ -0x29 = data+3 = data.m_keyBytes[j]
        //   （非独立 keys[] 局部）。原译把键写进另一个局部数组，data 的 5 个键恒 0 ⇒
        //   customizingSkillCommand 存入 map / SaveSkill 落库 skill_command 的键全 0
        //   （DB 实测 0x6000004c：cmd=0x4c,count=3,keys=0）⇒ 重进频道后客户端回落默认搓招。
        // [DispA6 P0] ORIG 无 keyRangeError 具名槽（该标记从未被读取）：
        //   0x081d2c71 置 0xfa 后直接 `jmp 0x081d2c92`（末位键检查段）。
        for (int j = 0; j < (int)(char)data.m_count; ++j)
        {
            data.m_keyBytes[j] = req.m_entries[i].m_keys[j];
            if ((char)data.m_keyBytes[j] < 0 || (char)data.m_keyBytes[j] > 8)
            {
                res.m_4 = 0xfa;
                break;   // ORIG:跳出内层后仍无条件执行末位键检查序列
            }
        }

        // 末位键检查:keys[(char)m_count-1] ∉ {0,1,2,3,7} 才放行
        //   (ORIG 0x081d2c92..0x081d2cfd 每比较点重算 `movzbl -0x2a(%ebp),%eax;
        //    movsbl %al,%eax; sub $0x1,%eax; movzbl -0x29(%ebp,%eax,1),%eax`
        //    ⇒ 无 lastKey 具名槽，P0/P48)
        if ((char)data.m_keyBytes[(int)(char)data.m_count - 1] == 0 ||
            (char)data.m_keyBytes[(int)(char)data.m_count - 1] == 1 ||
            (char)data.m_keyBytes[(int)(char)data.m_count - 1] == 2 ||
            (char)data.m_keyBytes[(int)(char)data.m_count - 1] == 3 ||
            (char)data.m_keyBytes[(int)(char)data.m_count - 1] == 7)
        {
            res.m_4 = 0xfa;
            break;
        }

        // ORIG: cVar1 = customizingSkillCommand(user, &data);
        //       if (cVar1 != 1) { res.m_4 = 0xfc; break; }
        if (user->customizingSkillCommand(data) != 1)
        {
            res.m_4 = 0xfc;
            break;
        }
    }

    if (res.m_4 != 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            __PRETTY_FUNCTION__,
            0x3051,
            "SKILL_COMMAND_LOG : CUSTOMIZING ERROR!! ALL COMMAND DEFAULT!! m_id (%s)",
            NumberToString(user->get_acc_id(), 0));
        user->defaultSkillCommand();
    }
    return 0;
}