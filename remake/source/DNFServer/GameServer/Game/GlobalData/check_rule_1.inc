// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x08283c5a
// [b28d-solo-GlobalData] 归零 AE 85=85（p167 11/11 CLEAR）：①各失败点 ORIG 为
// `mov $imm,%eax; jmp end` 直接物化 ⇒ 源码全 `return X;` 早返（旧 result 变量
// 汇聚形多 -0x1c 槽 + 4 条）；②pvp/expired 两处零槽短路物化用 P226 语句表达式
// `if (({ A || B; }))`（具名 bool 落槽 -0x16/-0xd）；③remain 声明在 if 外、
// 赋值在 if 体内（ORIG 0x08283cfb 在 isGMUser 之后）；④CUser.h is_equip_aura_avatar
// 返回 char→int（Main 代落，消调用点 movsbl）。
int Teleport::check_rule(CUser* user)
{
    // ORIG 0x08283c5a：状态/渠道/vill/aura 冷却校验链（报告 Teleport/check_rule.md）。
    // [b28d-solo-GlobalData] ①各失败点 ORIG 为 `mov $imm,%eax; jmp end` 直接
    // 物化 ⇒ 源码全 `return X;` 早返（Ghidra 的 iVar3 汇聚是假象，旧 result
    // 赋值形多 -0x1c 槽）；②pvp/expired 两处为 `mov $1/$0` 岛 + test 直测
    // （零槽，P226 语句表达式）——具名 bool 会落槽 -0x16/-0xd 并顶浅后续槽。
    if (user == 0)
    {
        return 1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (({ G_GameWorld()->IsPVPChannel() ||
           G_GameWorld()->IsPvPSkilTreeChannel(); }))
    {
        return 0x13;
    }
    if (user->getCurCharacVill() == 7)
    {
        return 0xbe;
    }
    // [b28d-solo-GlobalData] ORIG 0x08283cfb `movl $0,-0x14(%ebp)` 在
    // isGMUser 检查之后（Ghidra C 亦为 if 体内 local_18 = 0）⇒ 声明在 if 外、
    // 赋值在 if 体内（声明即初始化会提前到 0x08283ce9 之前 +1 条）。
    int remain;
    if (!user->isGMUser())
    {
        remain = 0;
        int equipped = (int)user->is_equip_aura_avatar(2, remain);
        if (equipped > 0)
        {
            return equipped;
        }
    }
    int auraEnd = user->get_aura_avatar_option_value(2);
    if (({ (!user->isGMUser()) &&
           (auraEnd > GlobalData::s_systemTime_.getCurSec()); }))
    {
        return 0x16;
    }
    return 0;
}