int Dispatcher_InformNotice_2nd::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e30fa：param+4=check_error；<0 → LineFunc(0x59c7,err,0)；
    // ==0 → GetCharacExpandData(0x13)->handleInformNoticeFlag(msg+0xd)
    // ORIG 0x081e30fa 序言：`mov 0x14(%ebp),%eax; mov %eax,-0x14(%ebp)` ⇒ param 引用
    // 在 check_error **之前**落槽；call 结果无 err 槽（`mov %eax,%edx;
    // mov -0x14(%ebp),%eax; mov %edx,0x4(%eax)` 直存），两判定都读回 p.m_4。
    InformNoticeData2nd& p = static_cast<InformNoticeData2nd&>(param);
    p.m_4 = check_error(user, msg);
    // ORIG 0x081e30fa：`test %eax,%eax; jle <正常路径>; mov $0x0,%eax; jmp <尾>`
    // ⇒ 守卫写成**正极性早返**（>0 直落 return 0，<=0 跳后续）；且后续只留
    // **一个**函数尾 `return 0`（ORIG 尾部仅一条 mov $0，4.4 对内层 return 会
    // 就地物化 + jmp 反多两条）⇒ 删内层 return。
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x59c7,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }
    // ORIG 0x081e30fa：msg 指针槽 -0x10 在 GetCharacExpandData 之前物化，
    // 用点是 `mov -0x10(%ebp),%eax; mov 0xd(%eax),%eax`（单级带 disp 读）
    // ⇒ 具名视图指针 + packed 视图字段（禁 `add $0xd` 两段裸偏移，§4.3）。
    struct __attribute__((packed)) Notice2ndMsgView
    {
        char m_head[0xd];
        unsigned int m_flag;      // +0x0d
    };
    Notice2ndMsgView* const mv = reinterpret_cast<Notice2ndMsgView*>(&msg);
    CCharacInformNotice* notice = reinterpret_cast<CCharacInformNotice*>(
        user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_19));
    if (notice != 0)
    {
        notice->handleInformNoticeFlag(mv->m_flag);
    }
    return 0;
