bool GameWorld::move_area(CUser* user, int vill, int area, int x, int y, int z,
                          bool flag, int param, int prevVill, int prevArea)
{
    // §6.2 巨件核销：P167 127/127 [0缺0多]。逐块人工比对完成，语义零缺口。
    // 真修四处：①teleport_state==1 比较物化；②elapsed 具名局部（P0 槽 -0x2c）；
    // ③buf[5] 清零 movl+movb（P202①）；④getCurCharacVillPrev 返回 signed char
    // （ORIG 调用点 movsbl，头已同步）。
    PacketGuard errorPacket;

    if (!GlobalData::s_timeGate_->isOpen() &&
        vill == 0xb && area == 5)
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0x13);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (user->IsBusyDisjointStore())
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0xbf);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0x3c);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (user->IsCurCharacGhost() && !GlobalData::s_power_manager->IsPowerWarEventOn() &&
        user->getCurCharacVill() != 7)
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0x13);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (!user->isGMUser() &&
        IsPowerWarSafeZone(vill, area, (ENUM_POWER_SIDE_TYPE)user->getPowerSide()))
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(4);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }

    int curVill = (int)user->getCurCharacVill();
    int curArea = user->get_area(false);
    bool teleport = false;
    CParty* party = (CParty*)user->GetParty();
    // ORIG @0x086c5ea2：party!=0 守卫后内层 state==1（char 比较物化 sete 为
    // 4.1 特有，4.4 直 jne，N 类）。
    if (party != NULL)
    {
        if (((CPartyTelePort*)party->GetPartyTelePort())->get_teleport_state() == 1)
        {
            teleport = true;
        }
    }
    if (param == 2)
    {
        teleport = true;
    }
    if (!teleport && vill == curVill && area == curArea)
    {
        return false;
    }

    PacketGuard packet;
    if (!check_valid_area(vill, area) || !check_valid_area(curVill, curArea))
    {
        packet.put_header(1, 0x26);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.put_byte(0);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    Village* destVillage = NULL;
    // ORIG 0x086c5xxx 段：`mov %eax,-0x48; cmpl $0,-0x48; je` ⇒ 赋值与判零拆成两句
    // （&& 链内赋值使 4.4 多发 mov $1/$0 物化 4 条）。
    if (-1 < vill && vill < MAX_VILLAGE_NUM)
    {
        destVillage = &m_pVill[vill];
        if (destVillage != NULL)
        {
            // ORIG `movl $0x0,-0x38`（P198① 预清）+ `cmpl $0,-0x38; setne %al;
            // test %al,%al; je`（P225 while+break 复现 bool 四段物化）。
            int moveTown = 0;
            moveTown = user->CheckMoveTown(vill);
            while (moveTown != 0)
            {
                packet.put_header(1, 0x26);
                packet.put_byte(0);
                packet.put_byte(moveTown);
                packet.put_byte(vill);
                packet.finalize(true);
                user->Send(packet);
                return false;
            }
        }
    }

    Village* curVillage = NULL;
    if (-1 < curVill && curVill < MAX_VILLAGE_NUM)
    {
        curVillage = &m_pVill[curVill];
    }
    int prevVill2 = (int)user->getCurCharacVillPrev();
    int prevArea2 = user->get_before_area();
    user->set_position((unsigned short)x, (unsigned short)y, (char)z);
    user->setCurCharacVill((char)vill);
    user->set_area(area);

    if (curVillage != NULL)
    {
        Area* curAreaObj = curVillage->getArea(curArea);
        if (flag && !IsPVPChannel())
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            // ORIG 0x086c61b5/0x086c61cd（P0/P48 去缓存）：条件臂与体各独立调用
            // getBeforeAreaTime() 一次，不缓存到具名局部（缓存则少 1 call 点，P167 差集 -1）。
            if (user->getBeforeAreaTime() != 0)
            {
                // ORIG @0x086c61d7..：差值落 -0x2c 槽（P0 ⇒ 具名局部 elapsed），
                // cmpl $0x2 后 jg 走失败汇聚（&& 短路链首条件）。
                int elapsed = now - user->getBeforeAreaTime();
                if (elapsed <= 2 &&
                    CHackLog_InvalidAreaMove::IsInvalidMovableArea(
                        curVill, curArea, vill, area) &&
                    !CHackLog_InvalidAreaMove::isMovableRoute(
                        prevVill2, prevArea2, curVill, curArea, vill, area) &&
                    prevVill2 != vill && prevArea2 != area)
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACK_TYPE)0x1fc, 1, 0, 0);
                }
            }
            user->setBeforeAreaTime(now);

            if (param == 0)
            {
                if (!curAreaObj->IsMovalbeArea(vill, area) &&
                    CHackLog_InvalidAreaMove::IsInvalidMovableArea(
                        curVill, curArea, vill, area) &&
                    curVill == prevVill && curArea == prevArea)
                {
                    // ORIG 0x86c6352：先记日志（User acc_id 与四区坐标），
                    // 再把 (prevVill,prevArea)/(vill,area) 各拼成 "%d%d" 字符串
                    // 经 atoi 还原后作为 hack 计数参数（数值拼接怪癖）。
                    LogManager::logFormat(
                        1, "world.cpp", __PRETTY_FUNCTION__, 0x832,
                        "move area error. User:%s (%d,%d)->(%d,%d)",
                        NumberToString(user->get_acc_id(), 0), curVill,
                        curArea, vill, area);
                    char buf[5] = {0};
                    int hackA = 0;
                    int hackB = 0;
                    if (prevVill >= 0 && prevVill <= 10 &&
                        prevArea >= 0 && prevArea <= 10)
                    {
                        sprintf(buf, "%d%d", prevVill, prevArea);
                        hackA = atoi(buf);
                        memset(buf, 0, 5);
                    }
                    if (vill >= 0 && vill <= 10 && area >= 0 && area <= 10)
                    {
                        sprintf(buf, "%d%d", vill, area);
                        hackB = atoi(buf);
                    }
                    user->getHackAnalyzer()->addServerHackCnt(
                        user,
                        (WongWork::ENUM_HACK_TYPE)CHackLog_InvalidAreaMove::GetHackType(),
                        1, hackA, hackB);
                }
            }
        }
        curAreaObj->delete_user(user);
    }

    if (destVillage != NULL)
    {
        int destArea = area;
        // ORIG 0x086c6a1d..（P167 差集 -1 G_GameWorld）：ORIG 经 G_GameWorld() 取
        // 单例后再调 IsSchoolPvPChannel()，而非直接用 this —— 忠实保留跨调用形。
        if (G_GameWorld()->IsSchoolPvPChannel() && vill == 2 && area == 4)
        {
            // ORIG 0x086c64e5 `call CUser::GetSchoolNo; mov %eax,0x4(%esp);
            // movl $&g_schoolMgr,(%esp); call CSchoolMgr::GetSchoolArea` ⇒ 学号
            // 来自 GetSchoolNo() 返回值（原实现丢弃返回值传 0 = 真语义 bug）。
            destArea = g_schoolMgr.GetSchoolArea(user->GetSchoolNo());
            // ORIG 0x086c6a1d 段 `js <neg 块>`：负值跳走、非负直落 set_area ⇒
            // 原码按 `>= 0` 写（P194/P222 正极性包裹）；`< 0` 反产 `jns`。
            if (destArea >= 0)
            {
                user->set_area(destArea);
                user->set_trick_area(area);
            }
            else
            {
                destArea = area;
                user->set_trick_area(-1);
            }
        }
        else if (vill == 8 && (area == 0 || area == 1 ||
                               area == 2 || area == 3))
        {
            // ORIG 0x086c6ab0..（P0/P48 去缓存 + 循环内重调形）：area 4 值各成一
            // 独立 if-branch，每支各调 get_charac_guildkey() + GetGuildAgitAreaIndex()
            // 一次；cMyTrace 的 Guild(%d) 实参第 5 次独立调用 get_charac_guildkey()
            // （不缓存到具名局部）。P167 差集 -4/-3 根因。
            // [b28d-sweep] g_guildAgitMgr 为对象（ORIG B 0x58）⇒ 值语义调用；ORIG
            // 0x86c658d/0x86c65be/0x86c65ec/0x86c661a 全为 `movl $0x943e0e0,(%esp)`。
            if (area == 0)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 0);
            else if (area == 1)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 1);
            else if (area == 2)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 2);
            else if (area == 3)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 3);
            cMyTrace(__PRETTY_FUNCTION__, 0x88e, 0)(
                "Move Guild Agit Area User(%d) Guild(%d) dest area(%d), virtual area(%d) ",
                user->getCurCharacNo(), user->get_charac_guildkey(), area, destArea);
            // 同 2699 段：ORIG `js` 负值跳走 ⇒ 源按 `>= 0` 正极性写。
            if (destArea >= 0)
            {
                user->set_area(destArea);
                user->set_trick_area(area);
            }
            else
            {
                destArea = area;
                user->set_trick_area(-1);
            }
        }
        else
        {
            user->set_trick_area(-1);
        }
        Area* destAreaObj = destVillage->getArea(destArea);
        destAreaObj->insert_user(user);
    }

    // ORIG 0x86c6710：MoveArea(curVill, curArea, vill, area, param)
    // （汇编实参：+0x4=curVill、+0x8=curArea、+0xc=vill(0x10ebp)、
    //   +0x10=area(0x14ebp)、+0x14=param(0x28ebp)；Ghidra C 掉第 5 实参，
    //   e 在实现中未使用，但调用侧必须按原寄存器序列补齐）。
    user->m_historyLog.MoveArea(curVill, curArea, vill, area, param);
    return true;
