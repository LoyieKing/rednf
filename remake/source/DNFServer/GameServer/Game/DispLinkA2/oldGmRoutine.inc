// [P6AuditOldN.P6S2 2026-09-11 · 本波复核] 父泳道已落签名首行同步（第 3 参
//   `std::vector<std::string>&` → `StrVector&`，prett 串 ORIG 81 == OURS 81）。
//   n1_v DIFF 6625/6641 gap=-16（blocks=696，整体槽/装载序）。新钥匙判定：P339
//   （696 块、无单点槽序入口）/P0 删多余具名局部（无目标槽证据）/真 switch（无 cmp 链）
//   均不适用；未落地，维持 N（未决）。
//   另：p167fix 报 `Dispatcher_EnchantByBead::check_error` MISSING `set<int>::find const`
//   / EXTRA 非 const 版（本 TU 内非本件，父泳道线索）——本波未及处理。
int Dispatcher_New_Gmdebug_Command::oldGmRoutine(CUser* user, PacketBuf& packet,
                                                 StrVector& args)
{
    (void)packet;                        // ORIG 仅持引用，函数体不消费
    DisPatcher_DebugCommand dbgCmd;      // local_254（ORIG 栈构造）
    PacketGuard guard;                   // local_260（函数级持有，末尾析构）
    unsigned int ret = 0;


    // ---- lawless：SetCurCharacChaosPoint（0 < v < 10000）----
    if (args[0] == "lawless")
    {
        int v = std::atoi(args[1].c_str());
        if (0 < v && v < 10000)
        {
            user->SetCurCharacChaosPoint(std::atoi(args[1].c_str()));
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(100);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- bal：help abuse ratio（0 < v < 10000，×100）----
    else if (args[0] == "bal")
    {
        int v = std::atoi(args[1].c_str());
        if (0 < v && v < 10000)
        {
            user->setCurCharacHelpAbuseComputedRatio(std::atoi(args[1].c_str()) * 100);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x65);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- questclear2 / q：GM 任务旗标开关/执行 ----
    else if (args[0] == "questclear2" || args[0] == "q")
    {
        // [R16a01 2026-09-12 §15 A 轴/#33 守卫存在性] ORIG 0x0820c271 / 0x0820c29c 为
        //   **两条独立前置 `jne <共享 epilogue>`**（args[1] 为 on/off 任一 ⇒ 直接早退到
        //   函数尾共享块，字节 `0f 85 ..`）；我方原把两条件折进 `&&` 短路 ⇒ 4.4 产
        //   `xor $0x1; test; je <bool=false 汇聚点>` + 末尾 `je <epilogue>`（守卫被合并，
        //   且 on/off 命中时多走 3 条）。按 P133/§16.1 拆为两条独立前置守卫。
        if (args[1] == "on")
        {
            return 0;
        }
        if (args[1] == "off")
        {
            return 0;
        }
        {
            int v = std::atoi(args[1].c_str());
            if (0 < v && v < 100000)
            {
                user->setGmQuestFlag(true);
                user->quest_action(0x23, v, 0, 0);
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x66);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
    }
    // ---- guild exp ----
    else if (args[0] == "guild")
    {
        if (args[1] == "exp")
        {
            int v = std::atoi(args[2].c_str());
            if (0 < v && v < 10000)
            {
                user->set_guild_exp(std::atoi(args[2].c_str()));
                user->m_field796d9 = 1;                  // +0x796d9
                user->WorkPerFiveMin();
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x67);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
    }
    // ---- troublemaker on/off ----
    else if (args[0] == "troublemaker")
    {
        if (args[1] == "on")
        {
            user->setCurrCharacBlackCount(100);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x68);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
        if (args[1] == "off")
        {
            user->setCurrCharacBlackCount(0);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x68);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- fatigue set / fatigue <v> ----
    else if (args[0] == "fatigue")
    {
        if (args[1] == "set")
        {
            int v = std::atoi(args[2].c_str());
            if (-1 < v && v < 10000)
            {
                // [R9a01 2026-09-12] ORIG 0x0820c796 无形参/具名槽、无冗余掩码：
                //   `call; movzwl %ax,%eax; mov %eax,%edx; sub v,%edx`（P0 表达式直传；
                //   movzwl 已零扩展 ⇒ 原 `& 0xffff` 是多余物化，按用户裁定移除）。
                user->setCurCharacFatigue(
                    static_cast<int>(user->getCurCharacMaxFatigue()) - v);
                user->SendFatigue();
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x69);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
        else
        {
            int v = std::atoi(args[1].c_str());
            if (-1 < v && v < 10000 && 0 < v)
            {
                user->FatigueUp(v);
                user->SendFatigue();
            }
        }
    }
    // ---- show lp：查询幸运点 ----
    // [R17a01 §15 A 轴/#33 守卫存在性] ORIG 0x0820c8ca 判 `args[0]=="show"`（不匹配 →
    //   下一命令检查点），0x0820c8f5 判 `args[1]=="lp"`（**不匹配 → 0x08213440 共享
    //   ret0 块 = return 0**）。原 `&&` 合并短路使 args[1]!="lp" 时继续 else-if 链，
    //   落到下方 tautological 的 `args[0]=="show"` 分支（ORIG 0x0820cdbc 为死块）⇒ 多调
    //   DisPatcher_DebugCommand::_debugCommandVisible 并写 -0x12d 标志 ⇒ 行为分叉。
    //   按 P133/§16.1 拆为「外层 args[0] + 内层独立否定守卫」复刻 ORIG 落点。
    else if (args[0] == "show")
    {
        if (args[1] == "lp")
        {
            int luck = user->GetCurCharacLuckPoint();
            std::string text("remained luck point : ");
            // [R3a01 修复·语义优先] ORIG 此点确为 255B（0x0820c982 `mov $0x3f,%ecx` +
            //   `mov %ax,(%edx)/add $2/mov %al,(%edx)` = 0x3f*4+2+1 = 255）。旧「负结论·
            //   保留 0x100」按 gap/形态指标取舍；按用户 2026-09-12 裁定（完全复刻 ORIG，
            //   含尺寸）改回 0xff ⇒ 我方 4.4 展开 10 条 vs ORIG 9 条属 N 类形态代价。
            char b[0xff] = {0};
            std::sprintf(b, "(%d) ", luck);
            text += b;
            A2_SEND_STR(user, text);
        }
        return 0;
    }
    // ---- max lp：+100000 幸运点 ----
    // [R17a01 §15 A 轴/#33] ORIG 0x0820cb28 判 args[0]=="max"（不匹配 → 下一命令），
    //   0x0820cb53 判 args[1]=="lp"（**不匹配 → 0x08213440 共享 ret0 = return 0**）；
    //   成功路径同样 jump ret0。原 `&&` 合并短路使 args[1]!="lp" 时继续链 ⇒ 拆开复刻。
    else if (args[0] == "max")
    {
        if (args[1] == "lp")
        {
            user->IncCurCharacLuckPoint(100000);
            user->enableSaveCharacStat();
            {
                std::string s("100000 luck point!!");
                A2_SEND_STR(user, s);
            }
        }
        return 0;
    }
    // ---- item：GM 发物 ----
    else if (args[0] == "item")
    {
        unsigned long v = static_cast<unsigned long>(std::atoi(args[1].c_str()));
        dbgCmd._debugCommandAddItem(user, v, 100);
        return 0;
    }
    // ---- show / hide（顶级可见性）----
    else if (args[0] == "show")
    {
        MSG_DEBUG_COMMAND dbg;
        dbgCmd._debugCommandVisible(user, dbg);
        return 0;
    }
    else if (args[0] == "hide")
    {
        MSG_DEBUG_COMMAND dbg;
        dbgCmd._debugCommandInvisible(user, dbg);
        return 0;
    }
    // ---- dirty ----
    else if (args[0] == "dirty")
    {
        DoUserDefineCommand(user, 0x1f, args[1].c_str());
    }
    // ---- avatar：GM 发头像 ----
    else if (args[0] == "avatar")
    {
        MSG_DEBUG_COMMAND dbg;
        dbg.m_arg = std::atoi(args[1].c_str());
        dbg.payload.avatar.periodKind = static_cast<unsigned char>(std::atoi(args[2].c_str()));
        dbg.payload.avatar.slotByte = static_cast<signed char>(std::atoi(args[3].c_str()));
        dbgCmd._debugCommandAddAvatar(user, dbg);
        return 0;
    }
    // ---- combo：无操作（ORIG 匹配后无动作）----
    else if (args[0] == "combo")
    {
        // ORIG 空分支
    }
    // ---- coin：扣/清金币 ----
    else if (args[0] == "coin")
    {
        int v = std::atoi(args[1].c_str());
        unsigned int coin = user->getCurCharacInvenR()->GetCoin();
        unsigned int left = coin - v;
        if (static_cast<int>(left) < 0)
            left = 0;
        user->getCurCharacInvenW()->SetCoin(left);
        if (v == 0)
        {
            user->getCurCharacInvenW()->SetCoin(0);
            user->getCurCharacInvenW()->SetPayCoin(0);
            user->getCurCharacInvenW()->SetEventCoin(0);
        }
        user->SendCoin();
    }
    // ---- use item：重复 increase_status ----
    else if (args[0] == "use")
    {
        if (args[1] == "item")
        {
            int kind = std::atoi(args[2].c_str());
            int count = std::atoi(args[3].c_str());
            if (kind < 1 || 6 < kind)
            {
                ret = 0;
                return 0;
            }
            kind = kind + 2;
            for (int i = 0; i < count; ++i)
                user->increase_status(static_cast<short>(kind));
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, kind);
        }
    }
    // ---- npcfav / favterm / pickup / refund / durable / bluemarble：双参透传 ----
    else if (args[0] == "npcfav")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x24, b);
    }
    else if (args[0] == "favterm")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x23, b);
    }
    // ---- sumhell / sdale ----
    else if (args[0] == "sumhell")
    {
        DoUserDefineCommand(user, 0x15, args[1].c_str());
    }
    else if (args[0] == "sdale")
    {
        DoUserDefineCommand(user, 0x1c, args[1].c_str());
    }
    // ---- mdt / monsterdroptest / startmdt / endmdt / goldmdt ----
    else if (args[0] == "mdt" || args[0] == "monsterdroptest" || args[0] == "startmdt" ||
             args[0] == "endmdt" || args[0] == "goldmdt")
    {
        char b[0xff] = {0};     // [R3a01 修复] ORIG 0x0820d4f8 为 255B（0x3f+word+byte）
        if (args[0] == "startmdt")
            std::sprintf(b, "%d %s", 1, args[1].c_str());
        else if (args[0] == "endmdt")
            std::sprintf(b, "%d %s", 2, args[1].c_str());
        else if (args[0] == "goldmdt")
            std::sprintf(b, "%d %s", 3, args[1].c_str());
        else
            std::sprintf(b, "%d %s", 0, args[1].c_str());
        DoUserDefineCommand(user, 0x4b, b);
    }
    // ---- cli / checklottoitem ----
    else if (args[0] == "cli" || args[0] == "checklottoitem")
    {
        char b[0xff] = {0};     // [R3a01 修复] ORIG 0x0820d712 为 255B（0x3f+word+byte）
        if (args[1] == "ven")
        {
            std::sprintf(b, "%d %s", 1, args[2].c_str());
            DoUserDefineCommand(user, 0x4d, b);
        }
        else
        {
            std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
            DoUserDefineCommand(user, 0x4c, b);
        }
    }
    // ---- cbind / checkbind ----
    else if (args[0] == "cbind" || args[0] == "checkbind")
    {
        DoUserDefineCommand(user, 0x4e, args[1].c_str());
    }
    // ---- oneadayitem / craneDay / cinven ----
    else if ("oneadayitem" == args[0])
    {
        DoUserDefineCommand(user, 0x52, args[1].c_str());
    }
    else if ("craneDay" == args[0])
    {
        DoUserDefineCommand(user, 0x79, args[1].c_str());
    }
    else if (args[0] == "cinven")
    {
        DoUserDefineCommand(user, 0x46, args[1].c_str());
    }
    // ---- pickup / refund ----
    else if (args[0] == "pickup")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x47, b);
    }
    else if (args[0] == "refund")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x48, b);
    }
    // ---- tb reset/adjust/no ----
    else if (args[0] == "tb")
    {
        if (args[1] == "reset")
            DoUserDefineCommand(user, 0x5d, args[1].c_str());
        else if (args[1] == "adjust")
            DoUserDefineCommand(user, 0x5e, args[1].c_str());
        else if (args[1] == "no")
            DoUserDefineCommand(user, 0x5f, args[1].c_str());
    }
    // ---- achieve/ac reset/report/clear/all/no ----
    else if (args[0] == "achieve" || args[0] == "ac")
    {
        if (args[1] == "reset")
            DoUserDefineCommand(user, 0x60, args[1].c_str());
        else if (args[1] == "report")
            DoUserDefineCommand(user, 0x61, args[1].c_str());
        else if (args[1] == "clear")
            DoUserDefineCommand(user, 0x62, args[2].c_str());
        else if (args[1] == "all")
            DoUserDefineCommand(user, 99, args[1].c_str());
        else if (args[1] == "no")
            DoUserDefineCommand(user, 100, args[1].c_str());
    }
    // ---- power userpoint/guildpoint/win ----
    else if (args[0] == "power")
    {
        if (args[1] == "userpoint")
        {
            MSG_DEBUG_COMMAND dbg;
            dbg.m_arg = std::atoi(args[2].c_str());
            dbgCmd._debugCommandUserPowerwarPointUp(user, dbg);
            return 0;
        }
        else if (args[1] == "guildpoint")
        {
            int v = std::atoi(args[2].c_str());
            unsigned int characNo = user->getCurCharacNo();
            unsigned int guildKey = user->get_charac_guildkey();
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendIncreaseDecreasePowerWarPoint(guildKey, characNo, v);
        }
        else if (args[1] == "win")
        {
            DoUserDefineCommand(user, 0x2b, args[2].c_str());
        }
    }
    // ---- townatk success/fail/end/其它 ----
    else if (args[0] == "townatk")
    {
        if (args[1] == "success")
            SendVillageAttackedGMCommand(user, 1);
        else if (args[1] == "fail")
            SendVillageAttackedGMCommand(user, 2);
        else if (args[1] == "end")
            SendVillageAttackedGMCommand(user, 3);
        else
            DoUserDefineCommand(user, 0x1e, args[1].c_str());
    }
    // ---- rec start/stop/run ----
    else if (args[0] == "rec")
    {
        if (args[1] == "start")
            DoUserDefineCommand(user, 0x12, args[1].c_str());
        else if (args[1] == "stop")
            DoUserDefineCommand(user, 0x13, args[1].c_str());
        else if (args[1] == "run")
            DoUserDefineCommand(user, 0x14, args[1].c_str());
    }
    // ---- spd / durable ----
    else if (args[0] == "spd")
    {
        DoUserDefineCommand(user, 9, args[1].c_str());
    }
    else if (args[0] == "durable")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x29, b);
    }
    // ---- sep upgrade/up success|s|fail|f ----
    else if (args[0] == "sep" && (args[1] == "upgrade" || args[1] == "up"))
    {
        char b[0xff] = {0};     // [R3a01 修复] ORIG 0x0820e4a4 为 255B（0x3f+word+byte）
        if (args[2] == "success" || args[2] == "s")
        {
            if (user->getUpgradeSeparateTrueFlag() == 0)
            {
                // ORIG 0x0820e530/0x0820e58c：数组以 `movl $imm` + `movb $0`
                // 共 5 字节一次初始化 ⇒ 源形是定长 char[5] 字符串初始化器。
                char ok[5] = "ok";
                user->setUpgradeSeparateTrueFlag(1);
                user->setUpgradeSeparateFalseFlag(0);
                std::sprintf(b, "sep upgrade success - %s", ok);
            }
            else
            {
                char end[5] = "end";
                user->setUpgradeSeparateTrueFlag(0);
                std::sprintf(b, "sep upgrade success - %s", end);
            }
        }
        else if (args[2] == "fail" || args[2] == "f")
        {
            if (user->getUpgradeSeparateFlaseFlag() == 0)
            {
                char ok[5] = "ok";
                user->setUpgradeSeparateTrueFlag(0);
                user->setUpgradeSeparateFalseFlag(1);
                std::sprintf(b, "sep upgrade fail - %s", ok);
            }
            else
            {
                char end[5] = "end";
                user->setUpgradeSeparateFalseFlag(0);
                std::sprintf(b, "sep upgrade fail - %s", end);
            }
        }
        {
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
        }
        return 0;
    }
    // ---- initstat / guilddebug / delguild / changename ----
    else if (args[0] == "initstat")
    {
        DoUserDefineCommand(user, 0x3f, args[1].c_str());
    }
    else if (args[0] == "guilddebug")
    {
        DoUserDefineCommand(user, 0x42, args[1].c_str());
    }
    else if (args[0] == "delguild")
    {
        DoUserDefineCommand(user, 0x2c, args[1].c_str());
    }
    else if (args[0] == "changename")
    {
        DoUserDefineCommand(user, 0x2d, args[1].c_str());
    }
    // ---- startevent（三参）/ endevent ----
    else if (args[0] == "startevent")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s %s", args[1].c_str(), args[2].c_str(), args[3].c_str());
        DoUserDefineCommand(user, 0x2e, b);
    }
    else if (args[0] == "endevent")
    {
        DoUserDefineCommand(user, 0x2f, args[1].c_str());
    }
    // ---- shopcheck / shopnpc ----
    else if (args[0] == "shopcheck")
    {
        DoUserDefineCommand(user, 0x30, args[1].c_str());
    }
    else if (args[0] == "shopnpc")
    {
        DoUserDefineCommand(user, 0x31, args[1].c_str());
    }
    // ---- cargo delete ----
    else if (args[0] == "cargo")
    {
        if (args[1] == "delete")
            DoUserDefineCommand(user, 0x36, args[1].c_str());
    }
    // ---- guildfund / dropitem ----
    else if (args[0] == "guildfund")
    {
        DoUserDefineCommand(user, 0x41, args[1].c_str());
    }
    else if (args[0] == "dropitem")
    {
        DoUserDefineCommand(user, 0x34, args[1].c_str());
    }
    // ---- premiumdelete/pd / mailitem ----
    else if (args[0] == "premiumdelete" || args[0] == "pd")
    {
        DoUserDefineCommand(user, 0x3c, args[1].c_str());
    }
    else if (args[0] == "mailitem")
    {
        DoUserDefineCommand(user, 0x3d, args[1].c_str());
    }
    // ---- ci true/false ----
    else if (args[0] == "ci")
    {
        if (args[1] == "true")
            DoUserDefineCommand(user, 0x6d, args[2].c_str());
        else if (args[1] == "false")
            DoUserDefineCommand(user, 0x6d, args[1].c_str());
    }
    // ---- ritemclear / difficulty ----
    else if (args[0] == "ritemclear")
    {
        DoUserDefineCommand(user, 0x76, args[1].c_str());
    }
    else if (args[0] == "difficulty")
    {
        DoUserDefineCommand(user, 0x78, args[1].c_str());
    }
    // ---- bluemarble ----
    else if (args[0] == "bluemarble")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x7d, b);
    }
    // ---- ulti max ----
    // [R17a01 §15 A 轴/#33] ORIG 0x0820f007 判 args[0]=="ulti"（不匹配 → 0x0820f217
    //   下一命令），0x0820f044 判 args[1]=="max"（**不匹配 → 0x08213440 ret0**）；
    //   成功路径 0x0820f212 `jmp 0x08213440` 亦 return 0。
    else if (args[0] == "ulti")
    {
        if (args[1] == "max")
        {
            // ORIG 0x0820f04a..0x0820f074：G_CDataManager()->get_limit_inout_count(2)
            // 的 char 返回值经 movsbl 直接落 0x8(%esp)，不物化中间变量。
            user->setUltimateInoutValue(
                2, G_CDataManager()->get_limit_inout_count((ENUM_BLOOD_TYPE)2));
            {
                std::string s("Add ultimate inout max count!!");
                A2_SEND_STR(user, s);
            }
        }
        return 0;
    }
    // ---- dimen max ----
    // [R17a01 §15 A 轴/#33] ORIG 0x0820f22a 判 args[0]=="dimen"（不匹配 → 0x0820f462
    //   下一命令），第二条件（"max"）不匹配 → 0x08213440 ret0；成功路径亦 return 0。
    else if (args[0] == "dimen")
    {
        if (args[1] == "max")
        {
            // ORIG 0x0820f26d..0x0820f2c4：循环条件为 `i <= 5`（cmpl $0x5 / setle），
            // 且 get_dimensionInout 的 char 返回值经 movsbl 直接进实参槽，无中间变量。
            for (int i = 0; i <= 5; ++i)
            {
                user->setDemensionInoutValue(static_cast<char>(i),
                                             G_CDataManager()->get_dimensionInout(i));
            }
            {
                std::string s("Add dimension inout max count!!");
                A2_SEND_STR(user, s);
            }
        }
        return 0;
    }
    // ---- dontsend on/off ----
    else if (args[0] == "dontsend")
    {
        if (args[1] == "on")
            user->SetDebugCommand(1);
        else if (args[1] == "off")
            user->SetDebugCommand(0);
    }
    // ---- pvpskilllevel ----
    else if (args[0] == "pvpskilllevel")
    {
        int v = std::atoi(args[1].c_str());
        if (0x25 < v)
        {
            return 0;
        }
        int nextPoint = GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(v);
        user->getPVPResultW()->m_expPoint = nextPoint;   // +0x08
        user->getPVPResultW()->m_pvpGrade = v;           // +0x14
        G_GameWorld()->send_AllBasicInfo(user);
        user->givePvPSkillTree(0, false, 3);
        user->send_skill_info();
    }
    // ---- lethe：清技能 ----
    else if (args[0] == "lethe")
    {
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())->clear_all_skills_both();
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())->set_parent(user);
        {
            WongWork::CSkillChanger changer;
            changer.SkillInitialize(user, 0, false);
            user->givePvPSkillTree(0, false, 3);
            user->send_skill_info();
        }
    }
    // ---- rematch ----
    else if (args[0] == "rematch")
    {
        PvP_Room* pvpRoom = static_cast<PvP_Room*>(user->GetPVPRoom());
        if (pvpRoom != 0)
        {
            // IMatch 接口已权威声明（MatchRoomList.h）：OnAskRematch(CUser*,int)
            // 位于虚表 +0x68。具名虚调用与原裸槽直调生成同一 vtable+0x68 派发，
            // 语义/链接不变，仅消除 hardcode 偏移（§4.3；同 PvP_Room.cpp 的
            // ((IMatch*)m_matching)->... 口径）。m_matching 以 int 存储。
            IMatch* match = reinterpret_cast<IMatch*>(pvpRoom->m_matching);
            if (match != 0)
                match->OnAskRematch(user, std::atoi(args[1].c_str()));
        }
    }
    // ---- wjfakd / wjfakd2 ----
    else if ("wjfakd" == args[0])
    {
        TOD_Layer layer(static_cast<unsigned short>(std::atoi(args[1].c_str())));
        TOD_UserState* state =
            reinterpret_cast<TOD_UserState*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
        state->setEnterLayer(layer);
        TOD_Layer today = state->getTodayEnterLayer();
        // [R3a01 修复] ORIG 0x0820f86b 直接 `lea -0x668(%ebp),%eax` 进 sprintf，
        //   该分支（0x0820f7ca 入口）无任何 rep stos/memset ⇒ 原码缓冲未初始化；
        //   删除我方多余 `= {0}`（复刻 ORIG；仅影响被调方对未写区的读取行为）。
        char b[0x100];
        std::sprintf(b, "set Enter Layer : %d",
                     static_cast<int>(today.GetLayer()) & 0xffff);
        TowerOfDespairMgr::SendMessageToCUser(user, b);
    }
    else if ("wjfakd2" == args[0])
    {
        if ("reload" == args[1])
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xb719);
            CStreamGuard g(s, true);
            (*g)->operator<<(0x277);
            (*g)->operator<<(-1);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
        else if ("randomselect" == args[1])
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xb720);
            CStreamGuard g(s, true);
            (*g)->operator<<(0x275);
            (*g)->operator<<(-1);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
        else
        {
            // ORIG：反序比较 "layerinfo" == args[1]，结果未使用（空分支）。
            (void)("layerinfo" == args[1]);
        }
    }
    // ---- mission 系列 ----
    else if ("mission" == args[0])
    {
        int v2 = std::atoi(args[2].c_str());
        int v1 = std::atoi(args[1].c_str());
        user->SetMissionParameter(v1, v2);
        user->send_MissionList();
    }
    else if ("newdailymission" == args[0])
    {
        user->ResetDailyMission();
        user->send_MissionList();
    }
    else if ("clearmission" == args[0])
    {
        user->ClearMission(static_cast<short>(std::atoi(args[1].c_str())));
        user->send_MissionList();
        user->send_pvp_record();
    }
    else if ("clearmissionkind" == args[0])
    {
        user->ClearMissionKind(static_cast<short>(std::atoi(args[1].c_str())));
        user->send_MissionList();
        user->send_pvp_record();
    }
    else if ("resetmissionlist" == args[0])
    {
        user->ResetMissionList();
        user->send_MissionList();
    }
    // ---- resetdaily ----
    else if ("resetdaily" == args[0])
    {
        G_GameWorld()->ResetAllFatigue();
        G_GameWorld()->ReselectDailyQuest();
        G_GameWorld()->AddDailyItem();
    }
    // ---- combomission ----
    else if ("combomission" == args[0])
    {
        short v = static_cast<short>(std::atoi(args[1].c_str()));
        short kind = user->getCurCharacGrowType() == 0 ? 0x1f : 0x20;
        user->Update_ComboMission(kind, v);
    }
    // ---- roiregen：ORIG 空分支（args[0] 匹配后直接 return 0）----
    else if ("roiregen" == args[0])
    {
        // ORIG 空分支
    }
    // ---- qp ----
    else if (args[0] == "qp")
    {
        user->setQuestPoint(std::atoi(args[1].c_str()));
    }
    // ---- registauction / burningstage ----
    else if (args[0] == "registauction")
    {
        random_option::CRandomOptionItemHandleTest test;
        test.test_regist_auction(user);
    }
    else if (args[0] == "burningstage")
    {
        int v = std::atoi(args[1].c_str());
        reinterpret_cast<fair_pvp::CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetRemainBonusCount(v);
        int daily = static_cast<int>(
            G_CDataManager()->m_serverParameter.m_schoolMatchDailyPlayLimit) - v;
        reinterpret_cast<fair_pvp::CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetDailyPlayCount(daily);
    }
    // ---- randitem ----
    else if (args[0] == "randitem")
    {
        if (args[1] == "")
        {
            return 0;
        }
        int itemIdx = std::atoi(args[1].c_str());
        Inven_Item item;                       // local_4d5（0x3d 字节）
        item.m_addInfo = itemIdx;              // +0x02
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            return 0;
        }
        pItem->make_item(item);                // vtbl+0x08 槽 make_item
        // ORIG +0x25 起连续写入 RandomOption 的三个 3-byte 字段
        // （m_field0/m_field3/m_field6）；不得从 m_field3.m_field1（+0x29）
        // 再叠加 +0x29，否则会越过 Inven_Item 的随机选项区。
        unsigned char* raw = &item.m_random.m_random.m_field0.m_field0;
        int idx = 2;
        for (int i = 0; i < 3; ++i)
        {
            if (args[idx] == "")
                break;
            if (args[idx + 1] == "")
                break;
            // ORIG 0x82100d0/0x821011d/0x8210166：相对 local_4b1 的
            // +1/+2/+3 即 RandomOption 三字节字段的 +0/+1/+2；第三个
            // 值复用第二个参数，保持 ORIG 行为。
            raw[i * 3 + 0] = static_cast<unsigned char>(std::atoi(args[idx].c_str()));
            raw[i * 3 + 1] = static_cast<unsigned char>(std::atoi(args[idx + 1].c_str()));
            raw[i * 3 + 2] = static_cast<unsigned char>(std::atoi(args[idx + 1].c_str()));
            idx += 2;
        }
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)6, true, true);
        if (slot >= 0)
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
    // ---- insertitem（拼串 find_item → AddItem）----
    else if (args[0] == "insertitem")
    {
        std::string name;
        for (int i = 1; i < 10; ++i)
        {
            if (args[i].length() == 0)
                break;
            if (name.length() != 0)
                name += " ";
            name += args[i];
        }
        CItem* pItem = G_CDataManager()->find_item(name.c_str());
        if (pItem == 0)
        {
            char b[0xff] = {0};     // [R3a01 修复] ORIG 0x082103bf 为 255B（0x3f+word+byte）
            // [R4b01 2026-09-12 · Main 裁决回滚] R4a01 曾把本串改成 "cannot find item"（误修）：
            // ORIG 0x082103e7 `movl $0x8bc3069,0x4(%esp)`，0x8bc3069 处 raw 字节实测为
            // "cannot find item (%s) \0"（含尾空格）；ORIG 全库计数 长串=2 / 短串=0，
            // 且 ORIG 同点 0x082103e3 压入 c_str() 到 0x8(%esp) ⇒ %s 实参被消费，保留原形。
            std::sprintf(b, "cannot find item (%s) ", name.c_str());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
            return 0;
        }
        {
            char b[0xff] = {0};     // [R3a01 修复] ORIG 0x08210525 为 255B（0x3f+word+byte）
            std::sprintf(b, "item_index(%d) ", pItem->get_index());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
        }
        int itemIdx = pItem->get_index();
        int count = 100;
        // [R9a01 2026-09-12] 补 ORIG 0x0821068f 判空守卫（`cmpl $0x0,-0xc0(%ebp);
        // je 0x082106d9`，与 is_stackable 的 je 同落点 ⇒ 短路形 `pItem != 0 &&`）。
        if (pItem != 0)
        {
            if (pItem->is_stackable())
            {
                if (static_cast<CStackableItem*>(pItem)->getStackableLimit() < count)
                {
                    count = static_cast<CStackableItem*>(pItem)->getStackableLimit();
                }
            }
        }
        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
        int slot = user->AddItem(itemIdx, count, (eItemAddReason)6, space, 0);
        // ORIG 0x0821071f `js` 到公共返回尾；正路径整体包在 if (slot >= 0) 内，
        // 两条路径都返回 0。先 getCurCharacName（%edi）再 get_acc_id→NumberToString
        // （%esi），最后构造 cMyTrace。
        if (slot >= 0)
        {
            const char* name = user->getCurCharacName();
            char* accIdStr = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0xb916, 0);
            trace("%s:\'%s\' GM Mode Add Item(%d).", accIdStr, name, slot);
            user->SendUpdateItemList(CUser::eSendTarget_1, space, slot);
        }
        return 0;
    }
    // ---- itemskill ----
    else if (args[0] == "itemskill")
    {
        int a = std::atoi(args[1].c_str());
        int b = std::atoi(args[2].c_str());
        CItemMakingSkill* skill = reinterpret_cast<CItemMakingSkill*>(
            user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(10)));
        skill->SetItemMakingSkillPoint(a, b, user);
    }
    // ---- multimail stack/equip ----
    else if (args[0] == "multimail")
    {
        if (args[1] == "stack")
        {
            std::vector<std::pair<int, int> > items;
            items.clear();
            items.push_back(std::make_pair(5, 0xbdd));
            items.push_back(std::make_pair(4, 0xbda));
            items.push_back(std::make_pair(3, 0xbd9));
            items.push_back(std::make_pair(2, 0xbdb));
            items.push_back(std::make_pair(1, 0xbdc));
            Inven_Item base[10];   // ORIG 原地默认构造 10 元素（无临时/赋值）
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, base, 10);
            // ORIG 把消息串存入栈局部再 strlen(local_9c)，非折叠；用指针变量复现。
            const char* pMsg = "stackable item test";
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                "multimail test", base, 5, 1000, user->getCurCharacNo(),
                pMsg,
                static_cast<int>(std::strlen(pMsg)),
                0, (ENUM_SERVER_GROUP)99, false);
        }
        else if (args[1] == "equip")
        {
            std::vector<std::pair<int, int> > items;
            items.clear();
            items.push_back(std::make_pair(5, 10000));
            items.push_back(std::make_pair(5, 0x2711));
            items.push_back(std::make_pair(5, 0x2712));
            items.push_back(std::make_pair(5, 0x2713));
            items.push_back(std::make_pair(5, 0x2714));
            items.push_back(std::make_pair(5, 0x2715));
            items.push_back(std::make_pair(5, 0x2716));
            items.push_back(std::make_pair(5, 0x2717));
            items.push_back(std::make_pair(5, 0x2718));
            items.push_back(std::make_pair(5, 0x2719));
            Inven_Item base[10];   // ORIG 原地默认构造 10 元素（无临时/赋值）
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, base, 10);
            // ORIG 把消息串存入栈局部再 strlen(local_9c)，非折叠；用指针变量复现。
            const char* pMsg = "equip item test";
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                "multimail test", base, 10, 9999, user->getCurCharacNo(),
                pMsg,
                static_cast<int>(std::strlen(pMsg)),
                0, (ENUM_SERVER_GROUP)99, false);
        }
    }
    // ---- statistic gold ----
    else if (args[0] == "statistic")
    {
        if (args[1] == "gold")
        {
            GetInstanceCirculationStatistic()->InsertCirculationStatistic();
        }
    }
    // ---- ingameday/day ----
    else if ("ingameday" == args[0] || "day" == args[0])
    {
        DoUserDefineCommand(user, 0x66, args[1].c_str());
    }
    // ---- setinven ----
    else if (args[0] == "setinven")
    {
        int cap = std::atoi(args[1].c_str());
        int oldCap = user->getCurCharacInvenR()->get_inventory_capacity();
        user->m_historyLog.ExpandInventory(oldCap, cap, 2);
        const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(cap);
        int newCap = user->getCurCharacInvenR()->get_inventory_capacity();
        DB_UpdateInventoryCapacity::makeRequest(user->GetServerGroup(),
                                                static_cast<int>(user->GetUID()),
                                                user->getCurCharacNo(), newCap);
        guard.clear();
        guard.put_header(0, 0x42);
        guard.put_short(0xb);
        guard.put_short(user->getCurCharacInvenR()->get_inventory_capacity());
        guard.finalize(true);
        user->Send(guard);
    }
    // ---- itemseed ----
    else if (args[0] == "itemseed")
    {
        int itemIdx = std::atoi(args[1].c_str());
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            char b[0xff] = {0};     // [R3a01 修复] ORIG 0x08211337 为 255B（0x3f+word+byte）
            // [R4b01 2026-09-12] 同 0x082103e7：ORIG 0x0821136c 亦取 0x8bc3069 ⇒ 保留下方原形。
            std::sprintf(b, "cannot find item (%s) ", args[1].c_str());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
            return 0;
        }
        int idx = pItem->get_index();
        int count = 100;
        // [R9a01 2026-09-12] 补 ORIG 0x082114b6 判空守卫（`cmpl $0x0,-0x90(%ebp);
        // je 0x08211500`，与 is_stackable 的 je 同落点 ⇒ 短路形 `pItem != 0 &&`）。
        if (pItem != 0)
        {
            if (pItem->is_stackable())
            {
                if (static_cast<CStackableItem*>(pItem)->getStackableLimit() < count)
                {
                    count = static_cast<CStackableItem*>(pItem)->getStackableLimit();
                }
            }
        }
        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
        int slot = user->AddItem(idx, count, (eItemAddReason)6, space, 0);
        // ORIG 0x08211546 `js` 到公共返回尾：正路径包在 if (slot >= 0) 内，
        // 两条路径均返回 0（无提前 return）。
        if (slot >= 0)
        {
            Inven_Item* ref = user->getCurCharacInvenRefW()->GetInvenRef(1, slot);
            if (ref != 0)
                ref->m_addInfo2 = std::atoi(args[2].c_str());   // +0x07 add_info
            // ORIG 0x082115a9 先取角色名（留 %esi），再 get_acc_id → NumberToString
            // （留 %ebx），最后才构造 cMyTrace。
            const char* name = user->getCurCharacName();
            char* accIdStr = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0xb9fb, 0);
            trace("%s:\'%s\' GM Mode Add Item(%d).", accIdStr, name, slot);
            user->SendUpdateItemList(CUser::eSendTarget_1, space, slot);
        }
        return 0;
    }
    // ---- moveavatar/mva ----
    else if (args[0] == "moveavatar" || args[0] == "mva")
    {
        PacketDispatcher* disp = G_PacketDispatcher();
        IDispatcher* d13 = disp->getDispatcher(0x13);
        if (d13 == 0)
        {
            return 0;
        }
        {
            PacketGuard g;
            g.put_header(1, 0x13);
            g.put_byte(1);
            g.put_short(0);
            g.put_int(0);
            g.put_int(1);
            g.put_byte(3);
            g.put_short(1);
            g.put_int(0x9b07);
            g.put_int(0);
            g.put_byte(0);
            g.put_byte(0);
            g.put_byte(0);
            g.finalize(false);
            g.set_index(0xf);
            // ORIG：vtbl[0]（dispatch_template）以 (obj, user, PacketBuf*) 调用。
            d13->dispatch_template(user, *g.get());
        }
    }
    // ---- quickon/qon ----
    else if (args[0] == "quickon" || args[0] == "qon")
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() >= 2)
        {
            party->send_quick_party_matching_result(true, 0);
            party->set_quick_party(true);
        }
        return 0;
    }
    // ---- quickoff/qoff ----
    else if (args[0] == "quickoff" || args[0] == "qoff")
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() >= 2)
        {
            party->send_quick_party_matching_result(false, 0);
            party->set_quick_party(false);
        }
        return 0;
    }
    // ---- quickreward/qr ----
    else if (args[0] == "quickreward" || args[0] == "qr")
    {
        int v = std::atoi(args[1].c_str());
        if (v < 0 || 10 < v)
        {
            return 0;
        }
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0 && party->get_member_count() >= 2)
            {
                party->set_gm_random_buff_type(static_cast<QuickParty::RandomBuffType>(v));
                char b[0xff] = {0};     // [R3a01 修复] ORIG 0x08211b24 为 255B（0x3f+word+byte）
                std::sprintf(b, "GM QUICK PARTY RANDOM BUFF (%d) TYPE!!", v);
                PacketGuard g;
                g.put_header(0, 0xc);
                g.put_byte(0);
                g.put_short(0);
                g.put_byte(0);
                g.put_int(static_cast<int>(std::strlen(b)));
                g.put_str(b, static_cast<int>(std::strlen(b)));
                g.finalize(true);
                party->send_to_party(g);
            }
        }
        return 0;
    }
    // ---- hellvalue/hv ----
    else if (args[0] == "hellvalue" || args[0] == "hv")
    {
        int v = std::atoi(args[1].c_str());
        if (v < 0 || 100 < v)
        {
            return 0;
        }
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0 && party->get_member_count() >= 2)
            {
                BattleData* bd = &party->m_battleData;   // +0x32c（MonC9 具名化：m_padBattleData→BattleData m_battleData）
                bd->SetHellPartyValueTotal(v);
                party->send_hellParty_value();
                char b[0xff] = {0};     // [R3a01 修复] ORIG 0x08211d6e 为 255B（0x3f+word+byte）
                std::sprintf(b, "HELL PARTY VALUE (%d)",
                             bd->GetHellPartyValueTotal());
                PacketGuard g;
                g.put_header(0, 0xc);
                g.put_byte(0);
                g.put_short(0);
                g.put_byte(0);
                g.put_int(static_cast<int>(std::strlen(b)));
                g.put_str(b, static_cast<int>(std::strlen(b)));
                g.finalize(true);
                party->send_to_party(g);
            }
        }
        return 0;
    }
    // ---- statisticlog/sl ----
    else if (args[0] == "statisticlog" || args[0] == "sl")
    {
        StatisticsGmCmdPacket pkt;
        pkt.m_characNo = user->getCurCharacNo();      // +0x0a（ORIG local_34c）
        CStatisticServerProxy* proxy =
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0xe);
    }
    // ---- itemdrop / upcard ----
    else if (args[0] == "itemdrop")
    {
        DoUserDefineCommand(user, 0x6f, args[1].c_str());
    }
    else if (args[0] == "upcard")
    {
        DoUserDefineCommand(user, 0x70, args[1].c_str());
    }
    // ---- inde item drop ----
    else if (args[0] == "inde" && args[1] == "item" && args[2] == "drop")
    {
        std::string st;
        if (user->getForceDropFlag() == 1)
            st = "off";
        else
            st = "on";
        DoUserDefineCommand(user, 0x77, st.c_str());
    }
    // ---- timegate/tg ----
    else if (args[0] == "timegate" || args[0] == "tg")
    {
        Packet_CollectItemsGm pkt;
        pkt.m_a = std::atoi(args[1].c_str());            // +0x0b（ORIG local_368）
        pkt.m_b = std::atoi(args[2].c_str());            // +0x0f（ORIG local_364）
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x17);
    }
    // ---- bossdifficult / bossmap ----
    else if (args[0] == "bossdifficult")
    {
        char v = static_cast<char>(StringToNumber(args[1].c_str()));
        // [R8a01 2026-09-12 ④修正] ORIG 0x082122aa..0x082122bb **双独立守卫**：
        //   `mov %al,-0x5d(%ebp); cmpb $0x0,-0x5d(%ebp); jle 0x08213440(fail)`
        //   +`cmpb $0x4,-0x5d(%ebp); jg 0x08213440(fail)` ⇒ 下界 0 与上界 4 都在，
        //   两跳同落统一失败出口（0x796cc = m_gmBossDifficulty）。
        //   旧注释「ORIG 无下界检查」系误断（#18 推断注释），已删去 `v > '\0'` 的
        //   行为会放过 0/负值 ⇒ 恢复 ORIG 双边界。
        if (v > '\0' && v < '\x05')
            user->m_gmBossDifficulty = v;              // +0x796cc
    }
    else if (args[0] == "bossmap")
    {
        unsigned int v = static_cast<unsigned int>(StringToNumber(args[1].c_str()));
        user->m_gmBossMapIndex = v;                    // +0x796d0
    }
    // ---- reset fatigue ----
    // [R17a01 §15 A 轴/#33] ORIG 0x08212344 判 args[0]=="reset"（不匹配 → 0x082123ca
    //   下一命令），0x0821237d 判 args[1]=="fatigue"（**不匹配 → 0x08213440 ret0**）；
    //   成功路径 0x082123c0 亦 `mov $0,%ebx; jmp ret0` ⇒ 拆成外层 args[0] + 内层条件。
    else if (args[0] == "reset")
    {
        if (args[1] == "fatigue")
        {
            G_TimerQueue()->InsertTimer(
                static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                static_cast<TIMER_MESSAGE>(0x50), 3, 0, 0);
        }
        return 0;
    }
    // ---- apply_effect ----
    else if (args[0] == "apply_effect")
    {
        int code = StringToNumber(args[1].c_str());
        int value = StringToNumber(args[2].c_str());
        if (code < 0)
        {
            ret = 0;
        }
        else
        {
            AradServerStateMessage msg;
            msg.m_active = static_cast<short>(value != 0);
            msg.m_endSec = static_cast<unsigned int>(value);
            GlobalData::s_serverStateManager_->deleteState(
                static_cast<ENUM_GLOBAL_EFFECT_ARAD>(code));
            GlobalData::s_serverStateManager_->insertState(
                static_cast<ENUM_GLOBAL_EFFECT_ARAD>(code), msg);
            ret = 0;
        }
        return 0;
    }
    // ---- open sd/boss/aa ----
    else if (args[0] == "open")
    {
        if (args[1] == "sd")
            GlobalData::s_DailyScheduleManager->GM_OpenSouthernDale();
        else if (args[1] == "boss")
            GlobalData::s_DailyScheduleManager->GM_OpenBossTower();
        else if (args[1] == "aa")
            GlobalData::s_DailyScheduleManager->GM_OpenAdvanceAltar();
    }
    // ---- close sd/boss/aa ----
    else if (args[0] == "close")
    {
        if (args[1] == "sd")
            GlobalData::s_DailyScheduleManager->GM_CloseSouthernDale();
        else if (args[1] == "boss")
            GlobalData::s_DailyScheduleManager->GM_CloseBossTower();
        else if (args[1] == "aa")
            GlobalData::s_DailyScheduleManager->GM_CloseAdvanceAltar();
    }
    // ---- ap set/reset/gmmode/realmode/week ----
    else if (args[0] == "ap")
    {
        if (args[1] == "set")
        {
            APSystem::CUserProc::GM_Set(user, static_cast<unsigned int>(std::atoi(args[2].c_str())));
        }
        else if (args[1] == "reset")
        {
            APSystem::CUserProc::GM_Reset(user);
        }
        else if (args[1] == "gmmode")
        {
            APSystem::CActionPointManager::_IsGMOn = 1;
        }
        else if (args[1] == "realmode")
        {
            APSystem::CActionPointManager::_IsGMOn = 0;
            // [R8a01 2026-09-12 ④修正] ORIG 0x08212791/0x08212798 双**字节**清零：
            //   `movb $0x0,0x93ff07c`(= _IsGMOn, nm 实证) +
            //   `movb $0x0,0x937008c`(= _gmWeekday.m_gmOn，即 _gmWeekdayE+0x0)。
            //   旧形写 m_weekday(+0x4, dword) 是**字段错位**（0x9161664/_gmWeekdayE+0x4）。
            APSystem::CActionPointManager::_gmWeekday.m_gmOn = 0;
        }
        else if (args[1] == "week")
        {
            int v = std::atoi(args[2].c_str());
            int weekday = 7;
            switch (v)
            {
                case 1: weekday = 1; break;
                case 2: weekday = 2; break;
                case 3: weekday = 3; break;
                case 4: weekday = 4; break;
                case 5: weekday = 5; break;
                case 6: weekday = 6; break;
                case 7: weekday = 0; break;
                default:
                    ret = 0;
                    return 0;
            }
            APSystem::CActionPointManager::_gmWeekday.m_gmOn = 1;
            APSystem::CActionPointManager::_gmWeekday.m_weekday = weekday;
        }
    }
    // ---- pcroom ----
    else if (args[0] == "pcroom")
    {
        // ORIG pcroom：全局（非 Taiwan 命名空间）Packet_Pcroom_Response，
        // setter 均内联写成员，无独立 call；长度走结构 +0x02 的 len 字段（=0x1f）。
        Packet_Pcroom_Response pkt;
        pkt.m_field12 = 1;                                        // +0x12 local_38c
        pkt.m_field1a = 1;                                        // +0x1a local_384
        pkt.m_field1b = 0xb;                                      // +0x1b local_383
        std::time_t now = std::time(0);
        pkt.m_expireTime = static_cast<int>(now) + 0x93a80;      // +0x16 local_388
        pkt.m_uid = static_cast<int>(user->GetUID());            // +0x0a local_394
        pkt.m_accId = user->get_acc_id();                        // +0x0e local_390
        {
            Stream* s = GlobalData::s_stream_pool->Acquire(
                "PacketDispatcher_Impl_1.cpp", 0xbc0a);
            CStreamGuard g(s, true);
            (*g)->operator<<(0x17e);
            (*g)->operator<<(pkt.m_uid);                                       // ORIG: local_394 (复用已存 GetUID)
            g->put_binary(reinterpret_cast<char*>(&pkt),
                          static_cast<int>(sizeof(pkt)));         // ORIG len=local_39c=0x1f
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), g);
        }
    }
    // ---- avatarcoin ----
    else if (args[0] == "avatarcoin")
    {
        unsigned int v = static_cast<unsigned int>(std::atoi(args[1].c_str()));
        // ORIG 0x08212a36-0x08212a6c：**无** coinR/coinW/total 具名槽（P0/P183/P329）：
        //   R 读侧 `add $0x658,%eax; mov %eax,(%esp); call AvatarCoin::Get()` 直入实参，
        //   结果 `add %eax,-0x44(%ebp)` 就地在 v 上累加；W 写侧 `lea 0x658(%eax),%edx`
        //   亦无槽直传 Set。
        v += user->getCurCharacInvenR()->m_avatarCoin.Get();  // +0x658（ORIG 用 R 读）
        user->getCurCharacInvenW()->m_avatarCoin.Set(v);      // +0x658
        AvatarCoin::SaveToDB(user);
        AvatarCoin::SendSyncPacket(user);
    }
    // ---- avr sorneh/ghkrfbf ----
    else if (args[0] == "avr")
    {
        if (args[1] == "sorneh")
        {
            short a = static_cast<short>(std::atoi(args[2].c_str()));
            short b = static_cast<short>(std::atoi(args[3].c_str()));
            // ORIG 0x08212b42：GmReduceEndurance 为 static，直呼三参无 this
            // （不引入 Singleton<>::Get() call，cmp_calls 实证 ORIG 侧无该 call）。
            AvatarRechargeServer::GmReduceEndurance(user, a, b);
        }
        else if (args[1] == "ghkrfbf")
        {
            if (args[2] == "on")
                AvatarRechargeServer::GmChangeEnduranceReduceRate(
                    user, true, std::atoi(args[3].c_str()));
            else if (args[2] == "off")
                AvatarRechargeServer::GmChangeEnduranceReduceRate(user, false, 0);
        }
    }
    // ---- startva ----
    else if (args[0] == "startva")
    {
        if (user != 0)
        {
            Packet_Game_Monitor_GM_Village_Attacked pkt;
            pkt.m_flag = 1;                           // +0x0a（ORIG local_3b5）
            pkt.m_a = std::atoi(args[1].c_str());        // +0x0c（ORIG local_3b4）
            pkt.m_b = std::atoi(args[2].c_str());        // +0x10（ORIG local_3b0）
            pkt.m_c = std::atoi(args[3].c_str());        // +0x14（ORIG local_3ac）
            CMonitorServerProxy* proxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x17);
        }
    }
    // ---- SetAvengerReady ----
    else if (args[0] == "SetAvengerReady")
    {
        int v = std::atoi(args[1].c_str());
        ARAD::Arad_DataManager* dm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        ARAD::Arad_EventPeriodDataManager* period =
            static_cast<ARAD::Arad_EventPeriodDataManager*>(
                // ORIG：std::string 直接就地构造进 by-value 形参槽（无 copy ctor），
                // 故内联临时，避免额外 _ZNSsC1ERKSs 与其析构。
                dm->findGameScript(std::string("Arad_EventPeriodDataManager")));
        period->setCreatePeriod_GM(v);
    }
    // ---- SetCreateTime ----
    else if (args[0] == "SetCreateTime")
    {
        if (args.size() < 5)
        {
            return 0;
        }
        {
            // ORIG 无 4×`movw $0x0`（4 条）——4 个元素随后全被赋值，初始化冗余（P0 族）。
            short fields[4];
            fields[0] = static_cast<short>(std::atoi(args[1].c_str()));
            fields[1] = static_cast<short>(std::atoi(args[2].c_str()));
            fields[2] = static_cast<short>(std::atoi(args[3].c_str()));
            fields[3] = static_cast<short>(std::atoi(args[4].c_str()));
            long long t = ARAD::uint32Time(fields);
            user->set_charac_createtime(t);
            arad::SigSetCharacCreateTime sig;
            unsigned int createTime = user->getCurCharacCreateTime();
            unsigned int characNo = user->get_charac_no(-1);
            sig.set(characNo, createTime);
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xbc83);
            CStreamGuard g(s, true);
            unsigned int uid = user->GetUID();
            ARAD::DISPATCHER::make_internal_stream_jpn(
                g, ARAD::ENUM_INTERNALPACKET_JPN(0x24), static_cast<int>(uid));
            g->put_binary(reinterpret_cast<char*>(&sig), 8);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
    }
    // ---- aa time/star/ticket/reset ----
    else if (args[0] == "aa")
    {
        advancealtar::CharacAdvanceAltarManager* mgr =
            &user->m_advanceAltar;                     // +0x8df60
        if (args[1] == "time")
        {
            int v = std::atoi(args[2].c_str());
            if (0 < v && v < 0x33)
                advancealtar::Manager::gmTimeSpeed_ = v;
        }
        else if (args[1] == "star")
        {
            int v = std::atoi(args[2].c_str());
            if (-1 < v && v < 1000)
                mgr->gmSetStar(v);
        }
        else if (args[1] == "ticket")
        {
            int v = std::atoi(args[2].c_str());
            if (-1 < v && v < 100)
                mgr->gmSetTicket(v);
        }
        else if (args[1] == "reset" && args[2] == "star")
        {
            int v = std::atoi(args[3].c_str());
            if (-1 < v && v < 2)
                mgr->resetStar(user, static_cast<CMDPacketStruct::ResetStarType::T>(v));
        }
    }
    // ---- SetRewardLast / SetQuestLast / SetBreak ----
    else if (args[0] == "SetRewardLast")
    {
        int v = StringToNumber(args[1].c_str());
        CLocalChina_DB_LastDay_BreakAway::makeRequest(user->get_acc_id(), v, 1);
    }
    else if (args[0] == "SetQuestLast")
    {
        int v = StringToNumber(args[1].c_str());
        CLocalChina_DB_LastDay_BreakAway::makeRequest(user->get_acc_id(), v, 0);
    }
    else if (args[0] == "SetBreak")
    {
        int v = StringToNumber(args[1].c_str());
        GlobalData::s_BreakAwaySys->GM_UpdateUvList(
            user->get_acc_id(), static_cast<unsigned int>(v));
    }

    return 0;
}