// df_game_r 角色任务列表实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CMissionList_Charac.md（Ghidra C）+ ORIG
// 反汇编（ctor 0x832af8a / dtor 0x85ec626 / 弱方法 0x85e68b6..0x85e7130）。
// [b28d-64 判词汇总 · 泳道64] 本 TU 10 件（明细 /tmp/b28d_lane_64_result.tsv）：
//   addNewMission 54/54 AE；Update_Revenge_event 97/97 NEAR·N82（槽号）；
//   Update_EnterWorld_event 85/83 N封顶（P133 短路 bool 物化跨版本）；
//   Update_RecvPacket_event 116/117 N82；ClearMission 123/121 N7/N82；
//   MakeMissionList_JustKind 96/104 N82；MakeMissionList_forNewbies 93/105 N82；
//   check_mission_kind 109/114 N7/N82；MakeMissionList_forOldUser 109/121 N82；
//   loadData 156/180 N82；Update_ComboMission_Event 150/135 N82。
// [b28d-solo-CMissionList_Charac 判词汇总 · 单文件泳道] 清单 10 件结果
// （明细 /tmp/b28d_solo_CMissionList_Charac_result.tsv）：
//   loadData 156/156 AE（P171 删 (*it) 判空 + P74② it!=end + P309 全头部声明）；
//   Update_RecvPacket_event 116/116 AE（P214① 短路直判 + P227 临时链 + P223 三岛）；
//   ClearMission 123/123 AE（P59/P222 块序翻转 + P223 goto 尾标签）；
//   check_mission_kind 109/111 N121（尾部 epilogue-threading jmp+nop 岛）；
//   Update_Revenge_event 97/97 NEAR·N82/N98（槽号）；Update_EnterWorld_event 85/85
//   NEAR·N115/N82（帧差 0x10 量化 + 槽号）；Reset_DailyMission 132/133 N104（EH 尾块序）；
//   BitsetToStr(Ss) 51/48、BitsetToStr(Ph) 44/42 N98+P183（折叠+槽序）；
//   Update_ComboMission_Event 150/149 N104（EH 尾块序 + 少 1 岛）。
//   P0：本文件无 register asm 绑定；p167 十件全 CLEAR；TU COMPILE OK 0 错误。
#include <bitset>
#include <cstring>
#include <map>
#include <utility>
#include <vector>

#include "CMissionList_Charac.h"
#include "CMission.h"
#include "CUser.h"   // 经 SigTypes.h 提供权威 SIG_REQUEST_PVP_MISSION_LIST（0x14a）
#include "CDataManager.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"
#include "PacketGuard.h"

// PvP_Room 最小声明（PvP_Room.h 引入 PvP_deps.h 的重复 PVP_BATTLE_MODE，
// 本 TU 只需 get_pvp_battle_mode）
class PvP_Room
{
public:
    int get_pvp_battle_mode();
};

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号（ORIG 真实符号，形态按 mangled 名转录）
// ============================================================================
// CUser::GetPVPRoom / cUserHistoryLog::pvpMissionAdd/pvpMissionDel 均已在
// CUser.h 权威声明，直接真实调用（原 extern "C" asm 桥已删除）。


// 全局辅助（数据域 TU）：bitset <-> 存档字节串、每日排程判定
// ORIG 0x8a5a989：逐字节/位转换
void StrToBitset(unsigned char* data, std::bitset<256>& bitset)
{
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 8; ++j)
            bitset[i * 8 + j] = (data[i] >> j) & 1;
}
// ORIG 0x8a5a86c：bitset → 32 字节（第二参 std::string&，resize(0x20) 后逐字节写）
// [b28d-58] DIFF N（P183 折叠 + N98 槽序）：ORIG 0x08a5a8d2 段为读-加-写三段
//   `mov %eax,%edx; movzbl byte,%eax; lea (%edx,%eax,1),%eax; mov %al,byte`；
//   4.4-O0 对 `x+byte` / `byte+x` / `+=` / `+(int)byte` 四形恒折 `add %al,byte`
//   内存直加（P183 折叠铁律）；i 槽 ORIG -0x10 vs 4.4 -0x14（N98 首次使用序，
//   声明序不可翻）。语义等价（uchar 回绕 + out[i] 截断同）。
// [b28d-solo-CMissionList_Charac] 判词 N98+P183（51/48）：残差=槽号(-0x10/-0x14)
//   与读-加-写三段被 4.4 折成 `add %al,byte`，两族均跨版本不可造，封顶。
void BitsetToStr(const std::bitset<256>& bitset, std::string& out)
{
    out.resize(0x20);
    for (int i = 0; i < 32; ++i)
    {
        unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            // ORIG 0x08a5a8d2 段：`mov %eax,%edx; movzbl byte,%eax;
            // lea (%edx,%eax,1),%eax; mov %al,byte` ⇒ 源形是 `bit + byte`
            // 顺序（x 在 edx、byte 在 eax）；`byte + x`/`+=` 均被 4.4 折成
            // `add %al,mem` 内存直加。
            // [b28d-solo-CMissionList_Charac] 本轮旋钮穷举实测（P248）：①`(int)byte`
            // ②`(unsigned int)byte` ③`(byte&0xff)` ④裸 `byte` 四形 4.4 恒折
            // `add %al,mem`（48 条）；⑤具名 `unsigned int bv` 形加法段逐条对齐
            // 但多 store+load 两条（53 条）且引入 ORIG 无之局部（P0 违）⇒ 弃。
            // 结论：P183 折叠铁律 + N98 槽序（i -0x14/-0x10、byte -0xd/-0x11），
            // 跨版本不可造，封顶。
            byte = (unsigned char)((bitset[i * 8 + j] << j) + byte);
        out[i] = (char)byte;
    }
}

