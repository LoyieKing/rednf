// [b28d-11b] 修法：memset→char path[256]={0} 复现 4.1 rep stos（P214③）56→45；残余 N80 变参求值序/具名局部 @0x086c9638
// [b28d-solo-GameWorld] _ZN9GameWorld15WriteLogChatMsgEP5CUsercPc verdict=NEAR 75=75
// 助记符全等，仅 N82/P112 槽分配差：ORIG path=-0x120/log=-0x20/state=-0x134/pvpIdx=-0x130；
// 4.4.6 path=-0x128/log=-0x28/state=-0x20/pvpIdx=-0x1c（标量区与聚合对象区方向相反，声明序不可翻）。
// [L11·round1 追加实测（判据⑩边界）] 两形均零效（复编复测 ndiff 恒 10）：
//   ① `int state; int pvpIdx;` 前置到 `char path[256]` 之前 + 赋值留原位（纯声明重排）；
//   ② 声明/赋值分离（`int state; … state = user->get_state();`）。
// ⇒ 本件 4.4 按**首次使用序**分配：`char path[256]={0}` 的 rep stos 是最早使用点 ⇒
// 占最深槽，标量只能在浅区；ORIG 4.1 按类型分组（标量浅/聚合深）相反 ⇒ 判 N（P112/N98）。
// [H8B 2026-09-11 复核·终判] 75/75 等长 + p167 仅 CMyFileLog C1/C2 别名 ⇒ 无真缺口。
// diff 唯一非别名差 = 槽位排布：ORIG path(-0x120，0x086c96a6 `rep stos` 0x40 dwords) 与
// state/pvpIdx(-0x134/-0x130，0x086c96c6/c6d) 相对 **buf 更深**；我方 path -0x128、
// state/pvpIdx -0x20/-0x1c（buf **更浅**）+ 第 2 实参装载 ORIG 0x086c96f9
// `mov -0x130(%ebp),%ecx` vs 我方 %eax 复用。P331 两向已尽：当前源码即「buf 声明在前、
// 标量在后」（父建议方向），反方向（标量声明前置）b28d-solo 实测槽位逐值不变（4.4 恒按
// 首次使用序，N98）；且 get_state 调用序受 ORIG 反证约束（0x086c96b5 在 sprintf 之后）
// 不可提前 ⇒ N82/N126/N98 封顶。
void GameWorld::WriteLogChatMsg(CUser* user, char type, char* msg)
{
    // [b28d-solo-GameWorld] _ZN9GameWorld15WriteLogChatMsgEP5CUsercPc：ORIG 0x086c964d
    // `call G_GameWorld; mov %eax,(%esp); call GetChannelType`（两段链，非成员直调）；
    // 0x086c965a `cmp $6; sete %al; test; je` 需拆嵌套 if 让 4.4 对寄存器直比物化。
    if (G_GameWorld()->GetChannelType() == 6)
    {
        if (user != 0)
        {
            // [b28d-solo-GameWorld] P197③/P28 已试：标量先声明（无初始化）+ 后赋值
            // ⇒ 4.4 读数逐值不变（仍 path=-0x128/log=-0x28/state=-0x20/pvpIdx=-0x1c）
            // ⇒ 4.4 槽按首次使用序 + 聚合对象优先深槽（path/log 深、标量浅），
            // 与 ORIG 4.1 声明序（state/pvpIdx 最深）方向相反，N82/P112 封顶。
            // b28d-solo 复验：把 state/pvpIdx 声明前置到 path 之前（赋值仍留在
            // sprintf 之后）实测槽位逐值不变（state=-0x20/pvpIdx=-0x1c）
            // ⇒ 4.4 恒按首次使用序分配（N98），声明序旋钮无效。
            // ORIG 0x086c96b5/0x086c96c6 两句前置具名槽：state=-0x134、pvpIdx(cwtl)=-0x130。
            char path[256] = {0};
            sprintf(path, "./log/%s/GuildWarChat",
                    G_CEnvironment()->get_file_name());
            int state = user->get_state();
            int pvpIdx = (int)user->GetPvpIndex();
            // ORIG ctor 在全部 getter 之后 ⇒ P227 全表达式临时链。
            CMyFileLog(__PRETTY_FUNCTION__, 0xd6d)(path,
                "T:%d\tMsg:%s\tC_Name:%s\tG_Key:%d\tR_Idx:%d\tState:%d",
                (int)type, msg, user->getCurCharacName(), user->get_charac_guildkey(),
                pvpIdx, state);
        }
    }
}