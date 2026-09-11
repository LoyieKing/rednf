// IncreaseExpertJobExp @ 0x849ae40：clamp exp 至 m_expVec 末项上限；跨级/边界
// 处理 isBoundaryExpValue + isExpertJobLimitLevel；SetCurCharacExpertJobExp +
// make_basic_info 包；非边界本地 Send，边界则 LogHistory + send_all + 升级技能
// m_growSkillVec 循环 + send_skill_info + quest 包。
// [b28d-solo-CExpertJob] N 封顶（语义完成、p167 CLEAR 60/60）：本件 341 条。
//   已复现：P0 去 lvlMap/gw/sk/lv2 具名局部（ORIG 0x0849af10/0x0849b127/0x0849b14e
//   均寄存器免槽）、P225 推广嵌套 if 复现 setle 物化（0x0849af76 逐条全等）、
//   P226 扩展语句表达式复现 b 短路物化岛零槽（0x0849af90 段）、P196 比较方向
//   `exp > prevExp`（0x0849aff8 jle 段全等）、块序 `if (isBoundary){长}else{Send}`
//   复现 0x0849b0c6 `cmpb;je 尾部` 布局。剩余差异（结构性、语义等价）：
//   ① 开头 getCurCharacR()==0 的 4.1 取反物化 `setne;je` vs 4.4 `sete;jne`
//      （setcc-fold 跨版本极性，两形实测均折）；② 尾部 4.4 epilogue-threading
//      `jmp;nop;jmp;nop` 岛（N121 三形不可消）。ORIG 0x0849ae40
void CExpertJob::IncreaseExpertJobExp(CUser* user, int exp)
{
    // [b28d-solo-CExpertJob N-setcc-fold] ORIG 0x0849ae54 `test %eax,%eax;
    //   setne %al; test %al,%al; je <尾>`（4.1 取反物化）；`p == 0`/`!(p!=0)` 两形
    //   4.4 均产 `sete %al; jne`（极性反、语义等价）⇒ 跨版本 setcc-fold 封顶。
    if (user->getCurCharacR() == 0)
        return;
    STExpertJobScript* script = (STExpertJobScript*)
        G_CDataManager()->GetExpertJobScript(user->GetCurCharacExpertJobType());
    if (script == 0)
        return;
    int maxExp = script->m_expVec[script->m_expVec.size() - 1].m_exp;
    if (maxExp < exp)
        exp = maxExp;

    bool isBoundary = false;
    int curLevel = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    int newLevel = user->GetCurExpertJobLevel(exp);
    STExpertJobEtcScript* etc =
        (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
    if (curLevel != newLevel)
    {
        // [b28d-solo-CExpertJob P0] ORIG 0x0849af10 直接用 etc 重读 `mov -0x28(%ebp),
        // %edx; lea 0xc(%eax),%edx`（find 与 end 各重取一次），无 lvlMap 具名局部槽。
        std::map<unsigned char, short>::iterator it =
            etc->m_levelMap.find((unsigned char)newLevel);
        // [b28d-solo-CExpertJob P225 推广] ORIG 0x0849af76 `cmp %eax,%ebx;
        //   setle %al; test %al,%al; je`（4.1 对 <= 的 setcc 物化）。`&&` 合并形
        //   被 4.4 折成 jg/mov$1/mov$0 直跳；拆嵌套 if 即逐条复现（P225 推广实证）。
        if (it != etc->m_levelMap.end())
        {
            if ((int)it->second <= user->get_charac_level())
                isBoundary = true;
        }
    }
    else
    {
        // [b28d-solo-CExpertJob P226 扩展] ORIG 0x0849af90 短路物化岛零槽：
        //   `call isBoundaryExpValue; test; je L0; …call isExpertJobLimitLevel;
        //    test; je L0; mov $1,%eax; jmp L1; L0: mov $0,%eax; L1: test %al,%al;
        //    je skip; cmp $0xb,curLevel; je skip`。具名 bool b 会落栈槽
        //   （cmpb $0,-N），裸语句表达式复现零槽物化。
        if (({ script->isBoundaryExpValue(exp) &&
               etc->isExpertJobLimitLevel(user->get_charac_level()); }))
        {
            if (curLevel != 0xb)
                isBoundary = true;
        }
    }

    int prevExp = script->m_expVec[curLevel - 1].m_exp;
    // [b28d-solo-CExpertJob P196] ORIG 0x0849aff8 `mov 0xc(%ebp),%eax; cmp
    //   -0x24(%ebp),%eax; jle` ⇒ 比较方向 exp <= prevExp（源码写 exp > prevExp）。
    if (!isBoundary && exp > prevExp)
        exp = prevExp;
    if (isBoundary || user->GetCurCharacExpertJobExp() != exp)
    {
        user->SetCurCharacExpertJobExp(exp);
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->clear();
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        user->make_basic_info((char*)&packet, 0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        // [b28d-solo-CExpertJob 块序] ORIG 0x0849b0c6 `cmpb $0x0,-0x31(%ebp);
        //   je 0849b2a2`（isBoundary==0 跳尾部 Send 块）⇒ 原码正极性
        //   `if (isBoundary) {边界块} else {Send}`，4.4 才把 Send 短块放到
        //   边界块（含 questPacket 析构）之后，与 ORIG 物理序一致。
        if (isBoundary)
        {
            int lv = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
            user->LogHistory("ExpertJobLv+,%d,%d,%d", user->get_charac_level(),
                             user->GetCurCharacExpertJobType(), lv);
            // [b28d-solo-CExpertJob P0] ORIG 0x0849b127 `call G_GameWorld; mov %eax,
            // (%esp)` 直传 receiver，无 gw 具名局部槽。
            G_GameWorld()->send_all(packet, user);
            for (size_t i = 0; i < script->m_growSkillVec.size(); ++i)
            {
                // [b28d-solo-CExpertJob P0] ORIG 0x0849b14e body：lv2 落 %esi、sk 落
                //   %edi（均无槽）⇒ 禁具名 sk/lv2 局部，实参 R2L 内联（P157：每段重新
                //   operator[]）。
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), script->m_growSkillVec[i].first,
                    user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()),
                    (ENUM_SKILL_TREE_KIND)0);
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), script->m_growSkillVec[i].first,
                    user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()),
                    (ENUM_SKILL_TREE_KIND)1);
            }
            user->send_skill_info();
            PacketGuard questPacket;
            user->getCurCharacQuestR()->get_quest_info((char*)&questPacket);
            user->Send(questPacket);
        }
        else
        {
            user->Send(packet);
        }
    }
}