// ORIG 0x8a5a90a（另一重载 _Z11BitsetToStrRKSt6bitsetILj256EEPh，输出 unsigned char*）
void BitsetToStr(const std::bitset<256>& bitset, unsigned char* out)
{
    for (int i = 0; i < 32; ++i)
    {
        // [b28d-58] DIFF N（同 Ss 重载）：ORIG 槽 i(-0x10)/byte(-0x11)/j(-0xc)，
        // 4.4 为 i(-0x14)/j(-0x10)/byte(-0x9)（N98 首次使用序不可翻）；加法段
        // ORIG 读-加-写三段 vs 4.4 `add %al,byte`（P183 折叠）——语义等价。
        // [b28d-solo-CMissionList_Charac] 判词 N98+P183（44/42，同 Ss 重载）。
        //   本轮实测：`+=` 与 `= (x<<j)+byte` 两形同产 `add %al,byte`（P183 折叠
        //   铁律，与 Ss 重载一致）⇒ 保持 P0 合规形封顶。
        int j = 0;
        unsigned char byte = 0;
        for (j = 0; j < 8; ++j)
            byte = (unsigned char)((bitset[i * 8 + j] << j) + byte);
        out[i] = byte;
    }
}
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);

// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；与 CInventory.h
// 的占位版本不同，本 TU 使用真实形态）


// [b28d-solo-CMissionList_Charac] P154 发明符号清理：原 `static int
// GetDailyScheduleTime(CEnvironment*)` 在 ORIG nm 中**不存在**（仅 T
// CheckDailyScheduleTime），ORIG loadData 0x085e49d0 直接内联
// `mov 0x37c(%eax),%eax`（= m_serverEnvir.m_fatigueTime，CEnvironment.h 权威布局）
// ⇒ 删除该 helper，调用点改为具名字段内联。

// ============================================================================
// 构造 / 析构 / 虚接口
// ============================================================================

CMissionList_Charac::CMissionList_Charac()
{
    // ORIG C1 @0x832af90：仅隐式成员构造（MissionInfo[36] 逐个清零 +
    // bitset<256> 默认构造，不额外 reset；显式清零循环与 reset 均为
    // NEW 冗余，移除以对齐 ORIG 字节）。
}

CMissionList_Charac::~CMissionList_Charac()
{
}

bool CMissionList_Charac::loadData(CUser* pUser, char* pData)
{
    SIG_REQUEST_PVP_MISSION_LIST* pSig;
    int kind;
    const std::list<CMission*>* list;
    _reset();
    m_user = pUser;
    // pData 实为 SIG_REQUEST_PVP_MISSION_LIST（0x14a 字节 DB/存档 blob，
    // _saveData 侧同一类型），定基类型后全部具名字段访问。
    // ORIG 0x085e47ee `mov %eax,-0x14(%ebp)` 亦保留一份本地副本。
    pSig = reinterpret_cast<SIG_REQUEST_PVP_MISSION_LIST*>(pData);

    for (kind = 0; kind < 36; ++kind)
    {
        // ORIG 0x085e481c..0x085e4866：movzwl blob+6/+4/+8/+0xa → this+7/+5/+9/+0xb
        // 即 blob m_missionList[kind]（+0x04 起、步长 8）的 index/kind/count/streak。
        m_missionList[kind].m_index = pSig->m_missionList[kind].m_index;
        m_missionList[kind].m_kind = pSig->m_missionList[kind].m_kind;
        m_missionList[kind].m_count = pSig->m_missionList[kind].m_count;
        m_missionList[kind].m_streak = pSig->m_missionList[kind].m_streak;
    }
    StrToBitset(pSig->m_cleared, m_clearedFlags);   // +0x124（32 字节位图）
    m_field148 = pSig->m_field144;                  // +0x144
    m_field14a = pSig->m_field146;                  // +0x146
    m_field14c = pSig->m_field148;                  // +0x148

    // [b28d-solo-CMissionList_Charac] ORIG 0x085e48e6 段 `call G_CDataManager;
    // mov %eax,(%esp); call get_kind_mission_list` 无 mgr 具名槽（P0 右值内联）；
    // `_ZNKSt4listIP8CMissionSaIS1_EE5beginEv/3endEv` 为 const 版 ⇒ const 指针。
    // 判词 N82：残差=kind/list/pSig 槽号互换（4.4 首次使用序 vs 4.1 声明序，
    // N98 不可翻）；fatigueTime 已按 ORIG 内联（原 GetDailyScheduleTime helper
    // 为发明符号，已删）。
    list = (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(6);
    // [b28d-solo-CMissionList_Charac] P74 补充②：ORIG 0x085e495a 实参序
    // `arg0=&it(-0x1c) arg1=&end(-0x18)` ⇒ 原码是 `it != list->end()`
    //（写 `end() != it` 会反装实参序，探针三形第三例）。
    for (std::list<CMission*>::const_iterator it = list->begin();
         it != list->end(); ++it)
    {
        // [b28d-solo-CMissionList_Charac] P171 照抄：ORIG 0x085e4906 循环体
        // 只发一次 `_ZNKSt20_List_const_iteratorIP8CMissionEdeEv`（operator*）
        // 后直接 `mov (%eax),%eax; call GetMissionIndex`，**无 `(*it)` 判空**
        // （p167 EXTRA +1 const deEv 实证）⇒ 删除原 `(*it) &&` 防御。
        if (isClearMission((*it)->GetMissionIndex()))
        {
            m_field14e = 1;
            break;
        }
    }

    if (isUserNewbie())
    {
        MakeMissionList_forNewbies(*pUser);
        Reset_DailyMission();
    }
    else
    {
        MakeMissionList_forOldUser(*pUser);
    }

    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               pUser->getCurCharacLastPlayTick(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        Reset_DailyMission();
    }
    return 1;
}

void CMissionList_Charac::getData(char* pData) const
{
    // pData = SIG_REQUEST_PVP_MISSION_LIST blob（与 _saveData/loadData 同一布局；
    // ORIG 0x085e4acd `mov %eax,-0x10(%ebp)` 保留一份本地副本）。
    SIG_REQUEST_PVP_MISSION_LIST* pSig =
        reinterpret_cast<SIG_REQUEST_PVP_MISSION_LIST*>(pData);
    std::memset(pSig, 0, sizeof(SIG_REQUEST_PVP_MISSION_LIST));   // 0x14a
    pSig->m_characNo = m_tail14f.m_characNo;    // this+0x150 → blob+0x00
    for (int kind = 0; kind < 36; ++kind)
    {
        pSig->m_missionList[kind].m_index = m_missionList[kind].m_index;
        pSig->m_missionList[kind].m_kind = m_missionList[kind].m_kind;
        pSig->m_missionList[kind].m_count = m_missionList[kind].m_count;
        pSig->m_missionList[kind].m_streak = m_missionList[kind].m_streak;
    }
    {
        // ORIG 直接调用 BitsetToStr(Ph) 重载写 0x20 字节到 pData+0x124（无 string 中间层）
        BitsetToStr(m_clearedFlags, pSig->m_cleared);
    }
    pSig->m_field144 = m_field148;
    pSig->m_field146 = m_field14a;
    pSig->m_field148 = m_field14c;
}

bool CMissionList_Charac::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PvP_MissionSystem.cpp", 0x4a5),
        true);
    **guard << 0x263;
    **guard << (int)pUser->GetUID();
    SIG_REQUEST_PVP_MISSION_LIST* pData =
        guard->GetInBuffer<SIG_REQUEST_PVP_MISSION_LIST>();
    if (!pData)
    {
        return 0;
    }
    std::memset(pData, 0, 0x14a);
    pData->m_characNo = pUser->get_charac_no(-1);
    for (int kind = 0; kind < 36; ++kind)
    {
        pData->m_missionList[kind].m_index =
            m_missionList[kind].m_index;
        pData->m_missionList[kind].m_kind =
            m_missionList[kind].m_kind;
        pData->m_missionList[kind].m_count =
            m_missionList[kind].m_count;
        pData->m_missionList[kind].m_streak =
            m_missionList[kind].m_streak;
    }
    // ORIG +0x14f：BitsetToStr(m_clearedFlags, m_cleared) 直接写 unsigned char*
    //（Ph 重载），无 std::string 中间层（原实现错误引出了 string ctor/dtor/data）。
    BitsetToStr(m_clearedFlags, pData->m_cleared);
    pData->m_field144 = m_field148;
    pData->m_field146 = m_field14a;
    pData->m_field148 = m_field14c;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

