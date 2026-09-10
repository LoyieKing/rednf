void CGameManager::user_disconnect(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=155 our=154 gap=1；ndiff 55；P227 临时链
    // 已上无改善——主体为分支内局部槽组差）。ORIG 0x08298b48。
    // b28d-solo-CGameManager 判词：N82 维持（P167 CLEAR；残差为分支内 cMyTrace/
    // 临时对象槽组分区差）。
    // b28d-solo-CGameManager 真语义 bug 修正（ORIG 0x08298d54/0x08298da1→0x08298da6）：
    //   SendLastMsgDBQueue 是**两分支共享尾部**——state<3 短链末尾 returnUserPool 后
    //   fall-through 到 0x08298da6，state>=3 长链末尾 `jmp 0x08298da6`；原重建把它
    //   写在短链内 ⇒ state>=3 路径漏发。同时改 `state > 2` 正向守卫复现 ORIG
    //   `cmp $2; setg; je <短链>`（原 `< 3` 产 setle + 反向块序）。
    // b28d-solo-CGameManager 第二处真语义 bug（ORIG 0x08298c25 `movl $0x1,0x8(%esp)`）：
    //   长链 CheckOutParty 第二实参为 **true**（原重建写 false；user_exit 同位置已是 true）。
    if (user == 0)
    {
        // b28d-lane12: P227 全表达式临时链（消 N80 具名 ctor 前求值差）。
        cMyTrace(__PRETTY_FUNCTION__, 0x138b, 5)(
            "USER DISCONNECT [%s][%d]", __PRETTY_FUNCTION__, 0x138b);
        return;
    }
    HistoryLog::LogClose((FILE*)user->m_field796f8);
    // ORIG 0x08298bbb：`test %eax,%eax; sete %al; test %al,%al; jne 08298dbc`（08298dbc
    // 为 nop 空岛）——4.1 对 != 的 sete 物化形；4.4 对 if(!=0) 恒产 setne+je。
    // 用 while(x==0){return;} 复现 sete+jne 物化与尾 nop 岛（P225）。
    while (user->get_state() == 0)
    {
        return;
    }
    {
        user->prepareDisconnect();
        G_GameWorld()->EraseLoginUser(user);
        user->SetSaveRentalInfoToExchange(false);
        if (user->get_state() > 2)
        {
            CheckOutTrade(user);
            CheckOutParty(user, true);
            CheckOutPvp(user, false);
            CheckOutWarRoom(user);
            checkOutDeathTower(user);
            checkOutBossTower(user);
            checkOutAdvanceAltar(user);
            checkOutBlueMarble(user);
            G_GameWorld()->leave_game_world(user);
            user->SetGameMasterMode(false);
            user->UpdateData();
            user->LogoutCachedCharacter(0);
            user->UpdateLogout(true);
            user->DeleteRentalItemInfo(1);
            user->DeleteRentalItemInfo(2);
            user->ResetCurCharac();
            user->doLinkCharacDisconnect();
            returnUserPool(user);
        }
        else
        {
            user->SetSaveRentalInfoToExchange(false);
            user->LogoutCachedCharacter(0);
            user->UpdateLogout(true);
            returnUserPool(user);
        }
        // ORIG 0x08298da6：两分支共享尾部（短链 fall-through / 长链 jmp 同址）
        GetInstanceShutdowManager()->SendLastMsgDBQueue(user);
