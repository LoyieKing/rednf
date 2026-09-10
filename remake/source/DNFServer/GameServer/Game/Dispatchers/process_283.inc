int Dispatcher_Expand_Equipslot_Flag_Update::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user == 0)
    {
        return 1;
    }
    if (user->get_state() < 3)
    {
        return 0x13;
    }
    // [b28c-34] ORIG 0x081d5f4a 段：两引用槽在两道 early-return 之后才初始化
    // （req=msg→-0x14 深、out=param→-0x10 浅）；提到函数头会前移 store 并反槽序。
    MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE& req =
        reinterpret_cast<MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE&>(msg);
    stExpandEquipslotFlagUpdate& out = static_cast<stExpandEquipslotFlagUpdate&>(param);
    // [b28c-34] ORIG 0x081d5f8f `mov %al,-0x9(%ebp); cmpb $0x13,-0x9(%ebp)`：
    // 结果按 char 单字节落槽（原 int 生 movzbl+cmpl 两段）。
    // b28d-D0: result 声明与字段 out.m_4 同为 unsigned char（P211 同型化旋钮）⇒
    //   store 复现 ORIG 0x081d5fa4 LHS-first `mov -0x10(%ebp),%eax; movzbl -0x9,%edx`。
    unsigned char result = user->UpdateEquipslotFlag((char)req.m_flag);
    if (result == 0x13)
    {
        return LineFunc(0x3df3,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    // [b28c-34] ORIG 0x081d5fa4 `mov -0x10(%ebp),%eax; movzbl -0x9(%ebp),%edx;
    // mov %dl,0x4(%eax)` = LHS-first（同型）；显式 (unsigned char) cast 使 4.4
    // 判异型改 RHS-first（先 movzbl）。去 cast 恢复同型。
    out.m_4 = result;
    return 0;
