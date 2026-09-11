// 收集全部生效 premium 项：逐项发 DB_Q(0x54) 流、解除并重挂效果、通知剩余时长。
// [b28d-19 / b28c-R26 修复] verdict=DIFF(routed) [N82 残余] ORIG 0x084abf4e
//   orig/ours=242/243。本轮修掉三处 p167 真缺口（修复前 OUR calls=39 / ORIG=48）：
//   ① 8 处 `guard << x` → `guard.operator*()->operator<<(x)`：ORIG 每处先
//      `call CStreamGuard::operator*`（0x080c8c46）再 `call CStreamGuard::operator<<`
//      （0x080c8c56 / 0x080e2276 / 0x080e2292）⇒ P103/P132 链不可省（+8 call）。
//   ② 循环内 GetPremiumInfo 具名局部 → 两次独立调用：ORIG 0x084abf93（判条件）
//      与 0x084abfb0（push_back）各一次（P48：O0 无 CSE）。
//   ③ serverCode `unsigned int` → `int`：ORIG 用 `CStreamGuard::operator<<(int)`
//      （lsEi 0x080c8c56），unsigned 实参选 lsEj 0x080e2292 = 重载选错。
//   修复后 p167 CLEAR（48=48；残余 C1/C2、D1/D2 为 P93 同址别名伪差）。
//   残余 DIFF = 槽/帧序跨版本差（N82）：ORIG guard -0x3c/vector -0x30/it -0x24 一族。
// [round5-GmCmds_G 复核] 旧 N82 判词被**部分推翻**：
//   ④ P0 双向律：ORIG 0x084ac060 `call StreamPool::Acquire` 后结果**直留 %eax**
//      压 0x4(%esp) 给 CStreamGuard ctor（全程无 stream 栈槽）⇒ 原码把 Acquire
//      内联在 ctor 实参位；删具名 `Stream* stream` 局部后 243→241 条（gap −1→+1）。
//   残余 = 循环/局部槽整体平移（我方 guard -0x40/vector -0x34/it -0x38 一族 vs
//   ORIG -0x3c/-0x30/-0x24，含 4.4 首槽常数与 N98 使用序）⇒ 源级不可翻。
void CUserPremiumDelete::execute()
{
    CUser* user = GetUser();
    WongWork::CUserPremium* premium = user->GetPremiumInfo();

    std::vector<WongWork::SUserPremiumInfo> list;
    for (int type = 1; type <= 0x69; ++type)
    {
        // P48：ORIG 0x084abf93 与 0x084abfb0 是两次独立 GetPremiumInfo 调用
        // （O0 无 CSE；缓存具名局部会少 1 次 call，p167 MISSING 实测）。
        if (premium->GetPremiumInfo(type)->m_type != 0)
            list.push_back(*premium->GetPremiumInfo(type));
    }

    long curSec = (long)GlobalData::s_systemTime_.getCurSec();
    long nextSec = curSec + 1;

    for (std::vector<WongWork::SUserPremiumInfo>::iterator it = list.begin();
         it != list.end(); ++it)
    {
        int serverCode = 0;
        if (g_SPremiumInfo[it->m_type].m_useTargetServer == 1)
            serverCode = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378

        // P0 双向律：ORIG 0x084ac060 `call Acquire` 结果仍留 %eax 直压
        // 0x4(%esp) 给 CStreamGuard ctor（全程无 stream 栈槽往返）⇒ 原码把
        // Acquire 内联在 ctor 实参位；具名 Stream* 局部会多 store+load。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0x989), true);
        // ORIG operator<< 重载序：int / uint / uint / int / long / long /
        // int / uint（0x54 头 + uid + accId + type + curSec + nextSec +
        // flag + serverCode）。
        // P103/P132：ORIG 每处都先 call CStreamGuard::operator*（0x080c8c46）
        // 再 call CStreamGuard::operator<<（0x080c8c56/0x080e2276/0x080e2292）
        // ⇒ 必须保留 operator* 链（8 处；直接 `guard << x` 会少 8 次 call）。
        guard.operator*()->operator<<(0x54);
        guard.operator*()->operator<<(user->GetUID());
        guard.operator*()->operator<<(user->get_acc_id());
        guard.operator*()->operator<<(it->m_type);
        guard.operator*()->operator<<(curSec);
        guard.operator*()->operator<<(nextSec);
        guard.operator*()->operator<<(it->m_flag);
        guard.operator*()->operator<<(serverCode);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

        WongWork::CHandlePremium::handleSetUserPremium(
            user, (ENUM_PREMIUM_TYPE)it->m_type, curSec, nextSec,
            it->m_flag, false);
        WongWork::CHandlePremium::handlePremiumEffect(
            user, (ENUM_PREMIUM_TYPE)it->m_type);
        WongWork::CHandlePremium::handleNotifyPremiumInfo(
            user, it->m_type, (int)(nextSec - curSec));
    }
}