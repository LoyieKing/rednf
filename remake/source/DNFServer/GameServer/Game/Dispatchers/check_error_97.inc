unsigned int Dispatcher_GuildCargoPopItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        int guildkey = user->get_charac_guildkey();     // ORIG 0x080da38c 后链
        if (guildkey != 0)
        {
            // X1 位 A：ORIG 0x082301d8 此处是 `test;sete %al;test %al,%al;je` 物化岛
            // （台账 ours_only 缺 sete/test），bool callee 用 (x == 0) 才出 sete；
            // 裸 !boolcall() 反而出 xor 单段（技巧9/19 与技巧1 的分界）。
            if ((bool)(user->IsExistGuildAgit() == 0))
            {
                return 0xb5;
            }
            {
                int secu = GlobalData::s_pSecuProtectionField->Check(
                    user, (SECURITY_PROTCTION)9);       // ORIG 0x08288a02
                if (secu != 0)
                    return secu;
                // 位 C：ORIG 0x081d16ex 是 `mov 0x10(%ebp),%eax; mov 0xf(%eax),%ebx`
                // —— packed 具名视图给单级 disp（技巧14/F），register 绑 %ebx 同时把
                // 值钉进 callee-saved（消掉我方裸偏移写法的 add+mov 三步，并让
                // prologue 回到 ORIG 的 `push %ebx; sub $0x24,%esp`）。
                struct __attribute__((packed)) CargoPopParamView {
                    char m_head[0xf];
                    int m_itemIdx;                   // +0x0f
                };
                int itemIdx =
                    reinterpret_cast<CargoPopParamView*>(&param)->m_itemIdx;
                const CInventory* inven = user->getCurCharacInvenR();
                // X1 位 B：ORIG 0x0850562a 之后是 `test %eax,%eax; jne` 直分支
                // （int callee 免物化），我方 `!intcall()` 恒被折成 sete 岛 ⇒ 写 == 0。
                if (inven->possible_to_insert(itemIdx) == 0)
                    return 0xcb;
                if (G_CDataManager()->find_item(itemIdx) == 0)   // ORIG 0x0835fa32
                    return 0xc5;
                return 0;
            }
        }
        return 100;
    }
    return -1;
