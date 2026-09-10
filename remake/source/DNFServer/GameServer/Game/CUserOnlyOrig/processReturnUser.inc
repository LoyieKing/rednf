void CUser::processReturnUser()
{
    // ORIG 0x086??：`test %al,%al; je <T>` 无 xor ⇒ 正向直落形
    if (IsReturnUser()) {
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x12d);
    packet.put_int(GetReturnUserExpireTime()
                   - GlobalData::s_systemTime_.getCurSec());
    packet.finalize(true);
    Send(packet);

    if (IsReturnUserFirstLogin()) {
        GiveItemsReturnUser();
        GiveEquipItemsReturnUser();
        // ORIG 0x08691f88-0x08691fb9：右→左实参求值——IsReturnUserFirstLogin→`movzbl %al,%esi`、
        // GetReturnUserExpireTime→`mov %eax,%ebx`、get_acc_id 最后；三栈槽 mov %esi/%ebx/%eax。
        // bool 型绑 %esi（全等直落单条装载），int 绑 %ebx，保跨 call 无槽。
        // ORIG 0x08691f93 `movzbl %al,%esi`：4.4 恒物化两段（movzbl %al,%eax; mov %eax,%esi，
        // probe p6.h2 实证），单段直落 callee-saved 为 N18a 变体；bool 绑定少一条残差。
        // P0/N96：实参内联 R2L 求值序保真（b28 fresh AE）。
        DB_UpdateReturnUserFirstLogin::makeRequest(
            get_acc_id(),
            GetReturnUserExpireTime(),
            (int)IsReturnUserFirstLogin());
        SetReturnUserFirstUserLogin(false);
    }
    }
