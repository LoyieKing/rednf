// ---------------------------------------------------------------------------
// 7) CUser::_onChangeGrowType(uchar, uchar newSecond, eChangeGrowTypeReason reason)
//    报告：docs/class_func_reports/CUser/_onChangeGrowType.md。ORIG 0x08664080。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08664080：N82 封顶——帧/槽差（p167 CLEAR）。
void CUser::_onChangeGrowType(unsigned char newFirst, unsigned char secondGrew,
                              eChangeGrowTypeReason reason)
{
    // [Lane39 判据①/P0/P224] ORIG 0x0866409x 无 ev/on 具名槽：`call GetRepeatEvent` 后直接
    // `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; call *%edx` 且 `test %al,%al;
    // je +0xb4` 与 `cmpl $1,0x14(%ebp); je +0xb4` **两失败出口同目标**（裸短路 &&）；
    // 具名 bool on 反产 `movb %al,-0x29; cmpb $0,-0x29; je` 物化 + 多 jcc/jmp。
    {
        // [Lane39 判据①/P223] ORIG 两个失败出口同落 +0xb4（共享 join）。
        // [A06 2026-09-10 C05③ 两手实测] 负条件 goto 与 `||` 合并（199/207 反劣）均不达标；
        // 按 Main 三手协议第 2 手（守卫含 call/setcc ⇒ **正向嵌套 if**，F3c 实证）改写：
        // 4.4 把 fail 路径 jump-thread 到共享尾出口，逐条 `test; je` / `cmp; je` 与 ORIG 同构。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x15)->IsEventing((CUser*)0))
        {
            if (reason != (eChangeGrowTypeReason)0x1)
            {
                // ORIG：makeRequest 两分支独立，每分支各自重调 getCurCharacNo/get_acc_id
                // （访问器结果禁缓存，战术 P183/P48）。
                if (secondGrew == 0)
                {
                    DB_InsertArchieveEventLog::makeRequest(
                        get_acc_id(), getCurCharacNo(),
                        (SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE)10u);
                }
                else
                {
                    DB_InsertArchieveEventLog::makeRequest(
                        get_acc_id(), getCurCharacNo(),
                        (SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE)0xb);
                }
            }
        }
    }

    // [A06 2026-09-10 P226 扩展 试行记录·已回退] ORIG 0x086640c5 段 env/now 无槽 + 短路岛
    // `mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je` ⇒ 试语句表达式形实测**反劣**
    // （205→210 条：4.4 对语句表达式岛在多条件链上产两处岛/多 5 条）⇒ 回退。
    // [A06 2026-09-10 结果] 去 bool 槽 + 短路合写后 200→197 条、行级 diff 325→132；
    // 残余 6 条 = ORIG 尾部 4 个 `jmp+nop` 中继岛（+0x37d/+0x380/+0x383/+0x386，均跳
    // +0x387），4.4 对同目标多源恒 jump-thread（P223 负例族，待独立复查）。
    // [A06 2026-09-10 P0/P198②] ORIG 本段 **env/now 皆无栈槽**（0x086640c5 `call
    // G_CEnvironment` 后直接 `mov 0x378(%eax),%eax`；`call getCurSec` 后直接
    // `cmp $0x478e7050,%eax; ja`），且无具名 bool 槽（`mov $1/%eax; …; mov $0/%eax;
    // test %al,%al; je`=4.1 对整体 && 的寄存器物化）⇒ 具名 `bool gmTimeActive` 与
    // env/now 局部各多一对 store/load。改为**单一短路 if**（P133 合并短路），
    // 内层守卫嵌套（两失败出口同目标）。
    if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 10 &&
        GlobalData::s_systemTime_.getCurSec() < 0x478e7051u)
    {
      // [H6B 2026-09-11 试形·早退 return] Ghidra 反编译 0x086641xx 段为
      // `if (param_4 == 1) return; if (param_2 != '\0') return; <body>`（两失败出口
      // 为**早退**而非嵌套 if）；旧 `reason != 1 && secondGrew == 0` 单条短路 if 使
      // 4.4 少发 ORIG 尾部 jmp 岛。改早退形，读 197→?（ORIG 199）。
      if (reason == (eChangeGrowTypeReason)0x1) return;
      if (secondGrew != 0) return;
      char msg[256];
      strncpy(msg, g_scriptStringManager_.findString(4, "game_server_msg_13", 0), 0xff);
      Inven_Item gift;
      WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
          g_scriptStringManager_.findString(4, "game_server_msg_09", 0),
          gift, 150000, getCurCharacNo(), msg, strlen(msg), 0, GetServerGroup(), 0, 0);
    }

    // [A06 2026-09-10 P226 扩展 试行记录·已回退] ORIG 0x0866414d 段同形（无槽短路岛）；
    // 两条语句表达式同批实测净 +5 条（含本段）⇒ 一起回退。
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x10) == true &&
        GlobalData::s_systemTime_.getCurSec() <= 0x46e1bbd0u)
    {
      if (reason != (eChangeGrowTypeReason)0x1 && secondGrew == 0)
      {
        // ORIG 0x08664185：`call getCurCharacInvenR; … call GetInvenRef(0,0xb);
        // mov 0x2(%eax),%eax; sub $0x65d6,%eax; cmp $0x4,%eax` —— 两 accessor 结果
        // **均无具名槽**（inven/ref 旧形各多一对 store/load，P0 判据⑦）⇒ 内联链式。
        // [M8COA 2026-09-11 负结论·已回滚] ORIG 0x08664190 `cmp $0x4,%eax; ja <尾>`
        // （单条分支）vs 我方 `cmp $0x4,%eax; setbe %al; test %al,%al; je <尾>`（bool
        // 物化三连）。试 `<= 4u` 字形（探针 /tmp/r5/p_grow.cpp f1..f7 在独立函数内
        // 均产 ja）实测 **199→199 / setbe 三连原样保留** ⇒ 差异由**本函数上下文**
        // （三层嵌套守卫 + 巨型函数体）触发，单表达式字形不可控 ⇒ 归 N 族（连同尾部
        // ORIG 4 条 `jmp` 岛 vs 我方 2 条，两处合计净 0，属 4.1/4.4 分支 lower 差）。
        if ((getCurCharacInvenR()->GetInvenRef(0, 0xb)->m_addInfo - 0x65d6u) < 5u)
        {
            char msg[256];
            strncpy(msg, g_scriptStringManager_.findString(4, "game_server_msg_07", 0), 0xff);
            Inven_Item gift;
            gift.m_addInfo = 0x43d;
            // [H6 2026-09-11 语义轴·真字段错位/覆盖] ORIG 0x08664333/3d/44/4e 四处写：
            //   +0x02 m_addInfo  = 0x43d（0x08664333 `movl $0x43d,-0x90(%ebp)`）
            //   +0x01 m_field1   = 2    （0x0866433d `movb $0x2,-0x91(%ebp)`）
            //   +0x07 m_addInfo2 = 0x14 （0x08664344 `movl $0x14,-0x8b(%ebp)`）
            //   +0x0b m_fieldb   = 0    （0x0866434e `movw $0x0,-0x87(%ebp)`）
            // 旧形 `((char*)&gift)[0] = 2` 写错到 +0x00（m_field0），随后 `gift.m_addInfo = 0;`
            // **覆盖**刚写入的 0x43d ⇒ 邮件礼物 itemId 归零（真 bug，行为可见）。
            gift.m_field1 = 2;
            gift.m_addInfo2 = 0x14;
            gift.m_fieldb = 0;
            gift.ResetItemAttr();                           // ORIG 0x08664360
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_08", 0),
                gift, 0, getCurCharacNo(), msg, strlen(msg), 0, GetServerGroup(), 0, 0);
        }
      }
    }
}