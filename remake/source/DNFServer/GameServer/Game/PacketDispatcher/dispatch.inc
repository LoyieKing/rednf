// ORIG 0x8594e52 逐指令转写。
int PacketDispatcher::dispatch(int userId, char* data, int len)
{
    // b28d-solo-S161 判词 [ORIG 0x8594e52]：**AE（DIFF→AE，本泳道收口）**，orig 114 /
    //   our 114 insn，p167 callsite 13/13 CLEAR（0 missing / 0 extra）。前泳道 S153 残差
    //   三项已全部消除（各附 4.4.7/4.4.6 双后端 + 探针 /tmp/pd_probe6.cpp、probe7.cpp 实证）：
    //   ①N18a「取函数指针 4.4 恒两段」：`void***` 下标形（`(*reinterpret_cast<void***>(p))[0x3c/4]`）
    //     恒产 `mov (%eax),%eax; mov %eax,%esi`；改 **单层解引用 + char* 偏移 + 纯 `register`
    //     存储类提示** `register CheckFn fn = *reinterpret_cast<CheckFn*>((char*)*(void**)p + 0x3c);`
    //     即逐条复现 ORIG 0x8594ec9 `mov (%eax),%esi` 单段直落（探针 t3/t6 对照 t8）。纯 register
    //     非 asm 绑定，P0/P231 合规。
    //   ②N7「key/checker 寄存器角色互换」：ORIG 0x8594eda `mov %eax,%edx` 中转 ⇒ CheckFn 第 2
    //     形参须 `unsigned int`（`(unsigned int)user->getAntibotKey()`），P211 档① 全链同型化。
    //   ③doDispatch 实参装载序（N7/P75）：ORIG 0x8594f65 `mov -0x14(%ebp),%eax` 把 packetClass
    //     载入排在 cmd 压栈**之后**；`int packetClass` 形恒提前载入并挤掉 cmd 的 %eax。把局部声明
    //     改 `ENUM_PACKETCLASS packetClass`（实参无转换）即复现 ORIG 序。
    //   本轮按 ORIG 逐条重排源形：①packetClass 由 `(signed char)data[0]` 初始化
    //   （复现 0x8594e5d movzbl+movsbl 两段 + 存 -0x14 槽）；②cmd 声明 short
    //   （复现 0x8594ecb movswl -0xe 单段与 0x8594f02 movzwl+movzwl 双段传参）；
    //   ③checker/fn 去命名局部，函数指针表达式内联（0x8594ec9 mov (%eax),%esi 直取）；
    //   ④history 局部删除，`user->m_historyLog.AbnormalUser` 直算 add $0x79700
    //   （0x8594f09）；⑤`if(!doDispatch(...))`（0x8594f7e xor $1 形 ⇒ doDispatch 返回 bool）；
    //   ⑥末段 && 链改嵌套 if（0x8594f9e/fad/fbc 三 je 共享出口 0x8594fd4）；
    //   ⑦S11 将 IsCurCharacLeaveTimeout 改 bool 后末段物化归正（+2 行收）。
    //   （①~⑦ 为 S153 已落源形，全部保留。）
    ENUM_PACKETCLASS packetClass = (ENUM_PACKETCLASS)(signed char)data[0];
    short cmd = *(unsigned short*)(data + 1);
    CUser* user = G_CGameManager()->getUser(userId, 5);
    if (user == 0)
        return 0;

    pUserPacketCounter_->IncreasePacket(user->get_acc_id());

    typedef int (*CheckFn)(void*, unsigned int, int);   // s_antibotChecker_ vtbl+0x3c
    // b28d-solo-S161：P231/P307 正例——纯 `register` 提示（非 asm 绑定，P0 合规）
    // + 单层解引用 `*reinterpret_cast<CheckFn*>((char*)*(void**)p + 0x3c)` 形，使 4.4
    // 把函数指针直落 callee-saved（探针 /tmp/pd_probe6.cpp t6 `mov (%eax),%ebx` 单段 +
    // 免槽；t8 的 `void***` 下标形恒两段），复现 ORIG 0x8594ec9 `mov (%eax),%esi` 直落。
    register CheckFn fn = *reinterpret_cast<CheckFn*>(
        reinterpret_cast<char*>(
            *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) + 0x3c);
    // ORIG 0x8594eda `mov %eax,%edx` 中转 ⇒ key 实参按 unsigned int 形参传递（P211 档①）。
    if (fn(GlobalData::s_antibotChecker_, (unsigned int)user->getAntibotKey(), cmd) != 0)
    {
        user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)0, cmd,
                                        user->getAntibotKey(), 0);
        return 0;
    }

    if (!doDispatch(user, packetClass, cmd, data, len,
                    (ENUM_PACKETCLASS)0, 0))
        return 0;

    if (((CUserCharacInfo*)user)->getCurCharacR() != 0)
    {
        if (((CUserCharacInfo*)user)->IsCurCharacGhost())
        {
            if (((CUserCharacInfo*)user)->IsCurCharacLeaveTimeout())
            {
                ((CUserCharacInfo*)user)->LiveCurCharac();
                pvp_assault::CAssaultPlace::SendUserState(user);
            }
        }
    }
    return 1;
}