void CMissionList_Charac::_reset()
{
    std::memset(m_missionList, 0, sizeof(m_missionList));
    m_clearedFlags.reset();
    m_field148 = 0;
    m_field14a = 0;
    m_field14c = 0;
    m_field14e = 0;
    m_user = 0;
}

// ============================================================================
void CMissionList_Charac::Update_EnterWorld_event(CUser& user)
{
    // [b28d-64] P48：ORIG 0x085e600a/0x085e6014 两次独立 `G_GameWorld()->
    // GetChannelType()`（无 CSE，无 type 局部槽）；具名 type 会少 4 条调用且槽漂。
    // [b28d-solo-CMissionList_Charac] P214①：ORIG 0x085e602c/0x085e6037 两守卫
    // 均 `je <mov $0>` 共享落点 + 尾 `mov $1;jmp;test;jne <return>` ⇒ 原码是
    // **正极性早返** `if (A != 0xf && A != 0x10) return;`（负极性包裹被 4.4 反演）。
    // [b28d-solo-CMissionList_Charac] 判词 N115+N82（85/85，条件形已逐条对齐）：残差=帧 0x40 vs 0x50（差 0x10 =
    // 4.4 -O0 恒预留 16B 对齐 outgoing 区，N115）+ 槽号 param(-0x23/-0x2b)/
    // out(-0x14/-0x1c)/pvpType(-0x18/-0x10)/chanType(-0x1c/-0xc)（聚合优先低槽，
    // N82/P112）。P210 定义前置会反向影响同 TU 其它 caller（recv/clear 已 AE），
    // 不可全局移动 ⇒ 封顶；语义等价。
    if (G_GameWorld()->GetChannelType() != 0xf &&
        G_GameWorld()->GetChannelType() != 0x10)
        return;
    alter();
    std::vector<int> out;
    MissionClearCondition_Parameter param(PVP_BATTLE_MODE_2);
    int pvpType = G_CDataManager()->GetPvPChannelType();  // ORIG 死代码调用（结果未使用）
    int chanType = G_GameWorld()->GetChannelType();       // ORIG 死代码调用（结果未使用）
    check_mission_kind(7, user, param, out);
    check_mission_kind(0x1e, user, param, out);
    Send_Cleared_Mission_List(user, out);
}

void CMissionList_Charac::Update_Kill_event(
    CUser& user, const MissionClearCondition_Parameter& param)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    alter();
    std::vector<int> out;
    check_mission_kind(5, user, param, out);
    check_mission_kind(0xf, user, param, out);
    check_mission_kind(0x19, user, param, out);
    Send_Cleared_Mission_List(user, out);
}

