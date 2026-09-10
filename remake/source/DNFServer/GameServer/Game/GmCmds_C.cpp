// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_C.cpp — game_master 命令体系批次 C（df_game_r ORIG T 符号族还原）。
// 负责类（10）：
//   CCraneNeedMaterial        ctor 0x082a86aa / execute 0x084b0626
//   CCreateDisjointStoreCmd   ctor 0x082a8108 / execute 0x084a9720
//   CDeleteAccountCargo       ctor 0x082a84ce / execute 0x084abdb4
//   CDeleteGuild              ctor 0x082a83e0 / execute 0x084ab29c
//   DungeonDifficulty         ctor 0x082a8892 / parse 0x084b2110 / execute 0x084b2152
//   CEndEvent                 ctor 0x082a845e / parse 0x084ab914 / execute 0x084ab94c
//   CEnduranceCmd             ctor 0x082a83fc / parse 0x084aa73a / execute 0x084aa858
//   CGrowhWeaponEvent         ctor 0x082a88e6 / parse 0x084b28d6 / execute 0x084b293c
//   CGuildDebug               ctor 0x082a8592 / parse 0x084ac720 / execute 0x084ac76c
//   CGuildExpBookDelete       ctor 0x082a8506 / execute 0x084abf2c
// 对照 docs/class_func_reports/game_master__<Class>/{parse,execute}.md。
//
// 头文件占位字段失配说明：GameMasterCmd.h 对本批各类统一给出 m_str0(string@+0x08)
// 等占位名，但 ORIG 真实布局按各类不同：
//   CEndEvent          +0x08 int（事件代码）
//   CEnduranceCmd      +0x08 int（背包槽位）+0x0c unsigned short（耐久值）
//   CGrowhWeaponEvent  +0x08 byte（"gw" 重置标记）+0x0c int（时计数值）
//   DungeonDifficulty  +0x08 byte（难度 1..3）
//   CGuildDebug        +0x08 int +0x0c int +0x10 unsigned int（m_f1 占位即此字段）
//   （gcc4.4 32 位 std::string 仅 4 字节，故占位 string 与上述标量布局同尺寸。）
// 本 TU 一律按偏移访问真实标量字段，不向占位 std::string 写入。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CInventory.h"
#include "CServerProxyMgr.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "CDungeonClear.h"
#include "CDataManager.h"
#include "CExpertJob.h"
#include "CGuildServerProxy.h"
#include "CMonitorServerProxy.h"
#include "STItemScript.h"
#include "Packet_Monitor_Notice_Guild_Dismiss.h"
#include "Packet_Monitor_Event_End.h"
#include "Packet_Guild_Debug.h"
#include "CEnvironment.h"
#include "DB_EventGrowthWeaponReset.h"            // makeRequest（桥清零）
#include "DB_EventUpdateGrowthWeaponTimepiece.h"  // makeRequest（桥清零）

#include <stdlib.h>

