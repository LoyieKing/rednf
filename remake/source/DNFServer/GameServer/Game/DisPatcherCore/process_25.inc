// ORIG 0x081c6082 T _ZN22DisPatcher_RepairEquip7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=0x19, err&0xff) 返回 -1；err<0：LineFunc(0x1500,pretty,0,acc_id)
// 并返回其值；err==0：G_Store()->repair_equip(user, msg[0xd], (s16)msg+0xe, (u16)msg+0x10,
// [b28d-29] N82：帧差（ORIG sub $0x4c+push edi/esi，4.4 sub $0x34）+ 槽号平移，调用点全等  (ORIG 0x081c6082 insn 83->80)
// 出参 param+0xc) 写 param.m_errNo，成功(==0)时回显 m_8=msg[0xd]、m_a=(u16)msg+0xe；返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 69（ORIG 83 / ours 80），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_RepairEquip::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x19, (unsigned char)(errNo & 0xff));
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x1500,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    MSG_REPAIR_EQUIP& req = *reinterpret_cast<MSG_REPAIR_EQUIP*>(&msg);
    stRepairEquipParam& out = static_cast<stRepairEquipParam&>(param);
    // [G5-DisPatcherCore] P52/P183：ORIG 0x081c61xx 段先把 3 个 msg 字段读入
    // %edi/%esi/%ebx 跨 `call G_Store` 保活，receiver 表达式最后求值 ⇒ 原码无具名
    // `Store* store` 缓存（我方原形把 G_Store 提到语句前，实参退化为 caller-saved）。
    out.m_errNo = G_Store()->repair_equip(user, (char)req.m_0d, req.m_0e, req.m_10,
                                          out.m_c);  // 出参 ushort& @ param+0xc（ORIG asm -0x2c(%ebp)）
    if (out.m_errNo == 0)
    {
        out.m_8 = req.m_0d;                    // movzbl 0xd(%eax) -> mov %dl,0x8
        // [DPCB 实验 2026-09-11] 同型视图轴（M5 CheckHaveHellPartyPassItem 同族直证）：
        // 源字段 `short m_0e` 与目标 `unsigned short m_a` **异型**时 4.4 为值转换插一个
        // 中间临时（`movzwl …,%eax; mov %eax,%edx`），ORIG 0x081c60xx 为单步
        // `movzwl 0xe(%eax),%edx`。改用 pack(1) 同址视图把源读成 unsigned short（同型
        // ⇒ 无转换临时），位型/偏移不变。
        // 取址走 `req`（ORIG 用其槽 -0x20），非形参 `msg`（0x10(%ebp)）：
        // ORIG `mov -0x20(%ebp),%eax; movzwl 0xe(%eax),%edx`。
        out.m_a = static_cast<unsigned short>(req.m_0e);
    }
    return 0;
}