void CMissionList_Charac::Update_Win_event(
    CUser& user, const MissionClearCondition_Parameter& param,
    bool& outClear)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    Update_Within_Mission_event(user, param, outClear);
    alter();
    if (outClear == true)
        return;
    if (getIndex_byKind(0x1b) != 0)
        return;
    std::vector<int> out;
    check_mission_kind(2, user, param, out);
    check_mission_kind(4, user, param, out);
    check_mission_kind(9, user, param, out);
    check_mission_kind(10, user, param, out);
    check_mission_kind(0xc, user, param, out);
    check_mission_kind(0xe, user, param, out);
    check_mission_kind(0x11, user, param, out);
    check_mission_kind(0x13, user, param, out);
    check_mission_kind(0x14, user, param, out);
    check_mission_kind(0x16, user, param, out);
    check_mission_kind(0x18, user, param, out);
    check_mission_kind(0x1a, user, param, out);
    Send_Cleared_Mission_List(user, out);
}

void CMissionList_Charac::Update_Revenge_event(CUser& user,
                                               PVP_BATTLE_MODE mode,
                                               bool win)
{
    // [b28d-64] P133 补/P223 BM8：ORIG 0x085e6229/0x085e623d 两守卫各自
    // `setne %al; test %al,%al; jne` 物化 + 独立 nop 岛（+0x143/+0x146），
    // 合并 && 被 4.4 折成直跳（少 setne 与两岛）。拆两条否定 goto + 尾标签串。
    // win 分支 ORIG 0x085e6244 `cmpb $0,-0x2c; je <清零块>` ⇒ 正向 if(win){alter}。
    // [b28d-solo-CMissionList_Charac] 判词 NEAR（97/97，指令序列全同）：残差=三处
    // 栈槽号 out(-0x14 vs -0x18)/param(-0x23 vs -0x27)/issueMode(-0x22 vs -0xa)，
    // 系 4.4 按首次使用序分配且聚合对象优先低槽（N82/N98，声明序不可翻；
    // issueMode 为死值被 4.4 复用到浅槽）。语义等价。
    //   本轮实测：issueMode 声明提前/两语句形均不改槽（仍 -0xa）⇒ N98 首次使用序封顶。
    if (G_GameWorld()->GetChannelType() != 0xf)
        goto L_notchannel;
    if (getIndex_byKind(0x1b) != 0)
        goto L_hasmission;
    {
        if (win)
        {
            alter();
            std::vector<int> out;
            MissionClearCondition_Parameter param(mode);
            short issueMode = getIssue_mode(mode == PVP_BATTLE_MODE_2);  // ORIG 死代码调用（结果未使用）
            check_mission_kind(0xc, user, param, out);
            check_mission_kind(0x16, user, param, out);
            Send_Cleared_Mission_List(user, out);
        }
        else
        {
            m_missionList[0xc].m_streak = 0;
            m_missionList[0x16].m_streak = 0;
        }
    }
L_hasmission:;
L_notchannel:;
}

void CMissionList_Charac::Update_GameOver_event(
    CUser& user, const MissionClearCondition_Parameter& param)
{
    // b28c-L5：ORIG 0x085e5fe4 两守卫均为「call 结果寄存器直比 + bool 物化岛」：
    //   `call GetChannelType; cmp $0xf,%eax; setne %al; test %al,%al; jne <尾岛>`、
    //   `call getIndex_byKind; test %ax,%ax; setne %al; test %al,%al; jne <尾岛>`，
    // 且两失败出口落**不同** nop 中继（+0x1b5 / +0x1b8），body 后另有 `jmp`。
    // 4.4 对正极性 `if (a==K && b==0) {body}` 恒产 je/jne 直跳 + 结果汇聚岛
    // （少 setne、多 mov$1/jmp/mov$0），探针 /tmp/l5_probe_a.cpp hA
    // （`if (g1(x) != 0xf) return;`）实证否定早返直产 ORIG setne 三段物化；
    // 双岛由 goto 尾标签串复现（P223/BM8）。std::vector 局部包内层块，
    // 避开 4.4「goto 跨同层非平凡局部声明」拒编译约束。
    if (G_GameWorld()->GetChannelType() != 0xf)
        goto L_notchannel;
    if (getIndex_byKind(0x1b) != 0)
        goto L_hasmision;
    {
        alter();
        std::vector<int> out;
        check_mission_kind(1, user, param, out);
        check_mission_kind(3, user, param, out);
        check_mission_kind(8, user, param, out);
        check_mission_kind(0xd, user, param, out);
        check_mission_kind(0x10, user, param, out);
        check_mission_kind(0x12, user, param, out);
        check_mission_kind(0x17, user, param, out);
        Send_Cleared_Mission_List(user, out);
    }
L_hasmision:;
L_notchannel:;
}