namespace game_master
{

// ========================================================================
// CCraneNeedMaterial —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a86aa：基类 vptr → 自身 vptr。

// ORIG execute 0x084b0626：G_GameWorld()->UpdateMiniCraneSeed()。
#include "GmCmds_C/execute.inc"
{
    G_GameWorld()->UpdateMiniCraneSeed();
}

// ========================================================================
// CCreateDisjointStoreCmd —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8108。

// ORIG execute 0x084a9720：取分解师职业槽挂到 GM 角色，随后在其脚下
// 以固定名 "test"/容量 500 创建分解商店（坐标取 GM 角色当前位置低 16 位）。
// 【b28 判词 N-P75】job 槽(-0xc)值供 SetCurCharacExpertJob 实参：ORIG 4.1 在
// GetUser call 前物化入 %ebx（`mov -0xc(%ebp),%ebx` 前置），4.4 恒 call 后
// 重读槽入 %edx（参数装载时刻跨版本调度，P75；P226 语句表达式形在纯槽装载
// 场景不升格，探针 /tmp/bm3/p13 fSE），余 25 条逐条相等。
// [round5-GmCmds_C] 上述 N-P75 封顶**已推翻**（根因是调用点 static_cast 中间
// 表达式，见下函数内注释；改成派生类指针局部后 45=45 逐条等同 ⇒ AE）。
#include "GmCmds_C/execute_1.inc"
{
    // [round5-GmCmds_C 落地·推翻旧 N-P75 封顶] ORIG 0x084a9740 在 GetUser call
    // **之前** `mov -0xc(%ebp),%ebx`（值跨 call 保活入 callee-saved），4.4 原用
    // call 后重读槽入 %edx。根因是调用点的 **static_cast 中间表达式**（原码
    // `static_cast<CDisjointer*>(job)->OnCreate...` 使 4.4 分两次求值 lvalue）；
    // 把 job 的声明类型直接写成 expert_job::CDisjointer*（cast 收进初始化式）后，
    // 4.4 复现 ORIG 的 `mov -0xc(%ebp),%ebx` 前置保活 + 末尾 `mov -0xc(%ebp),%eax`
    // 直传形，45 条与 ORIG 逐条等同（verdict DIFF→**AE**；TU 四数 ae 54→55、
    // diff 3→2，零劣化）。历史「纯 register 提示 45→46」方向已作废。
    expert_job::CDisjointer* job = static_cast<expert_job::CDisjointer*>(
        GlobalData::s_ExpertJobMgr->AcquireExpertJob(
            static_cast<ENUM_EXPERT_JOB_TYPE>(3)));
    // ORIG 0x084a9720：GetUser 四独立重调（P48 无 CSE）；posY/posX 值经
    // %esi/%ebx 跨 call 直传（P0 无槽），movzwl=(unsigned short) 拓宽直传；
    // 实参 R2L：y(+0x40) → x(+0x50) → user(+0x66)。
    // ORIG 0x084a9740：job 在 GetUser call 前物化入 %ebx 跨调用保活（4.1 实参
    // 提前物化）；4.4 恒 call 后重读槽（N-P75 跨版本调度，语句表达式 P226 实测
    // 不升格——/tmp/bm3/p13 fSE 同结论），余 2 条为寄存器名差（N7）。
    // [round5-GmCmds_C 判词·N-P75/N7 复核] ORIG 0x084a9740 在 GetUser call **之前**
    // `mov -0xc(%ebp),%ebx` 把 job 值保活入 callee-saved；本轮补测两形均无效：
    // ①纯 `register` 存储类提示（P231/P248 四条件齐备）⇒ 45→46 条（且 push %ebx
    // 前移帧）；②裸语句表达式 `({ job; })`（P226）⇒ 与裸形逐条全同。⇒ 4.4 对纯槽
    // 装载实参恒 call 后重读（N-P75 跨版本实参调度），余 2 条为寄存器名差（N7）封顶。
    GetUser()->SetCurCharacExpertJob(job);
    job->OnCreateDisjointStore(
        GetUser(), "test", 500,
        static_cast<int>(static_cast<unsigned short>(GetUser()->get_posX())),
        static_cast<int>(static_cast<unsigned short>(GetUser()->get_posY())));
}

// ========================================================================
// CDeleteAccountCargo —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a84ce。

// ORIG execute 0x084abdb4：CAccountCargo::DeleteAccountCargo(GetUser())。
// ORIG 该函数为静态语义直调（cdecl，无 this 使用）；此处经 CUser 内嵌实例
// （布局注记 CUser.h：+0x6effc CAccountCargo）走同一实现路径。
#include "GmCmds_C/execute_2.inc"
{
    // ORIG：GetUser 结果经 eax 直通 DeleteAccountCargo 实参槽（无 user/cargo 具名槽）。
    CAccountCargo::DeleteAccountCargo(GetUser());
}

// ========================================================================
// CDeleteGuild —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a83e0。

// ORIG execute 0x084ab29c：GM 校验通过后向 guild 服务器发解散请求（0x1df 流）、
// 清本地公会键并回发 0x3d 包，同时向 monitor 广播 Packet_Monitor_Notice_Guild_Dismiss。
// [round5-GmCmds_C 复核] 本件已在 **AE** 档（不在 check_tu 的 DIFF 列表）：全函数
// 仅数据地址/别名差（p167 34=34，MISSING/EXTRA 全为 C2/D2↔C1/D1 同址别名，P93）；
// ORIG 0x084ab333 起 `mov -0xc(%ebp),%eax`（guildKey 具名局部跨 set 清零复用）等
// 槽序与 4.1 逐条同形 ⇒ 无旋钮、零改动。
#include "GmCmds_C/execute_3.inc"
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }
    if (user->get_state() != 3)
    {
        return;
    }
    // P48 no-CSE：ORIG 三处各自 fresh get_charac_guildkey()（if 判零 / guard<< /
    // notice.m_guildKey）。
    if (static_cast<unsigned int>(user->get_charac_guildkey()) == 0)
    {
        return;
    }

    // 向 DB 队列发解散流：0x1df + (uid, characNo, guildkey)。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0x68d), true);
    // ORIG 0x084ab333：guildKey 在 guard 构造后立即求值并缓存具名局部（-0xc 槽往返，
    // P0），供 notice.m_guildKey 复用；且必须早于下方 set_charac_guildkey(0)——否则
    // 解散通知包里的公会键恒为 0（真语义 bug，原实现即栽在此）。
    unsigned int guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    // ORIG：`(*guard)->operator<<(x)`（operator* de()）标识调用形态。
    (*guard)->operator<<(0x1df);
    (*guard)->operator<<(user->GetUID());
    (*guard)->operator<<(user->getCurCharacNo());
    // [L11·round3] p167 直证：ORIG 该处为 `_ZN12CStreamGuardlsEi`（int 实参），
    // 我方 `static_cast<unsigned int>` 发 `lsEj` ⇒ MISSING lsEi ×1 / EXTRA lsEj ×1。
    (*guard)->operator<<(static_cast<int>(user->get_charac_guildkey()));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    user->set_guild_create_right(0);
    user->set_charac_guildkey(0);

    PacketGuard packet;
    packet.put_header(0, 0x3d);
    packet.finalize(true);
    user->Send(packet);

    Packet_Monitor_Notice_Guild_Dismiss notice;
    notice.m_guildKey = guildKey;
    // ORIG 0x084ab467..0x084ab492：GetServerProxy 结果 eax 直通 receiver（无 proxy
    // 具名槽），&notice/packetSize 先物化 %ebx/%esi 跨调用保活（P52 链式 receiver +
    // R2L 实参独占）；size 取包内 packetSize（P171 照抄）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&notice), notice.packetSize);
}

