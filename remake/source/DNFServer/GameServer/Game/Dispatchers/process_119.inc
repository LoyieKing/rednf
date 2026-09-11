// err>0 → 错误包(0x18d,err) 返 -1；err<0 → 错误包(0x18d,0x15)+LineFunc(0x3e2c)；
// 否则对扩展槽视图(#9)执行 EquipslotSwitch(msg+0xf,msg+0xd,msg+0xe)，结果字节
// 回写 param.m_4。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d61be orig/ours=68/72 ndiff-blocks=11
//   帧差 sub $38 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp)
int Dispatcher_Equipslot_Switch::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d,
                                 (unsigned char)(err & 0xff));
        return -1;
    }
    if (err < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d, 0x15);
        return LineFunc(0x3e2c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [G1Dproc 2026-09-11] ORIG 0x081d61be 起 槽序直证：err(-0x18) → req(-0x14) → out(-0x10)
    //   → expand(-0xc)，声明序 = 地址递增，且 req/out 均在两道 guard **之后**；
    //   0x081d6254/0x081d6259 才写 -0x14/-0x10 槽。原源码把 out/req 放在最前并多一个
    //   `char* pReq` 槽（ORIG 无：`mov -0x14(%ebp),%eax; movzbl 0xf(%eax),%eax` 直读引用槽）。
    MSG_EQUIPSLOT_SWITCH& req = reinterpret_cast<MSG_EQUIPSLOT_SWITCH&>(msg);
    stEquipslotSwitch& out = static_cast<stEquipslotSwitch&>(param);
    CExpandEquipslot* expand =
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    out.m_result = (unsigned char)(char)expand->EquipslotSwitch(
        user, (char)req.m_field0f, (char)req.m_field0d, (char)req.m_field0e);

    return 0;
}