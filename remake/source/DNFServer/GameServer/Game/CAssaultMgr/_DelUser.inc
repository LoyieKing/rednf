void CAssaultPlace::_DelUser(int index, bool flag)
{
    // b28d-L3: IDENTICAL_AE — b28d-L3：正向 wrap `if (delUser != 0)` 消函数尾 jmp 中继
    // b28 BL6: N96/P0 清理 register asm 绑定违例，还原自然形。ORIG +0x24 的
    // `cmp $4; setg %al; test %al; je` 无槽物化岛（setcc-fold 负例族）源形
    // 不可造，残差封顶。
    if (flag)
    {
        if (m_assaulters[index].GetState() > (ENUM_ASSAULT_STATE)4)
        {
            // b28 BL6: P0——ORIG 内层 GetUser 无槽往返（4.1 走 %ebx 直缓存，
            // 禁绑定形）⇒ 实参右值直传；嵌套 if 裸形 `cmp $4; setg; test; je`
            // 与 ORIG 同形（f1 探针实证）。
            disconnect_detecter::GetInstanceDisconnectDetecter()->UnregisterUser(
                m_assaulters[index].GetUser());
        }
    }
    int team = m_assaulters[index].GetTeam();
    CUser* delUser = m_assaulters[index].GetUser();
    m_assaulters[index].DetachUser();
    m_field100 -= 1;
    // b28d-L3：ORIG +0xb5 为正向 wrap `if (delUser != 0) {…}`（早退 return 形
    // 在 4.4.6 于函数尾留一条 jmp 中继，ORIG 无）；EndPvP 段用负条件包裹
    // （ORIG 条件成立 `jne` 直跳 EndPvP 之后，goto 岛形多一条尾 jmp）。
    if (delUser != 0)
    {
        if (delUser->getCurCharacR() != 0)
        {
            // ORIG +0x0f2：EndPvP 段 getCurSec 差值落 %edx 跨装载直落 0xc(%esp)
            // （P44 变参即时求值，无 now 槽）；+0x13d G_GameWorld 结果 %eax 直通
            // receiver（P89 合并形，无 world 槽）；area 参数 get_area 结果直落。
            // b28d-L3 实测：负条件包裹（`if (!(A&&B))`）在 4.4.6 产 xor$1+极性
            // 反转（残 7）；goto 尾标签形条件段全等但 body 尾多 1 条 jmp（残 1）。
            // ORIG 布局 = 条件真时 `jne` 跳过 body、body 直落 ⇒ 空 then + else
            // body 形（4.4.6 下 jump-thread 消掉尾 jmp）。
            if (GlobalData::s_power_manager->IsPowerWarEventOn()
                && delUser->getCurCharacVill() == 7)
            {
            }
            else
            {
                delUser->m_historyLog.EndPvP(
                    (ENUM_PVP_HISTORY_REASON)5, m_teamFlag[team],
                    GlobalData::s_systemTime_.getCurSec() - m_abuseSec);
            }

            Village* village = G_GameWorld()->getUserVillage(delUser);
            if (village != 0)
            {
                Area* area = village->getArea(delUser->get_area(false));
                if (area != 0)
                {
                    area->send_area_users(delUser);
                    GetInstanceAssaultMgr()->OnMoveArea(delUser);
                }
            }
            GlobalData::s_villageObjectMgr->on_move_area(delUser);
            GlobalData::s_villageMonsterMgr->OnMoveArea(delUser);
            delUser->SetAssaultPlace(0);
        }
    }