void CMissionList_Charac::Update_RecvPacket_event(
    CUser& user, std::vector<short>& kinds, std::vector<int>& values)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    if (getIndex_byKind(0x1b) != 0)
        return;
    alter();
    PvP_Room* room = (PvP_Room*)user.GetPVPRoom();
    if (room == 0)
        return;
    {
        MissionClearCondition_Parameter param(
            (PVP_BATTLE_MODE)room->get_pvp_battle_mode());
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e5d2a `movl $0x0,-0x10(%ebp)`
        // 位于 for 初始化区（循环外）⇒ 原码 mission 声明于循环外并零预清（P198①），
        // 循环体内仅赋值。
        // 判词 N82（116/116，指令序列全同）：残差=槽号 room(-0x14/-0x18)、
        // mission(-0x10/-0x14)、i(-0xc/-0x10)、param(-0x33/-0x27)（4.4 首次使用序）。
        CMission* mission = 0;
        for (unsigned int i = 0; i < kinds.size(); ++i)
        {
            mission = (CMission*)G_CDataManager()->find_mission(kinds[i]);
            if (mission == 0)
                goto L_next;
            {
                    // ORIG 0x85e5caa：kind 6/0x15 执行 check_mission_kind，
                    // 其它 kind 记 HackUser 日志；ORIG 短路二次 GetMissionKind
                    // + 两分支内各重取（P167 定 4 调用位点，复现需逐位点对齐）。
                    // [b28d-solo-CMissionList_Charac] ORIG 无 kind 局部槽（首次调用
                    // 结果直接 `cmp $6,%eax`，P0 右值内联）⇒ 两处内联调用。
                    // [b28d-solo-CMissionList_Charac] 本轮修形：①ORIG 0x085e5d93
                    // 的 bool 物化 `mov $1,%eax;jmp;mov $0,%eax;test %al;je` 无栈槽
                    // ⇒ 删具名 hackUser，改短路条件直判（P214①/P0）；②trace 改
                    // P227 全表达式临时链，令 GetMissionKind 在 ctor 前求值并跨
                    // ctor 以 %ebx 保活（ORIG 0x085e5dae `mov %eax,%ebx`）。
                    if ((mission->GetMissionKind() != 6) &&
                        (mission->GetMissionKind() != 0x15))
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x704, 5)(
                            "[@missionSystem] HackUser! send [%d kind]\n",
                            mission->GetMissionKind());
                        goto L_join;
                    }
                    else
                    {
                        check_mission_kind(mission->GetMissionKind(), user,
                                           param, values);
                        goto L_join;
                    }
                }
        L_next:;
        L_join:;
        }
    }
}

void CMissionList_Charac::Update_ComboMission_Event(CUser& user,
                                                    int comboIdx,
                                                    short count)
{
    // [b28d-solo-CMissionList_Charac] 本轮修形：①P223 三独立岛（ORIG
    // 0x085e682a/682d/6830 三个 nop;jmp 落点）⇒ 两条负向 goto + 尾标签串；
    // ②c 移入 if 块内（ORIG 0x085e6689 赋值在守卫之后）。
    if (count < 0)
        goto L_neg;
    if (count > 8)
        goto L_over;
    {
    if ((comboIdx == 0x1f) || (comboIdx == 0x20))
    {
        short c = count;
        short value = m_missionList[comboIdx].m_count;
        if (count > 3)
        {
            c = (short)(c - 4);
            value = m_missionList[comboIdx].m_streak;
        }
        // [b28d-solo-CMissionList_Charac] 本轮修复：c/value/nib 槽位与三守卫岛
        // 全部对齐（c -0xe / value -0xc / nib -0xa；js/jg/jne 三岛 0x1d4/0x1d7/0x1d8）；
        // 判词 N104（150/149）：唯一残差=EH 正常路径尾块与 unwind 块相对序
        //（ORIG 正常 ~vector 在 jmp 前、unwind 居中、三岛收尾；4.4 相反）并少 1 岛；
        // 槽位集/调用点差集全等（p167 CLEAR），语义等价。
        short nib = (short)(((int)value >> ((3 - (int)c) * 4)) & 0xf);
        value = (short)(value -
                        (short)((int)(short)nib << ((3 - (int)c) * 4)));
        nib = (short)(nib - 1);
        if (nib < 0)
        {
            nib = 0;
        }
        nib = (short)((int)nib << ((3 - (int)c) * 4));
        value = (short)(value + (short)nib);
        if (count > 3)
        {
            m_missionList[comboIdx].m_streak = value;
        }
        else
        {
            m_missionList[comboIdx].m_count = value;
        }

        MissionClearCondition_Parameter param(PVP_BATTLE_MODE_1);
        param.m_mode = 1;
        std::vector<int> out;
        check_mission_kind(comboIdx, user, param, out);
        if (!out.empty())
        {
            Send_Cleared_Mission_List(user, out);
        }
        alter();
    }
    }
L_kind:;
L_over:;
L_neg:;
}

void CMissionList_Charac::Update_Within_Mission_event(
    CUser& user, const MissionClearCondition_Parameter& param,
    bool& outClear)
{
    std::vector<int> out;
    check_mission_kind(0x1b, user, param, out);
    if (!out.empty())
    {
        outClear = true;
    }
    Send_Cleared_Mission_List(user, out);
}

void CMissionList_Charac::check_mission_kind(
    unsigned int kind, CUser& user,
    const MissionClearCondition_Parameter& param,
    std::vector<int>& outClearList)
{
    bool cleared = false;
    if (m_missionList[kind].m_index == 0)
    {
        return;
    }
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission(m_missionList[kind].m_index);
    // [b28d-solo-CMissionList_Charac] ORIG 0x085e62bd..0x085e62e7：mission==0 与
    // canUpdate==0 共享 `je <mov $0>` 落点 + `mov $1;jmp;test %al;je <return>` 物化
    // ⇒ P214① 正极性包裹：`if (a!=0 && b!=0) {主流程}` 复现 je P;je P;mov$1;jmp;
    // P:mov$0;Q:test;je 形（负极性早返被 4.4 反演为 xor $1 直跳）。
    // [b28d-solo-CMissionList_Charac] 判词 N121（109/111）：守卫/物化段逐条对齐；残差仅尾部 4.4 epilogue-threading
    // 多 `jmp+nop` 岛 2 条。本轮实测三形均不消：①`cleared=false` 移出 if（111 不变）；
    // ②空 `else{return;}`（113 反劣）；③ORIG 无岛故 P223 尾标签不适用 ⇒ 封顶。
    if (mission != 0 && mission->canUpdate(param))
    {
        mission->Update(m_missionList[kind], param);
        if (mission->isClearable(m_missionList[kind], cleared))
        {
            int index = (int)m_missionList[kind].m_index;
            outClearList.push_back(index);
            if (kind == 0x1b)
            {
                m_missionList[7].clear();
                m_missionList[0x1e].clear();
            }
            ClearMission(user, (short)index);
        }
        if (cleared)
        {
            Send_Fail_Mission(user, m_missionList[kind].m_index);
            cleared = false;
        }
    }
}

