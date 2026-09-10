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
