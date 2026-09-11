int CDeathTower::handleStartGame(int dungeonIdx, char flag, ENUM_DUNGEON_TYPE dungeonType)
{
    // [b28d-solo-CDeathTower] verdict=DIFF（orig 440 / our 441，ndiff 5）：P167 81/81 CLEAR。
    // 本轮修 P225：`if (cond != 0) throw cond;` 改 `while (cond != 0) { throw cond; break; }`
    // 复现 ORIG 0x08463a48 `cmpl $0; setne %al; test %al,%al; je` 四段物化（4.4 对 if
    // 恒折直跳）。残余 1 处 = EnterDungeon 前置调用实参装载序（ORIG 值先中转 edx/this 后载
    // vs 4.4 this 先，P75/N7 跨版本）⇒ N 封顶。
    // [b28d-solo-CDeathTower2 复核] fresh 读数 orig 440 / our 441；该唯一残差为
    // `m_party->choose_random_buff(bf.getRandomBuffDungeonType(0))` 调用点：ORIG
    // 0x08463cfe 先读 this->m_party 到 %edx、结果留 %eax；4.4 先 `mov %eax,%edx`
    // 保存结果再读 receiver（+1 条）⇒ 纯 callee/caller 寄存器角色互换（N7，
    // P230 后端敏感族；4.4.6 可翻，改路由归 Main）。p167 81/81 CLEAR（差集全
    // 为 PacketGuard/CPacketHandler 的 C1/C2、D1/D2 同址别名，P93 伪差）。
    // [B24D判词 N] 语义正确：try/catch(int)+throw 8/0x16/cond/0x15/1+
    //   reset+initDungeonMgr+_checkStartGameCondition+stage/map 链全对；
    //   P167 调用点差集 81/81 CLEAR；diff=地址占位+EH typeinfo+个别 lowering。
    // ORIG 0x846398e：整体 try/catch(int)；checkFlag 槽 -0x21 于函数头清零
    // （0x846399c movb $0 早于首 if）；cond -0x20 先 `movl $0`（0x8463a28）再赋值。
    // [J10 2026-09-11 复核] orig 441 / our 442（gap −1）；p167 81/81 CLEAR（全为
    // PacketGuard/CPacketHandler 的 C1↔C2、D1↔D2 同址别名）。唯一残差 =
    // choose_random_buff 调用点 ORIG 0x08463cfe `mov 0x8(%ebp),%edx; mov (%edx),%edx;
    // mov %eax,0x4(%esp); mov %edx,(%esp)` vs 4.4.6 `mov %eax,%edx; mov 0x8(%ebp),%eax;
    // mov (%eax),%eax; mov %edx,0x4(%esp); mov %eax,(%esp)`（+1 regmove）⇒ N7 后端族
    // （本 TU 已走 446 后端，故判为 4.4.6↔4.1 寄存器角色差，非路由可解）；
    // 其余 6 处 `movl $<A>` vs `movl $0x0,0x4(%esp)` = typeinfo 地址（.o 未解析，AE 等价）。
    unsigned char checkFlag = 0;
    (void)flag;
    try
    {
        if (dungeonType == (ENUM_DUNGEON_TYPE)1)
        {
            throw (ENUM_ERROR)8;
        }
        m_dungeonMgr.reset();
        if ((int)!m_dungeonMgr.initDungeonMgr(dungeonIdx))  // ORIG 0x84639f5 xor$1 值形
        {
            throw (ENUM_ERROR)0x16;
        }
        int cond = 0;
        cond = _checkStartGameCondition(checkFlag);
        // [b28d-solo-CDeathTower] P225：ORIG 0x8463a44..0x8463a4d 为
        // `cmpl $0,-0x20; setne %al; test %al,%al; je` 四段物化（4.1 槽比零），
        // 4.4 对 if(cond!=0) 恒折 `cmpl;je` 直跳 ⇒ 改 while+break 形复现
        // （P225 CVillageAttacked/CDropItem 实证；register/P226 两形实测劣）。
        while (cond != 0)
        {
            throw (ENUM_ERROR)cond;
            break;
        }
        m_stage.reset();
        m_stage.resetCurrentStage();
        CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
        if (map == 0)
        {
            throw (ENUM_ERROR)0x15;
        }
        if ((int)!m_stage.consistMap(m_dungeonMgr, map, this))  // ORIG 0x8463b11 xor$1
        {
            throw (ENUM_ERROR)1;
        }
        m_playData.reset();
        m_playData.makeStartMemberInfo(m_party);
        for (int i = 0; i <= 3; ++i)
        {
            // ORIG 0x8463b7a：循环体首 `movl $0,-0x14` user dead init；get_user 先行、
            // || 共享物化岛 skip（0x8463bb9/0x8463bc0，范式40 同 _onFinishDeathTower）。
            CUser* user = 0;
            user = m_party->get_user(i);
            // [b28d-D13] P0 清绑（N96）：同 _onFinishDeathTower。
            if ((user == 0) || (int)!m_party->checkValidUser(i))
                continue;
            int minLevel = m_dungeonMgr.getDungeon()->get_min_level();
            if ((bool)(user->get_charac_level() < minLevel))  // ORIG 0x8463bf4 cmp+setl+test
            {
                throw (ENUM_ERROR)0xe;
            }
            user->set_state((ch_state)10);
            m_playData.setMemberAlive(i, true);
            // ORIG 0x8463c5f：0xcd8 具名字段直读（无 call）；块序（范式17）：
            // !=1 的 Increment(…,false) 为 fall-through@8463c6a，==1 块殿后@8463c88。
            if (m_party->m_battleField.m_bloodState != 1)
            {
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, false);
            }
            else
            {
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, true);
            }
        }
        m_party->gen_timer_key((TIMER_MESSAGE)0x2d);
        m_party->SetSelectedEPLPCmd(-1);
        // ORIG 0x08463ce9：party 内嵌 BF（+0xb24）getRandomBuffDungeonType(0) 结果
        // 免槽直入 call arg（范式37）。
        // [H10 CDeathTowerTU 试形 B·已回滚] 具名 buffType 中转实测 ndiff 9→236
        // （帧 +0x10、多 2 mov）⇒ 非 ORIG 形；维持免槽直入（范式37）。
        // [TowerSkill20 2026-09-11 试形·已回滚] receiver 具名前置（`CParty* party =
        // m_party; party->choose_random_buff(...)`）⇒ our 441→442、ndiff 9→236 劣化；
        // ORIG 0x08463cfe 形（receiver 先落 %edx、值留 %eax）属 4.1 寄存器分配族，源码不可择。
        m_party->choose_random_buff((QuickParty::RandomBuffDungeon)
            m_party->m_battleField.getRandomBuffDungeonType(0));

        CPacketHandler packet;  // ORIG -0x30：析构在 try 尾（0x8463e81），lifetime 覆盖后续全部
        packet.put_header(0, 0x8e);
        packet.put_int(dungeonIdx);
        packet.put_short(m_dungeonMgr.getEndStage());
        packet.put_byte(m_party->m_battleField.m_bloodState);    // ORIG 0x8463d69：0xcd8 直读
        // 块序（范式17）：非 NULL→get_random_buff_type 为 fall-through@8463d87，
        // NULL→put 0xb 岛殿后@8463da5。
        if (m_party != NULL)
        {
            packet.put_byte(m_party->get_random_buff_type());
        }
        else
        {
            packet.put_byte(0xb);
        }
        packet.finalize(true);
        m_party->send_to_party(packet);
        packet.clear();
        packet.put_header(0, 0x8f);
        m_stage.makeStagePacket(packet);
        packet.finalize(true);
        m_party->send_to_party(packet);
        _checkMemberReady();
        setTowerState((ENUM_TOWER_STATE)2);
        _onStartDeathTower();
    }
    // [P6/P229] ORIG 抛掷点 0x08463aXX 段 `movl $&_ZTI10ENUM_ERROR,0x4(%esp)`（0x8b75f40
    // = typeinfo for ENUM_ERROR，nm 实证）⇒ 原码是 ENUM_ERROR 抛掷 + 按值 catch；
    // 旧 `catch (int e)` 生成 _ZTIi（.o 重定位实证）为真差异。
    // 比较用 (int)e 规避 -fstrict-enums（P305）对窄枚举值域的折叠。
    catch (ENUM_ERROR e)  // ORIG 0x8463e91：type selector cmp $0x1 → begin_catch；e 槽 -0xc
    {
        if ((int)e == 0x11)  // ORIG 0x8463eab：0x11 分支 PacketGuard@-0x3c 带 checkFlag
        {
            PacketGuard g;
            g.put_header(1, 0x10);
            g.put_byte(0);
            g.put_byte(0x11);
            g.put_byte((int)checkFlag);
            g.finalize(true);
            m_party->send_to_party(g);
        }
        else            // ORIG 0x8463f68：另一 PacketGuard@-0x48 带 e
        {
            PacketGuard g;
            g.put_header(1, 0x10);
            g.put_byte(0);
            g.put_byte((int)e);
            g.finalize(true);
            m_party->send_to_party(g);
        }
        _destroy();
        return 0;
    }
    return 1;
}