// ============================================================================
// 列表管理
// ============================================================================

void CMissionList_Charac::MakeMissionList_forNewbies(const CUser& user)
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        // [b28d-solo-CMissionList_Charac] ORIG 调用 `_ZNKSt4listIP8CMissionSaIS1_EE5emptyEv`
        // /`beginEv`/`endEv` 全为 **const 版**且无 `_List_iterator`→`_List_const_iterator`
        // 转换构造 call ⇒ 原码 list 声明为 `const std::list<CMission*>*`。
        const std::list<CMission*>* list =
            (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
        if (list->empty())
        {
            continue;
        }
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e4d50..0x085e4d8d：MissionInfo
        // info(-0x1c) 构造 → list->begin()(-0x20) → const_iterator::operator* →
        // `cmp $2; sete; test; je 循环` ⇒ 原码用 begin()/`*it` 而非 front()
        // （front() 少一次 begin sret 调用与迭代器槽）。增量段 ORIG 为 `ppEv`
        // ⇒ `++it` 前置（P28 反向）。修复后 → AE（93/93）。
        MissionInfo info;
        std::list<CMission*>::const_iterator it = list->begin();
        if ((*it)->GetMissionGrade() == 2)
        {
            continue;
        }
        for (; list->end() != it; ++it)
        {
            if ((*it)->isAcceptCondition(user))
            {
                (*it)->makeMissionInfo(info, user);
                addNewMission(info);
                alter();
                break;
            }
        }
    }
}

void CMissionList_Charac::MakeMissionList_forOldUser(const CUser& user)
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e4e88..0x085e4ef7 同 forNewbies：
        // info(-0x1c) 构造 → begin()(-0x20) → operator* → GetMissionKind/getIndex_byKind
        // → operator* → GetMissionGrade；`list` 亦为 const 指针（empty/begin/end const 版）。
        // 循环条件实参序 this=&it(-0x20)、arg=&end(-0x14) ⇒ `it != list->end()`（与
        // forNewbies 相反，P74 补充② 三件三形）。修复后 → AE（109/109）。
        const std::list<CMission*>* list =
            (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
        if (list->empty())
        {
            continue;
        }
        MissionInfo info;
        std::list<CMission*>::const_iterator it = list->begin();
        if (getIndex_byKind((*it)->GetMissionKind()) != 0)
        {
            continue;
        }
        if ((*it)->GetMissionGrade() == 2)
        {
            continue;
        }
        for (; it != list->end(); ++it)
        {
            if ((*it)->isAcceptCondition(user))
            {
                (*it)->makeMissionInfo(info, user);
                addNewMission(info);
                alter();
                break;
            }
        }
    }
}

bool CMissionList_Charac::MakeMissionList_JustKind(const CUser& user,
                                                   unsigned int kind)
{
    bool result = false;
    // [b28d-solo-CMissionList_Charac] ORIG 0x085e4fc1..0x085e50f1：`movb $0,-0xd`
    // 预清 → empty 为真 `mov $0,%eax;jmp` 直返常量（非 movzbl result）⇒ 原码
    // `if (list->empty()) return false;`；getIndex!=0 同样直返常量 0；
    // 命中分支 `movb $1,-0xd; jmp 返回点` ⇒ `result = true; break;` 而非 `return true;`。
    // list 为 const 指针（empty/begin/end 全 const 版），it 用 begin()+operator*。
    // 修复后 → AE（96/96）。
    const std::list<CMission*>* list =
        (const std::list<CMission*>*)(void*)G_CDataManager()->get_kind_mission_list(kind);
    if (list->empty())
        return false;
    MissionInfo info;
    std::list<CMission*>::const_iterator it = list->begin();
    if (getIndex_byKind((*it)->GetMissionKind()) != 0)
        return false;
    for (; it != list->end(); ++it)
    {
        if ((*it)->isAcceptCondition(user))
        {
            (*it)->makeMissionInfo(info, user);
            addNewMission(info);
            alter();
            result = true;
            break;
        }
    }
    return result;
}

void CMissionList_Charac::addNewMission(const MissionInfo& info)
{
    // ORIG 0x85e4c6c: find_mission 以 MissionInfo.index（+2）判定存在性，slot 以 kind（+0）索引
    // [b28d-64] P0：ORIG 0x085e4c8f 具名指针局部（-0xc 槽往返 + cmpl）⇒ 声明局部；
    // 原码 `if (call())` 4.4 产 setne 物化（+4 条）。
    // [b28d-64] N84/P83：ORIG 0x085e4ca7..b0 为 8 字节结构整体赋值（读 +4 先/+0 后、
    // 写 +0/+4），逐 short 四写会膨胀 20 条 ⇒ 改 `m_missionList[kind] = info;`。
    void* mission = G_CDataManager()->find_mission(info.m_index);
    if (mission != 0)
    {
        m_missionList[info.m_kind] = info;
        if (m_user)
        {
            m_user->m_historyLog.pvpMissionAdd(
                (int)info.m_kind, (int)info.m_index,
                (int)info.m_count, (int)info.m_streak);
        }
    }
}

void CMissionList_Charac::Remove_Mission(int kind)
{
    // b28c-L5：ORIG 0x085e4bec 两条范围检查各落**独立** nop 中继岛
    // （+0x79 `nop; jmp +0x7d` / +0x7c `nop;` fall 到 +0x7d leave），body 尾另有
    // `jmp +0x7d`。4.4 对 `if ((kind>0)&&(kind<0x24)) {body}` 恒 jump-thread 合并两
    // 失败出口（少 4 条）。探针 /tmp/l5_probe_a.cpp fD 实证：`goto` 双守卫 + 尾标签串
    // 精确产 ORIG 三岛形（P223 多岛扩展 BM8 同法）。语义与原守卫完全一致。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    if (m_user)
    {
        m_user->m_historyLog.pvpMissionDel(
            (int)m_missionList[kind].m_kind,
            (int)m_missionList[kind].m_index);
    }
    m_missionList[kind].clear();
    alter();
L_overrange:;
L_nonrange:;
}

