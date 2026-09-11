// ORIG 0x0829996c：查询 "seLect m_id, level from gm_manifest"；失败 trace 返回；
// 成功先 clearGmList，逐行 fetch 取 m_id/level 并 LoadGmList，累计 m_id 日志串；
// 若 get_channel_no()==1 再 SendGMList_To_UpperServer(0) 同步监视服务器。
// [A28 round5 定稿] 本轮修复（ORIG 0x0829996c 逐段对齐）：
//   ① 返回类型 bool（三处 `mov $0x0,%ebx; jmp` + 尾 `movzbl -0x11(%ebp),%ebx`）；
//   ② P212 全头部声明（ORIG 序言四条 `movb/movl $0x0,-0x11/-0x10/-0x74/-0x78`）；
//   ③ 删 accounts 具名指针（P0：ORIG `mov &s_pGMAccounts_,%eax; mov %eax,(%esp)`）；
//   ④ fetch 结果丢弃 + 同一 ok 复用（P102；ORIG 无 `!fetch()||` 短路段）；
//   ⑤ P221/P227 五处 cMyTrace 改全表达式临时链（具名对象 4.4 复用同槽致帧浅 0x30，
//      同族 CGM_Manager::LoadGmListFromDb 0x08299530 实证）；
//   ⑥ idLog.append 提到 GM M_ID trace 之前（ORIG 0x08299af1 段 < 0x08299bd2）；
//   ⑦ 删旧 `ok = true;`（ORIG 无该 store）；⑧ 发送用 GlobalData::gmList（非隐式 this）。
//   残余 gap=5：三处 `mov $0x0/$0x1,%esi`（4.4 EH 状态寄存器，ORIG 无）+ 两处
//   NumberToString 结果 %ebx(ORIG)/%esi(4.4) ⇒ 判 N104（EH 尾块相对序，ORIG 正常
//   路径尾块在后/4.4 就地内联）+ N7（callee-saved 角色）封顶。
//   p167 调用点差集：仅 Packet_Sync_GM_List C1↔C2 同址别名（P93 良性）。
// [P1 round5 二轮复核] 残余 8 块（ours=251 / orig=246）全部属三类**无源码旋钮**族：
//   ① 4.4 EH 状态寄存器 `mov $0x0,%esi` / `mov $0x1,%esi`（各 2 处；ORIG 无此发射）；
//   ② N7 callee-saved 角色：ORIG `mov %eax,%ebx; … mov %ebx,0x8(%esp)`（两处 cMyTrace/
//      NumberToString 结果保活）vs 我方恒 `%esi`；
//   ③ EH cleanup 岛相对序（N104）：ORIG 0x08299xxx 段 `movzbl -0x11(%ebp),%ebx; jmp <epilogue>`
//      位于 fall-through，4.4 把它就地内联到 `mov $0x1,%esi` 之后。
//   ⇒ P5 旋钮①「守卫包裹形」**不适用**（它改的是条件分支落点，而这里是 EH region 结构 +
//   寄存器角色）；P5 旋钮②（真 switch，本件无 switch 链）与③（类型轴，本件无 lea/sub 歧义）同不适用。
//   按纪律「禁对仓库文件做临时改动+稍后恢复」未做实机 A/B；如需强证可用 /tmp 源码副本 +
//   `-I<Game 目录>` 复编。**维持 N104+N7 封顶**（A28 round5 的 8 条优化已全部在位）。
bool Gm_List::LoadGmListFromDb()
{
    // [P212 全头部声明] ORIG 序言 4 条初始化
    //   `movb $0x0,-0x11; movl $0x0,-0x10; movl $0x0,-0x74; movl $0x0,-0x78`
    //   ⇒ ok/rows/mid/level 全部声明在函数头（块内声明会让 4.4 在块入口才
    //   发初始化，且槽区整体漂移）；row 无初值（序言无 -0xc 清零）。
    bool ok = false;
    int rows = 0;
    int row;
    unsigned int mid = 0;
    int level = 0;
    m_db->set_query("seLect m_id, level from gm_manifest");
    // b28d-61 [P198① 预清必写两句式，ORIG 0x08299axx `call exec; mov %al,-0x11;
    // movzbl -0x11,%eax; xor $1`]：旧单句 `if (!m_db->exec(true))` 直用返回值，
    // 缺槽回读两段。
    ok = m_db->exec(true);
    if (!ok)
    {
        // [P221/P227 全表达式临时链] 具名 trace 对象在 4.4 下复用同槽（帧浅
        //   0x30，同族 CGM_Manager::LoadGmListFromDb 0x08299530 实证）⇒ 改临时链
        //   使 5 个 cMyTrace 各占独立槽（ORIG -0x70/-0x60/-0x44/-0x34/-0x24）。
        cMyTrace(__PRETTY_FUNCTION__, 0x15d4, 9, true, true)(
            "Failed to get GM list from DB.");
        return false;
    }
    {
        // ORIG 0x08299ab4：`mov &s_pGMAccounts_,%eax; mov %eax,(%esp); call
        // clearGmList` 直传 ⇒ 无具名 accounts 指针槽（P0 内联）。
        GlobalData::s_pGMAccounts_->clearGmList();
        std::string idLog;   // ORIG 0x08299ab9：clearGmList 之后构造（0x08109a4c 区）
        rows = m_db->get_n_rows();
        for (row = 0; row < rows; ++row)
        {
            // ORIG 0x08299a54 段：`call fetch` 结果**丢弃**（无 test），紧接着
            //   `call get_int(0,&mid); mov %al,-0x11` ⇒ 原码是两句独立语句 +
            //   同一 bool ok 复用（P102），非 `if(!fetch()||!get_int())` 短路。
            m_db->fetch();
            ok = m_db->get_int(0, mid);
            if (!ok)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x15e5, 9, true, true)(
                    "Failed to get GM's m_id from DB.");
                return false;
            }
            // ORIG 0x08299af1 段：`NumberToString → string ctor(-0x4c) →
            //   operator+(-0x50, ", ") → idLog.append` 位于 GM M_ID trace **之前**
            //   （0x08299b02/0x08299b31 早于 0x08299bd2 的 ctor）。
            idLog.append(std::string(NumberToString(mid, 0)) + ", ");   // ORIG: _ZStpl + append
            // ORIG 0x08299bb2：`mov %eax,%ebx`（NumberToString 结果跨 ctor 保活）
            //   ⇒ P227 形：实参在 ctor 之前求值。
            cMyTrace(__PRETTY_FUNCTION__, 0x15ec, 0)("GM M_ID : %s",
                                                      NumberToString(mid, 0));
            // ORIG 0x08299c05 段：get_int 先、trace 构造在失败分支内。
            ok = m_db->get_int(1, level);
            if (!ok)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x15f2, 9, true, true)(
                    "Failed to get GM's lv from DB.");
                return false;
            }
            GlobalData::s_pGMAccounts_->LoadGmList(mid, level);
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x15f9, 9, true, true)("\t- GM m_id: %s",
                                                              idLog.c_str());
        if (G_CEnvironment()->get_channel_no() == 1)
            // ORIG 0x08299d06：`mov &GlobalData::gmList,%eax; mov %eax,(%esp)`
            //   ⇒ receiver 是全局 Gm_List 对象，非隐式 this（P237 同族）。
            GlobalData::gmList->SendGMList_To_UpperServer(0);
    }
    return ok;
}