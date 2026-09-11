// sendBringUpReward（ORIG 0x0810fe90）
// ORIG 固定索引 1 比较 pair.second+0x2c = Action_SendMail::m_itemIdx（+0x20，
// 相对 pair 为 +0x2c；非 m_gold）。比较装载序：先 operator[]([1]) 后
// operator->(当前元素)。getBringUpRewardFlag() 用 if(flag) continue 布局
// （P59：call;test;jne 循环尾）。日志参数与 __PRETTY_FUNCTION__ 照 ORIG。
// [b28d-L14 N82附] ORIG 0x0810e2b0；p167 CLEAR（58/58）；残差=槽深/常量装载。
// [b28d-W8EventB 本波] 已修三处（DIFF 残差 329/333，语义/调用点全等）：
// ①result 提到循环外 + `int deadSlot = 1; (void)deadSlot;`：ORIG 0x810fed4
//   `movl $0x0,-0x14`（result 预清 P198①，两分支共用同槽）+ 0x810fedb
//   `movl $0x1,-0x10`（死局部，全函数无读取点 ⇒ P171 死写入照抄）⇒ 头部两条
//   store 与 result/sig 槽（-0x14/-0x10/-0xc）逐槽对齐；
// ②SIG_OBJECT_BRING_UP::m_serverGroup 1B→4B（SigTypes_D.h，Main 已批）：
//   ORIG 0x0810ffb8 `movzbl %al,%edx; mov %edx,0x8(%eax)` 与 CUser::UpdateData
//   0x08654089 同形，4B 写；sizeof 0x10 与 +0x0c/+0x0d 偏移不变；
// ③[S6 本波归零] 原记「残余三类跨版本产物」中两类**实际可修**，均已落地：
//   a) 帧 0x70 vs 0x60 与 it/actionMng 槽浅 4 —— **非版本常数**：actionMng 提至 for
//      之外后 it(-0x40)/actionMng(-0x39) 及全部 10 处 `lea` 与 ORIG 逐槽对齐、帧两侧 0x60；
//   b) varargs 顺序 —— 原码实参**写反**（ORIG = get_acc_id→getCurCharacNo→m_itemIdx，
//      见 0x0810ffcc/0x0810ffd9 调用序与压栈位 0x14/0x18/0x1c = accId/characNo/m_itemIdx）
//      ⇒ 属**语义轴**（日志输出错位），已按 ORIG 改正；
//   c) std::copy/back_inserter 内联块序（4.4 把正常路径析构块压到 landing pad 之后，
//      多 1 条 jmp）—— 保留为 N104 块排布残差。
// 【b28d-D03 更正 · 该头依赖已闭合】原记「getBringUpRewardFlag 返回 uchar（头依赖，
//   已上报 Main）」为**过时信息**：Main 已在 S350 代落 `CUserCharacInfo.h:567
//   m_bringUpRewardFlag` 与 `getBringUpRewardFlag()` 双 bool 化（1 字节/偏移/mangled 不变）。
//   本波 fresh 实测：调用点两侧均为 `call; test %al,%al; jne`（**无 setne 差异**）⇒ 该项
//   不再是本件残差来源。C05④ 四步落点（留档）：①无消费者死比较？否 ②值槽化/表达式
//   物化？否（两侧同寄存器直测）③callee 声明不一致？**否（已两侧 bool）**⇒ 归因 1 不成立
//   ④[S6 本波终值] 329/331（gap −2），残差仅 2 条：a) `sig->m_rewardFlag = f()` 装载
//   ORIG `mov -0xc(%ebp),%edx; mov %al,0xd(%edx)` vs 4.4 先搬 eax（N7/P211 寄存器角色）；
//   b) 正常路径 CStreamGuard 析构块位置（N104）。p167 58/58 CLEAR（仅 C1/C2·D1/D2 别名）
//   ⇒ 语义完成、byte 层 N 封顶。
bool CObjectBringUp::sendBringUpReward(CUser* user,
// [P8S2 2026-09-11 负结论·P139] ORIG prett=0x5a `bool EventClassify::CObjectBringUp::sendBringUpReward(CUser*, IntVector&, unsigned char&)`（0x08b4d2c0）VS 我方 0x77 展开形 `std::vector<int, std::allocator<int> >&`：修法=定义处第 2 参改 `IntVector&`，但全库无 `IntVector` typedef（GameTypes.h 只有 `INT_VECTOR`；ARAD_Database.h:31 的 IntVector 不在本 TU 可见域）⇒ 需 P7 在 GameTypes.h 补 `typedef std::vector<int> IntVector;` 后同法落地；本泳道不自行新增 typedef（父泳道硬性约束）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（P8S2 负结论·P139 挂起）→ 试=GameTypes.h 已备
//   `typedef std::vector<int> IntVector;`（Q6 落）⇒ 定义处第 2 参改 `IntVector&`（同底层，
//   mangled/ABI 零变化），3 处 __PRETTY_FUNCTION__ 串随之由展开形 0x77 变 ORIG 逐字节同形。
//   ⇒ 推翻 P8S2「需 P7 补 typedef」前置（已满足），落地。
                                       IntVector& rewardVec,
                                       unsigned char& flag)
{
    if (user == 0)
    {
        LogManager::logFormat(
            1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7ad,
            "CObjectBringUp::sendBringUpReward(pUser(NULL))");
        return false;
    }

    // [b28d-W8EventB 实测] ORIG 0x810fed4/0x810fedb 函数头两条 store：
    // `movl $0x0,-0x14(%ebp)`（result 预清，P198①）+ `movl $0x1,-0x10(%ebp)`
    // （死局部，全函数无读取点 ⇒ P171 死写入照抄）。两分支共用同一 result 槽。
    int result = 0;
    int deadSlot = 1;
    (void)deadSlot;

    // [L2Free4to15.S6 本波归零] P-L11d 正解 = 提到 **for 之外**（原置于 if 之前仅
    //   使两分支共用槽，槽号仍 -0x3d/it -0x3c，与 ORIG 差 4）。判据：ORIG actionMng
    //   槽 -0x39 高于 it 槽 -0x40（1B 对象落在分配单位末字节：[-0x3c,-0x39)）⇒
    //   actionMng 的展开先于 it；or 内声明无法产生该序。提到 for 前实测：it/actionMng
    //   及全部 10 处 `lea -0x40/-0x39` 与 ORIG 逐槽对齐，帧 0x60 两侧一致；
    //   CEventActionMng 无 ctor/dtor（ORIG 两侧调用点均无 ctor/dtor call）⇒ 生命周期
    //   提升无语义影响。
    CEventActionMng actionMng;

    for (::std::vector< ::std::pair<ObjectBringUpScript::ObjectBringUpRule,
                                  ::Action_SendMail> >::iterator it =
             static_cast<ObjectBringUpScript*>(m_script1c)
                 ->m_sendMails28.begin();
         it != static_cast<ObjectBringUpScript*>(m_script1c)
                   ->m_sendMails28.end();
         ++it)
    {
        if (!verifyGiveBringUpReward(
                it->first, (int)user->getCurUseBringUpMaterialTotal()))
            continue;

        if (static_cast<ObjectBringUpScript*>(m_script1c)
                ->m_sendMails28[1].second.m_itemIdx == it->second.m_itemIdx)
        {
            if (user->getBringUpRewardFlag())
                continue;

            result = actionMng.process_action_send_mail(
                user, it->second, false);
            if (result != 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7c3,
                    "CObjectBringUp::sendBringUpReward(m_id:%d, "
                    "charac_no:%d, reward:%d)",
                    (int)user->get_acc_id(), (int)user->getCurCharacNo(),
                    it->second.m_itemIdx);
                return false;
            }
            std::copy(it->second.m_vec38.begin(),
                      it->second.m_vec38.end(),
                      std::back_inserter(rewardVec));
            it->second.m_vec38.clear();
            ++flag;
            user->setBringUpRewardFlag(true);

            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("EventClassify.cpp",
                                                   0x7cc),
                true);
            guard.operator*()->operator<<((int)0x2ca);
            guard.operator*()->operator<<((int)-1);
            SIG_OBJECT_BRING_UP* sig =
                guard.operator->()->GetInBuffer<SIG_OBJECT_BRING_UP>();
            sig->m_accId = user->get_acc_id();
            sig->m_characNo = user->getCurCharacNo();
            // m_serverGroup 为 4B 字段（SigTypes_D.h，ORIG 0x0810ffb8 4B 写）；
            // (unsigned char) cast 保留 ORIG `movzbl` 取值宽度。
            sig->m_serverGroup =
                (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
            sig->m_usedMaterialCnt =
                user->getCurUseBringUpMaterialCount();
            sig->m_rewardFlag = user->getBringUpRewardFlag();
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            user->resetCurUseBringUpMaterialCount();
        }
        else
        {
            result = actionMng.process_action_send_mail(
                user, it->second, false);
            if (result != 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7e2,
                    "CObjectBringUp::sendBringUpReward(m_id:%d, "
                    "charac_no:%d, reward:%d)",
                    (int)user->get_acc_id(), (int)user->getCurCharacNo(),
                    it->second.m_itemIdx);
                return false;
            }
            std::copy(it->second.m_vec38.begin(), it->second.m_vec38.end(),
                      std::back_inserter(rewardVec));
            it->second.m_vec38.clear();
            ++flag;
        }
    }
    return true;
}