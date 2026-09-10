// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Gm_List/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r：Gm_List 独立编译单元。
// ---- b28d-61 判词汇总（本 TU 2 件非 identical，全 N 封顶）----
// SendGMList_To_UpperServer 0x08299d54（P167 MISSING+EXTRA：Packet_Sync_GM_List
//   构造/发送块差异，N80 变参求值序）、LoadGmListFromDb 0x08299a10
//   （P167 CLEAR 38/38；N78/N89 迭代器+局部初始化 lowering）。
// ORIG SendGMList_To_UpperServer @0x8299d54：
//   serverGroup 0=监视服务器、1=公会服务器；每 20 个 GM 一包（Packet_Sync_GM_List，
//   header 0x1f4d/0x70），m_pageNo 递增；末批不足 20 也发送。
#include "CGuildServerProxy.h"   // Gm_List / WongWork::CGMAccounts 权威声明

#include <list>

#include "CGuildServerProxy.h"      // WongWork::CGMAccounts / stGMInfo_t / proxies
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "Packet_Sync_GM_List.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "CInventory.h"   // NumberToString

// 类声明权威点在 CGuildServerProxy.h；此处仅实现。
#include "Gm_List/SendGMList_To_UpperServer.inc"
{
    if (param != 0 && param != 1)
        return;

    // 探针实测（/tmp/pp6.cpp vs /tmp/pp7.cpp）：形参为 enum 的调用点，若实参处
    //   现场做 int→enum cast，4.4 发「实参先载」`mov slot,%edx; mov <A>,%eax`；
    //   实参本身即 enum 类型时发 ORIG 形「receiver 先载」`mov <A>,%eax;
    //   mov slot,%edx`（ORIG 0x08299e03/0x08299e33 段逐条同形）⇒ 局部声明为 enum。
    ENUM_SERVER_GROUP serverGroup =
        static_cast<ENUM_SERVER_GROUP>(G_CEnvironment()->get_server_group());
    Packet_Sync_GM_List packet;
    // P183/P0：ORIG 0x08299d8c 段为 `call GetGMList; mov %eax,-0x10(%ebp)` 单条
    //   指针落槽，全函数无 list 拷贝构造/D1 —— 原码是引用绑定，非按值拷贝
    //   （按值拷贝引入 C1ERKS_ + 2×D1Ev + _Unwind_Resume + push %esi，gap +20）。
    const std::list<WongWork::CGMAccounts::stGMInfo_t>& gmList =
        GlobalData::s_pGMAccounts_->GetGMList();
    std::list<WongWork::CGMAccounts::stGMInfo_t>::const_iterator it = gmList.begin();
    int count = 0;
    while (it != gmList.end())
    {
        packet.m_gmIds[count] = it->mid;
        packet.m_gmNames[count] = static_cast<char>(it->type);
        ++count;
        if (count == 0x14)
        {
            // b28d-L3 复测（P34/P52）：ORIG 0x08299df4 `mov -0xc(%ebp),%eax;
            //   mov %al,-0x81(%ebp)` ⇒ 原码用 count 变量（此时 ==0x14），非立即数。
            packet.m_gmCount = static_cast<unsigned char>(count);
            if (param == 1)
            {
                // ORIG 0x08299e03 段：proxy **无具名槽**（`call GetServerProxy;
                //   mov %eax,(%esp)` 直传）⇒ 删具名局部、改单语句链式。
                GlobalData::s_guild_proxy_mgr->GetServerProxy(serverGroup)
                    ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
            }
            else if (param == 0)
            {
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(serverGroup)
                    ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
            }
            ++packet.m_pageNo;
            count = 0;
        }
        ++it;
    }
    if (count != 0)
    {
        packet.m_gmCount = static_cast<unsigned char>(count);
        if (param == 1)
        {
            GlobalData::s_guild_proxy_mgr->GetServerProxy(serverGroup)
                ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
        }
        else if (param == 0)
        {
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(serverGroup)
                ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0x70);
        }
    }
}


// ---- 生命周期 / DB 装载 ----

// ORIG 0x08299912：m_db = DBMgr::GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0)
// 后立即 LoadGmListFromDb()。
#include "Gm_List/Gm_List.inc"
{
    m_db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                             (ENUM_SERVER_GROUP)0);
    LoadGmListFromDb();
}

// ORIG 0x08299948：D1 为 deleting 形态（cmpl $0,0x8(%ebp); je;
// call D1(自身递归); call _ZdlPv）——源码为 `delete this` 展开。
#include "Gm_List/dtor_Gm_List.inc"

// ORIG 0x081094ea：push_back({mid, level})。
#include "Gm_List/LoadGmList.inc"
{
    // ORIG 0x081094ea：POD 直接两 store（mid 先、level 后），无 ctor call
    // （ORIG nm 无 stGMInfo_t C1 符号，聚合 POD）。
    stGMInfo_t info;
    info.mid = mid;
    info.type = static_cast<unsigned int>(level);
    m_gmList.push_back(info);
}

// ORIG 0x08109510。
#include "Gm_List/clearGmList.inc"
{
    m_gmList.clear();
}

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
#include "Gm_List/LoadGmListFromDb.inc"
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