// ========================================================================
// DungeonDifficulty —— sizeof=0xc；+0x08 byte 难度（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a8892。

// ORIG parse 0x084b2110：难度字节清零后 atoi(args) 写入 +0x08。
#include "GmCmds_C/parse.inc"
{
    m_difficulty = 0;
    trim(m_parser->m_args);
    m_difficulty = static_cast<unsigned char>(atoi(m_parser->m_args.c_str()));
}

// ORIG execute 0x084b2152：遍历全地下城索引表，为非特殊地下城写入通关难度记录。
#include "GmCmds_C/execute_4.inc"
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    // ORIG 0x084b2172/0x084b2181：m_difficulty 每次直读成员（无具名局部，P0）；
    // 原实现多一个 unsigned char 具名局部（-0x19 槽 + store/load 往返）。
    if (m_difficulty == 0 || m_difficulty > 3)
    {
        return;
    }

    // ORIG 0x084b2190 起：list 为 const 指针（begin/end 选 const 重载
    // _ZNK…hash_map…5beginEv/3endEv），且 end 只在循环外求值一次（-0x2c 槽）。
    // [round5-GmCmds_C 判词·N-P75 封顶] 全件唯一残差=ORIG 0x084b21a0/0x084b21b5 的
    // `lea -0x24/-0x2c(%ebp),%eax`（sret 槽地址）先于 0x084b21a3/0x084b21b8 的
    // `mov -0x10(%ebp),%edx`（this）：4.1 sret-first vs 4.4 this-first（实参装载调度）。
    // 三形实测均不可翻：①const 指针形（基线）逐条如上；②引用形（P256「先取整体引用
    // 再点首成员」）逐条相同；③`const CDungeonList* volatile`（P316 左先求值序）反致
    // list 槽 -0x10→-0x1c 且多 1 条 `mov %eax,%edx`（117→118 条）⇒ 回滚。
    // 剩余 2 处为纯寄存器名/次序差（N7/N-P75），语义与指令数 117=117 全等。
    const CDungeonList* list = G_CDataManager()->getDungeonIndexList();
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator it =
        list->m_dungeonMap.begin();
    __gnu_cxx::hash_map<int, CDungeon*>::const_iterator end =
        list->m_dungeonMap.end();
    // ORIG 增量段调 ppEi（后置）⇒ 源形写 it++（P28）。
    for (; it != end; it++)
    {
        CDungeon* dungeon = it->second;
        if (dungeon == 0)
        {
            continue;
        }
        // 血色地狱(1/2)、绝望之塔、锦标赛、kind==1 的地下城不写入。
        // ORIG 0x084b21f5..0x084b2244：OR 短路链物化 %al（mov$1/mov$0 双岛 +
        // test; jne 跳过 body，无具名 bool 槽）⇒ 4.4 复现形是正极性 AND 包裹
        // （P214①：负极性短路恒 je;jne 反演）。
        if (({ dungeon->get_blood_dungeon_type() == 1 ||
               dungeon->get_blood_dungeon_type() == 2 ||
               dungeon->isTowerOfDespairDungeon() ||
               dungeon->isTournamentDungeon() ||
               dungeon->getDungeonKind() == 1; }))
        {
            continue;
        }
        // ORIG 0x084b2246 起：getDungeonClearRefW 结果为 receiver 直传
        // （无具名 clear 局部）；实参 R2L（difficulty → get_index）。
        user->getDungeonClearRefW()->addClearedDungeon(
            static_cast<short>(dungeon->get_index()),
            static_cast<char>(m_difficulty));
    }
}

