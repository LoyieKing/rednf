// ---------------------------------------------------------------------------
// ORIG T 0x82ed43e (size 0x1b5)：RevivalUser(CUser*, int&)
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF 封 N（N104，P321-T5）：`||` 直接物化 +
//   uid 内联（P0）后逐条对齐，仅剩 N104 EH 尾块相对序——ORIG 正常路径 PacketGuard
//   D1+epilogue 在函数最末、unwind 块居中；4.4 把 unwind 块压最末、正常路径尾多
//   `jmp` 跳过（块内容逐条全等，仅两尾块互换，ndiff 17 系 difflib rotate 放大）。
//   p167 CLEAR。ORIG 0x082ed43e
void CAssaultPlace::RevivalUser(CUser* user, int& result)
{
    if (!((CUserCharacInfo*)user)->IsCurCharacGhost())
        throw (ENUM_ERROR)0x12;

    int nowSec = (int)GlobalData::s_systemTime_.getCurSec();

    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ed493 段：`||` 直接物化
    //   `mov $1/%eax ... mov $0/%eax; test; je`（P0 无具名 bool 槽；具名 char
    //   会多 movb 物化并把 nowSec 槽推到 -0x14、PacketGuard 推到 -0x20）。
    if (GlobalData::s_power_manager->IsPowerWarEventOn()
        || ((CUserCharacInfo*)user)->getCurCharacVill() == 7)
    {
        if (!GlobalData::s_power_manager->IsAbleToRevival(user,
                                                          (unsigned int)nowSec,
                                                          result))
            throw (ENUM_ERROR)0x16;

    } else {
        int dieDiff =
            nowSec - ((CUserCharacInfo*)user)->GetCurCharacChaosDieTime();
        if (!(dieDiff > 0x257)) {  // <=599
            result = 0x258 - dieDiff;
            throw (ENUM_ERROR)0x16;
        }
    }

    PacketGuard packet;
    // ORIG 0x082ed58d `movzwl %ax,%eax` 直传（P0：无 uid 具名槽）。
    _MakeRevivalAssaultPlayer(&packet, (int)((CUser*)user)->get_unique_id());
    G_GameWorld()->send_all(packet);
    ((CUserCharacInfo*)user)->SetCurCharacGhost(false);
}