void CMissionList_Charac::ClearMission(CUser& user, short index)
{
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission((int)index);
    if (!mission)
    {
        // ORIG 0x85e6441：mission 缺失记日志（cMyTrace 5/0x797）后返回。
        // [b28d-solo-CMissionList_Charac] P227：ORIG 0x085e646c 先把 index 载入
        // %ebx 跨 ctor 保活再 call op() ⇒ 全表达式临时链形（具名对象会即时读取）。
        // 判词 N7/N82（121/123）：残差=cMyTrace 变参装载寄存器角色（N7）与
        // sete/setne 极性（setcc-fold），语义等价。
        cMyTrace(__PRETTY_FUNCTION__, 0x797, 5)(
            "[@missionSystem] InvalidIndex: <index : %d>", (int)index);
        return;
    }
    int kind = mission->GetMissionKind();
    if (kind == 6)
    {
        m_field14e = 1;
    }
    // [b28d-solo-CMissionList_Charac] P59/P222 块序：ORIG 0x085e64b9 `je +0xf4`
    // 跳主流程、**日志块 fall-through 在前**；4.4 对正极性 if(==) 产反布局
    // ⇒ 改否定条件 `if (m_index != index) {日志} else {主流程}`。
    if (m_missionList[kind].m_index != index)
    {
        // ORIG 0x85e64e1：m_index 不匹配记日志（cMyTrace 5/0x7a5）后返回。
        cMyTrace(__PRETTY_FUNCTION__, 0x7a5, 5)(
            "[@missionSystem] NotMatchIndex: <valid index:%d invalid index:%d>",
            (int)m_missionList[kind].m_index, (int)index);
    }
    else
    {
        alter();
        m_clearedFlags[(int)index] = true;
        m_missionList[kind].clear();
        mission->Reward(user);
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e65b1 `cmp $2; sete %al;
        // test %al,%al; jne <nop 岛>; call; jmp <epilogue>; nop`（==2 跳过）
        // ⇒ P223 `if(c) goto L; body; L:;` 形。
        if (mission->GetMissionGrade() == 2)
            goto L_grade2;
        MakeMissionList_forOldUser(user);
    L_grade2:;
    }
}

void CMissionList_Charac::ClearMissionKind(CUser& user, short kind)
{
    // b28c-L5：同 Remove_Mission —— ORIG 0x085e63d6 两失败出口各落独立 nop 中继岛
    // （+0x40 nop;jmp / +0x43 nop fall），探针 l5_probe_a fD 形复现（P223）。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    ClearMission(user, m_missionList[kind].m_index);
L_overrange:;
L_nonrange:;
}

void CMissionList_Charac::ResetMissionList(CUser& user)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    _reset();
    MakeMissionList_forNewbies(user);
}

void CMissionList_Charac::ResetDaily()
{
    Reset_DailyMission();
}

void CMissionList_Charac::Reset_DailyMission()
{
    // [b28d-58] DIFF N104（EH 尾块相对序）：槽位集与 ORIG 全等——daily -0x38、
    //   bitset::reference -0x34、newList -0x44、trace -0x2c（P221 全表达式临时链
    //   `cMyTrace(...)(...)` 翻正 trace 槽方向，具名对象被 4.4 放最深 -0x44）。
    //   残差仅 ORIG 正常 epilogue 在前 / unwind 块在函数最末，4.4 相反（difflib
    //   rotate 放大 ~30 行），gap=1，语义等价。
    // [b28d-solo-CMissionList_Charac] 判词 N104（132/133）：调用点差集归零、槽位集
    //   全等，唯 EH 正常尾块/unwind 尾块相对序互换 ⇒ 封顶。
    std::bitset<32> daily = G_CDataManager()->get_daily_mission_kind();
    for (int i = 0; i < 0x20; ++i)
    {
        if (daily[i])
        {
            m_missionList[i].clear();
            m_missionList[i].m_kind = (short)i;
        }
    }

    std::vector<std::pair<unsigned int, unsigned int> > newList;
    G_CDataManager()->get_New_DailyMission(newList);
    if (newList.empty())
    {
        // ORIG 0x85e5607：cMyTrace 5/0x669 记 "[@missionSystem] Daily Mission List is Empty!!"
        // [b28d-58] 试 P221 全表达式临时链：具名 trace 被 4.4 放最深槽 -0x44，
        // ORIG 为 -0x2c（块内对象槽方向相反）。
        cMyTrace(__PRETTY_FUNCTION__, 0x669, 5)(
            "[@missionSystem] Daily Mission List is Empty!!");
    }
    for (unsigned int i = 0; i < newList.size(); ++i)
    {
        unsigned int kind = newList[i].first;
        if (daily[kind])
        {
            MakeMissionList_JustKind(*m_user, kind);
        }
    }
    alter();
}

void CMissionList_Charac::GM_SetMissionValue(int kind, int value)
{
    // b28c-L5：同 Remove_Mission —— ORIG 0x085e56ee 三岛形（+0x2f nop;jmp /
    // +0x32 nop fall），探针 l5_probe_a fD 复现（P223）。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    m_missionList[kind].m_count = (short)value;
    alter();
L_overrange:;
L_nonrange:;
}