// ========================================================================
// CEndEvent —— sizeof=0xc；+0x08 int 事件代码（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a845e。

// ORIG parse 0x084ab914：atoi(args) → +0x08。
#include "GmCmds_C/parse_1.inc"
{
    trim(m_parser->m_args);
    m_eventCode = atoi(m_parser->m_args.c_str());
}

// ORIG execute 0x084ab94c：GM 校验后向 monitor 发 Packet_Monitor_Event_End。
#include "GmCmds_C/execute_5.inc"
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }
    Packet_Monitor_Event_End packet;
    packet.m_eventCode = static_cast<unsigned int>(m_eventCode);
    // ORIG：GetServerProxy 结果 eax 直通 receiver（无 proxy 具名槽）；packet 基址
    // 跨 call 物化 %ebx 为 4.1 调度产物。N96/P0：register asm 绑定全域禁用，
    // 保持自然表达式直传（残差记 N18a/N7 族）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&packet), 0xe);
}

// ========================================================================
// CEnduranceCmd —— sizeof=0x10；+0x08 int 槽位、+0x0c unsigned short 耐久
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a83fc。

// ORIG parse 0x084aa73a：trim 后以首个空格切分 args；
// 前段 atoi → 槽位(+0x08)，后段 atoi → 耐久(+0x0c)。ORIG 未校验 find 失配。
#include "GmCmds_C/parse_2.inc"
{
    trim(m_parser->m_args);
    int pos = static_cast<int>(m_parser->m_args.find(" ", 0));
    std::string slotStr = m_parser->m_args.substr(0, pos);
    std::string enduranceStr = m_parser->m_args.substr(pos + 1, std::string::npos);

    m_slot = atoi(slotStr.c_str());
    m_endurance = static_cast<unsigned short>(atoi(enduranceStr.c_str()));
}

// ORIG execute 0x084aa858：GM 且槽位 ∈ (9,0x15]（cmp $0x15;jg 允许 ==0x15）时改写背包物品耐久并回发更新包。
#include "GmCmds_C/execute_6.inc"
{
    // ORIG 0x084aa866 起只有单个具名局部（-0x10 槽，GetUser 结果，P0）；原实现
    // 多声明一个 CUserCharacInfo* 具名局部，多出 -0x14 槽 + 两条 mov 往返。
    CUser* user = GetUser();
    // [b28d-63] ORIG 0x084aa879 起：bool 取反守卫 + 两次独立槽位比较各落
    // 独立 nop 岛（0x15f/0x162/0x165/0x168）⇒ P223 负条件 goto + 尾标签串；
    // m_slot 每次直读（无具名局部，P0）。
    if (!user->isGMUser())
        goto END1;
    if (m_slot <= 9)
        goto END2;
    if (m_slot > 0x15)
        goto END3;

    {
        Inven_Item* item =
            const_cast<CInventory*>(
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW())
                ->GetInvenRef(0, m_slot);
        // ORIG 0x084aa8ca：item==0 直跳 epilogue（0x084aa9c1，无 nop 岛）⇒ 原码是
        // 正向 `if (item != 0) { ... }` 包裹（写成 if(item==0)return 会多一个岛）。
        if (item != 0)
        {
            if (item->m_addInfo == 0)
                goto END4;

            item->m_fieldb = m_endurance;

            PacketGuard packet;
            packet.put_header(0, 0xe);
            packet.put_byte(3);
            packet.put_short(1);
            static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                ->MakeItemPacket(INVEN_TYPE_INVEN, m_slot, packet);
            packet.finalize(true);
            user->Send(packet);
        }
    }
END1:;
END2:;
END3:;
END4:;
}

// ========================================================================
// CGrowhWeaponEvent —— sizeof=0x10；+0x08 byte "gw" 重置标记、+0x0c int 时计值
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a88e6。

// ORIG parse 0x084b28d6：args=="gw" 时置重置标记，否则标记清零并 atoi 存时计值。
#include "GmCmds_C/parse_3.inc"
{
    trim(m_parser->m_args);
    if (m_parser->m_args == "gw")
    {
        m_resetFlag = 1;
    }
    else
    {
        m_resetFlag = 0;
        m_timepiece = atoi(m_parser->m_args.c_str());
    }
}

// ORIG execute 0x084b293c：改写成长武器事件数据并落库——
//   标记置位：整体清零事件数据 + DB_EventGrowthWeaponReset(accId)；
//   否则：写时计值/清计数 + DB_EventUpdateGrowthWeaponTimepiece(accId, value, 0)。
#include "GmCmds_C/execute_7.inc"
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    GrowthWeaponEvent* data =
        static_cast<GrowthWeaponEvent*>(user->getGrowthWeaponEventdata());
    if (m_resetFlag)                 // [0x084b296c 字段直测，无具名局部 P0]
    {
        data->m_field8 = 0;
        data->m_field4 = 0;
        data->m_eventFlag = 0;
        data->m_field10 = 0;
        data->m_fieldc = 0;
        DB_EventGrowthWeaponReset::makeRequest(user->get_acc_id());
    }
    else
    {
        data->m_field10 = static_cast<short>(m_timepiece);   // [0x084b29b6 直读]
        data->m_fieldc = 0;
        DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
            user->get_acc_id(),
            static_cast<short>((unsigned short)data->m_field10), 0);
    }
}

// ========================================================================
// CGuildDebug —— sizeof=0x14；+0x08/+0x0c int 参数、+0x10 unsigned int（m_f1 占位）。
// ========================================================================

// ORIG ctor 0x082a8592。

// ORIG parse 0x084ac720：atoi(args) → +0x08；+0x0c、+0x10 显式清零。
#include "GmCmds_C/parse_4.inc"
{
    trim(m_parser->m_args);
    m_param1 = atoi(m_parser->m_args.c_str());
    m_param2 = 0;
    m_f1 = 0;  // +0x10（ORIG 无其它写点，恒为 0）
}

// ORIG execute 0x084ac76c：组 Packet_Guild_Debug 经 guild 代理 SendTcpPacket
//（ORIG 无 isGMUser 校验）。size 取包内 0x1e。
#include "GmCmds_C/execute_8.inc"
{
    CUser* user = GetUser();
    Packet_Guild_Debug packet;
    packet.m_charNo = static_cast<unsigned int>(user->get_charac_no(-1));
    packet.m_guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    packet.m_mode = m_param1;
    packet.m_field16 = m_param2;
    packet.m_field1a = m_f1;  // +0x10

    // ORIG 0x084ac7cc：size 读包内 packetSize（双段 movzwl），&packet 与 size 先物化
    // %ebx/%esi 跨 GetServerProxy call 保活（P52 链式 receiver + R2L 实参独占）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
}

// ========================================================================
// CGuildExpBookDelete —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8506。

// ORIG execute 0x084abf2c：GetUser()->deleteGuildExpBook()。
#include "GmCmds_C/execute_9.inc"
{
    CUser* user = GetUser();
    user->deleteGuildExpBook();   // 真实定义 UserPartyLink4.cpp（T 0x0867e898）
}

} // namespace game_master