int CMissionList_Charac::getNewMission(CUser& user, int kind)
{
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission(kind);
    if (mission)
    {
        // ORIG kind 落槽 -0xc（P0 具名局部；槽序 mission=-0x10 先、kind=-0xc 后）。
        int missionKind = mission->GetMissionKind();
        ClearMissionKind(user, (short)missionKind);
    }
    // ORIG 尾无 mov $0x0（call ClearMissionKind 后直接 leave;ret）：源码无 return，
    // 返回值自然沿 ClearMissionKind 的 eax 掉出（4.1 实证）。
}

// ============================================================================
// 发送
// ============================================================================

    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
int CMissionList_Charac::Send_MissionList(CUser& user)
{
    PacketGuard packet;
    packet.put_header(0, 0x140);
    int index = packet.get_index();
    packet.put_byte(0);
    int count = 0;
    for (int kind = 1; kind < 0x24; ++kind)
    {
        if (m_missionList[kind].m_index != 0)
        {
            packet.put_short((int)m_missionList[kind].m_index);
            if (kind == 0x1b)
            {
                short combo = 0;
                for (int i = 0; i < 5; ++i)
                {
                    if (((int)m_missionList[kind].m_count >> i) & 1)
                    {
                        ++combo;
                    }
                }
                packet.put_short((int)combo);
            }
            else
            {
                packet.put_short((int)m_missionList[kind].m_count);
            }
            CMission* mission = (CMission*)G_CDataManager()->find_mission(
                (int)m_missionList[kind].m_index);
            if (!mission)
            {
                m_missionList[kind].clear();
            }
            else
            {
                if ((kind == 0x1b) || (kind == 0x1f) || (kind == 0x20))
                {
                    packet.put_short((int)m_missionList[kind].m_streak);
                }
                else
                {
                    packet.put_short(mission->getClearCondition());
                }
                ++count;
            }
        }
    }
    packet.put_byte(index, count);
    unsigned char buf[0x20];
    BitsetToStr(m_clearedFlags, buf);
    for (int i = 0; i < 0x20; ++i)
    {
        packet.put_byte((int)buf[i]);
    }
    packet.finalize(true);
    user.Send(packet);
    return 1;
}

void CMissionList_Charac::Send_Cleared_Mission_List(
    CUser& user, const std::vector<int>& clearList)
{
    if (clearList.empty())
    {
        return;
    }
    PacketGuard packet;
    packet.put_header(0, 0x141);
    packet.put_byte((int)clearList.size());
    for (unsigned int i = 0; i < clearList.size(); ++i)
    {
        packet.put_short(clearList[i]);
    }
    packet.finalize(true);
    user.Send(packet);
}

void CMissionList_Charac::Send_Fail_Mission(CUser& user, short index)
{
    PacketGuard packet;
    packet.put_header(0, 0x157);
    packet.put_short((int)index);
    packet.finalize(true);
    user.Send(packet);
}

void CMissionList_Charac::setIssue(bool isNewbie, bool isDaily)
{
    if (isDaily)
    {
        m_field148 = (short)(m_field148 * 2);
        m_field148 = (short)((unsigned short)m_field148 +
                             (unsigned short)(isNewbie ? 1 : 0));
    }
    else
    {
        m_field14a = (short)(m_field14a * 2);
        m_field14a = (short)((unsigned short)m_field14a +
                             (unsigned short)(isNewbie ? 1 : 0));
    }
    m_field14c = (short)(m_field14c * 2);
    m_field14c = (short)((unsigned short)m_field14c +
                         (unsigned short)(isNewbie ? 1 : 0));
}

// ============================================================================
// 查询
// ============================================================================

bool CMissionList_Charac::isClearMission(int index) const
{
    // b28c-L5：ORIG 0x085e50fa 尾形 `mov $0x1,%eax / jmp / mov $0,%eax / leave`（20 条）。
    // 4.4 对「if(cond) return 1; return 0;」恒额外物化一段 bool 归一岛（+5 条：
    // `mov $0,%eax; test %al; je; mov $1; jmp; mov $0`），探针 /tmp/l5_probe_isclear.cpp
    // fA(异形 25)/fB(三目 20 全等)/fD(嵌套 if 20 但条件树异形) 定案：三目返回形
    // 与 ORIG 逐条全等（P47 三目 vs if-return 块排布镜像的本例应用）。
    return ((index >= 0) && (index < 0x100) && m_clearedFlags[(unsigned int)index])
               ? 1
               : 0;
}

short CMissionList_Charac::getIndex_byKind(int kind) const
{
    if ((kind < 1) || (kind > 0x23))
    {
        return 0;
    }
    return m_missionList[kind].m_index;
}

unsigned short CMissionList_Charac::getIssue_mode(bool isDaily) const
{
    if (isDaily)
    {
        return m_field148;
    }
    return m_field14a;
}

unsigned short CMissionList_Charac::getIssue_all() const
{
    return m_field14c;
}

int CMissionList_Charac::getWinningRate() const
{
    unsigned short rate = (unsigned short)m_field14c;
    int count = 0;
    for (int i = 0; i < 10; ++i)
    {
        if (rate & 1)
        {
            ++count;
        }
        rate = (unsigned short)((int)(unsigned int)rate >> 1);
    }
    return count * 10;
}

bool CMissionList_Charac::empty_progress() const
{
    for (int kind = 1; kind <= 0x23; ++kind)
    {
        if (m_missionList[kind].m_index != 0)
        {
            return 0;
        }
    }
    return 1;
}

bool CMissionList_Charac::isUserNewbie() const
{
    static std::bitset<256> zeroBit(0);   // ORIG bitset<256>::bitset(unsigned long)（C1Em），非默认构造
    if (empty_progress() && (zeroBit == m_clearedFlags))
        return true;
    else
        return false;
}

bool CMissionList_Charac::isClearStealingSkillMission() const
{
    return m_field14e;
}
