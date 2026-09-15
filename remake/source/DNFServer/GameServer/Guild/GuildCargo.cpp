// df_guild_r — GuildCargo (split from source/guild per ORIG layout)
// ORIG 的 GuildCargo TU 中 STGuildCargoLog 无用户析构（InsertHistory 局部对象
// 不调用 D1Ev、无 EH 清理块；D1Ev/D2Ev 弱符号由其它 TU 的头文件内联定义提供），
// 与 DNFFunctionLib.h 中声明析构的 TU 构成 ODR 差异——按二进制证据原样复现。
#define DNF_GUILD_ODR_TRIVIAL_CARGOLOG_DTOR
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "GuildCargo.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

// 按 ORIG 布局的字段覆盖视图（避免 char* 算术产生 add 指令形态）
struct GuildCargoLayout
{
    char pad[0x18d8];
    int m_capacity;      // +0x18d8
    int m_field18dc;     // +0x18dc
    int m_guildKey;      // +0x18e0
    bool m_load;          // +0x18e4
    char rest[0x1910 - 0x18e8];
};

#pragma pack(push,1)
// 按 ORIG 布局的 DnfItemInfo 字段覆盖视图（+1 为 item_id，0x35 字节/项）
struct GuildCargoItemLayout
{
    unsigned char m_seal;          // +0x0
    unsigned int m_itemId;         // +0x1
    unsigned char m_attr;          // +0x5
    unsigned int m_addInfo;        // +0x6
    unsigned short m_endurance;    // +0xa
    unsigned int m_extendInfo;     // +0xc
    unsigned char m_abilityType;   // +0x10
    unsigned short m_abilityValue; // +0x11
    char m_rest[0x35 - 0x13];      // +0x13
};
#pragma pack(pop)

STGuildCargoLog::STGuildCargoLog()
{
    memset(this, 0, sizeof(STGuildCargoLog));
}

STGuildCargoDBInfo::STGuildCargoDBInfo()
{
    m_capacity = 0;
}

bool CGuildCargo::IsLoadComplete()
{
    return ((GuildCargoLayout*)this)->m_load;
}

void CGuildCargo::SetCapacity(unsigned int capacity)
{
    ((GuildCargoLayout*)this)->m_capacity = (int)capacity;
}

int CGuildCargo::GetCapacity()
{
    return ((GuildCargoLayout*)this)->m_capacity;
}

void* CGuildCargo::GetGuildCargoDBInfo()
{
    return this;
}

CGuildCargo::CGuildCargo()
{
    Reset();
}

CGuildCargo::~CGuildCargo()
{
    Reset();
}

void CGuildCargo::Reset()
{
    m_info.m_capacity = 0;
    m_itemCount = 0;
    m_guildKey = 0;
    m_load = 0;
    m_history.clear();
    memset(&m_info, 0, 0x18d8);
}

void CGuildCargo::SetGuildInfo(int guildKey)
{
    ((GuildCargoLayout*)this)->m_guildKey = guildKey;
}

bool CGuildCargo::IsValidSlot(int slot)
{
    if (slot < 0 || ((GuildCargoLayout*)this)->m_capacity <= slot || 0x77 < slot)
    {
        return 0;
    }
    return 1;
}

int CGuildCargo::AddItem(DnfItemInfo& info, int slot, int count)
{
    if (m_info.m_items[slot].m_itemId == 0)
    {
        m_info.m_items[slot] = info;
        m_itemCount += 1;
        DNF_LOG_SCOPE_LINE(0x4b,"./log/GuildCargo",
            "AddItem SUCCESS - GUILD:%d, CHARAC:%d, ITEM:%d, SLOT:%d",
            m_guildKey, count, info.m_itemId, slot);
        return 0xc1;
    }
    DNF_LOG_SCOPE_LINE(0x54,"./log/GuildCargo",
        "AddItem ITEM ALREADY EXIST - GUILD:%d, CHARAC:%d, INSERT ITEM:%d, SLOT:%d, AREADY ITEM:%d",
        m_guildKey, count, info.m_itemId, slot,
        m_info.m_items[slot].m_itemId);
    return 0xc9;
}

int CGuildCargo::CalcItemCount()
{
    m_itemCount = 0;
    for (int i = 0; i < 0x78; i++)
    {
        if (m_info.m_items[i].m_itemId != 0)
        {
            m_itemCount = m_itemCount + 1;
        }
    }
    return m_itemCount;
}

int CGuildCargo::InsertItem(DnfItemInfo& info, int& slot, int count, unsigned char a, int b)
{
    if (!IsValidSlot(slot))
    {
        return 0xc4;
    }
    if (a == 1)
    {
        int existingSlot = GetSpecificItemSlot(info.m_itemId);
        if (existingSlot != -1)
        {
            int oldCount = m_info.m_items[existingSlot].m_addInfo;
            int addCount = info.m_addInfo;
            if (oldCount + addCount <= count)
            {
                m_info.m_items[existingSlot].m_addInfo =
                    m_info.m_items[existingSlot].m_addInfo + info.m_addInfo;
                DNF_LOG_SCOPE_LINE(0x89,"./log/GuildCargo",
                    "InsertItem STACKABLE ADD SUCCESS - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, ADD:%d, CURR:%d",
                    m_guildKey, b, info.m_itemId,
                    oldCount, addCount, m_info.m_items[existingSlot].m_addInfo);
                slot = existingSlot;
                return 0xc1;
            }
            DNF_LOG_SCOPE_LINE(0x96,"./log/GuildCargo",
                "InsertItem STACKABLE ADD OVER STACK LIMIT - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, ADD:%d, LMT:%d, CURR:%d",
                m_guildKey, b, info.m_itemId,
                oldCount, addCount, count, m_info.m_items[existingSlot].m_addInfo);
            slot = existingSlot;
            return 200;
        }
    }
    int ret = AddItem(info, slot, b);
    return ret;
}

int CGuildCargo::CheckInsertItem(int itemId, int count, int slot, unsigned char stackable,
                                 int maxStack)
{
    if (!IsValidSlot(slot))
    {
        return 0xc4;
    }
    if (stackable == 1)
    {
        int existingSlot = GetSpecificItemSlot(itemId);
        if (existingSlot != -1)
        {
            if ((int)m_info.m_items[existingSlot].m_addInfo + count > maxStack)
            {
                return 200;
            }
            return 0xc1;
        }
    }
    if (m_info.m_items[slot].m_itemId != 0)
    {
        return 0xc9;
    }
    return 0xc1;
}

// [DNF-NONIDENTICAL] DNF-GLD-NEAR-0001 | guild | 与ORIG差异=NEAR | CGuildCargo::DeleteItem | 详见 function_reports/guild/_ZN11CGuildCargo10DeleteItemER11DnfItemInfoiihii.md
// [2026-09-15 W3-GuildRest 已翻正 ✓] Main 解禁 Game/DnfItemInfo.h 后，按其三条约束落地：
//   `unsigned int m_addInfo` → `int m_addInfo`（同宽同偏移，static_assert sizeof/offsetof 全过）
//   ⇒ 本件 NEAR→**AE identical**（164/164、ndiff 0）✓；guild 全量重编 identical 1867→**1875**、
//   非identical 63→**55**（本泳道 2 件直翻 + DNFPacketTranslater 侧连带/并发），只升不降 ✓。
//   ⇒ 机理 C 的跨服务头路径（§5.19-C 排除项）自此解禁并由本件闭环 ✓。
// [机理 C · 待环境（历史记录）] 本件差异 = `m_addInfo` signedness 决定 LHS/RHS 载入序；
// 沙箱实证：改 Game/DnfItemInfo.h 的 m_addInfo → int 可令本件 NEAR→AE identical（164/164 ndiff 0）。
// 阻断原因：跨服务头 ⇒ game InterDispatcher.cpp.o md5 变化（2ec9edcf→deeb80d0）且 game 不可重链 ⇒ 无法验证 ⇒ 按闸门维持回退。
// 若将来 game 可重链 ⇒ 本件 + 同族件应优先重估。
// [GuildA 2026-09-15 实证但受阻塞，未落盘] P2 判据（赋值目标 signedness 决定 LHS/RHS 载入序）：
//   本件 ELF 口径残差仅 2 行、纯载入序交换（ORIG `mov 0xc(%ebp),%eax`(info/LHS) 先于
//   `mov 0x1c(%ebp),%edx`(b/RHS)；我方相反），对应源码 `info.m_addInfo = (int)b;`。
//   沙箱实测：把 `Game/DnfItemInfo.h` 的 `unsigned int m_addInfo` 改成 `int m_addInfo`
//   （同宽同偏移同大小）⇒ 本件 **NEAR → AE identical**（mdiff(.o)=0 equal=164；
//   mdiff_ta(ELF)= AE 164/164）。
//   **未落盘原因**：该头影响 **game 机器码**（第 1 级 .o md5 A/B：game
//   `InterDispatcher.cpp.o` `2ec9edcf02aec2021e93a96ec572dc41` → `deeb80d0e4c4e2560cdecefe7b00874c`；
//   `DnfItemInfo.cpp.o` 中性），而 game ELF 无法重链 ⇒ 不可验证；游戏侧不在任务范围内
//   ⇒ 按 Main 裁决维持回退（`unsigned int`），待未来有 game 可验证环境时可直接复用本证据。
//   其它已试形态（均 2–3，无改善）：去 `(int)` cast、`(int)(int)b`、
//   `*(int*)((char*)&info + 6) = b;`。
//   [NEAR-Guild 2026-09-15 追加 2 形态（本 TU 内、零跨服务面），均劣化 ⇒ 已回退]
//   V1 `(int&)info.m_addInfo = b;` ⇒ 目标降级为无 disp 的间接写：`add $0x6,%eax` + `(%eax)`
//      （165 条、ndiff 9）✗
//   V2 `*(int*)(&info.m_addInfo) = b;` ⇒ 同上形态（165 条、ndiff 9）✗
//   ⇒ 目标类型变 signed 才能复现 ORIG 的 LHS-first，而**任何指针/引用视图都会丢 disp 寻址**
//     ⇒ 唯一有效路径 = 改 `Game/DnfItemInfo.h` 字段为 int（已由 GuildA 沙箱证明 NEAR→AE，
//     受 game 不可重链阻塞，见上）⇒ 本 TU 内无靶点 ✓
int CGuildCargo::DeleteItem(DnfItemInfo& info, int slot, int count, unsigned char a, int b, int c)
{
    if (!IsValidSlot(slot))
    {
        return 0xc4;
    }
    if (m_info.m_items[slot].m_itemId == 0 ||
        m_info.m_items[slot].m_itemId != count)
    {
        return 0xca;
    }
    info = m_info.m_items[slot];
    if (a == 1)
    {
        info.m_addInfo = (int)b;
        int oldCount = m_info.m_items[slot].m_addInfo;
        int subCount = info.m_addInfo;
        if (oldCount < subCount)
        {
            return 199;
        }
        m_info.m_items[slot].m_addInfo =
            m_info.m_items[slot].m_addInfo - info.m_addInfo;
        if (m_info.m_items[slot].m_addInfo == 0)
        {
            m_info.m_items[slot].reset();
        }
        DNF_LOG_SCOPE_LINE(0xee,"./log/GuildCargo",
            "DeleteItem STACKABLE DELETE SUCCESS(Stackable) - GUILD:%d, CHARAC:%d, ITEM:%d, OLD:%d, SUB:%d, CURR:%d",
            m_guildKey, c, info.m_itemId,
            oldCount, subCount, m_info.m_items[slot].m_addInfo);
    }
    else
    {
        m_info.m_items[slot].reset();
        DNF_LOG_SCOPE_LINE(0xfa,"./log/GuildCargo",
            "DeleteItem STACKABLE DELETE SUCCESS(Equip) - GUILD:%d, CHARAC:%d, ITEM:%d",
            m_guildKey, c, info.m_itemId);
    }
    return 0xc1;
}

int CGuildCargo::MoveItem(DnfItemInfo& from, DnfItemInfo& to, int fromSlot, int fromItemId,
                          int toSlot, int toItemId, int charNo)
{
    if (!IsValidSlot(fromSlot) || !IsValidSlot(toSlot) || fromSlot == toSlot)
    {
        return 0xc4;
    }
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x115,"./log/GuildCargo",
        "Before MoveItem - GUILD:%d, CHARAC:%d, SLOT1:(%d,%d), SLOT2:(%d,%d)",
        m_guildKey, charNo, fromSlot, m_info.m_items[fromSlot].m_itemId,
        toSlot, m_info.m_items[toSlot].m_itemId);
    from = m_info.m_items[fromSlot];
    to = m_info.m_items[toSlot];
    if (from.m_itemId != fromItemId || to.m_itemId != toItemId)
    {
        return 0xca;
    }
    {
        DnfItemInfo tmp;
        tmp = m_info.m_items[fromSlot];
        m_info.m_items[fromSlot] = m_info.m_items[toSlot];
        m_info.m_items[toSlot] = tmp;
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x131,"./log/GuildCargo",
            "After MoveItem - GUILD:%d, CHARAC:%d, SLOT1:(%d,%d), SLOT2:(%d,%d)",
            m_guildKey, charNo, fromSlot, m_info.m_items[fromSlot].m_itemId,
            toSlot, m_info.m_items[toSlot].m_itemId);
        return 0xc1;
    }
    return 0xca;
}

int CGuildCargo::GetSpecificItemSlot(int itemId)
{
    for (int i = 0; (int)m_info.m_capacity > i; i++)
    {
        if (m_info.m_items[i].m_itemId == itemId)
        {
            return i;
        }
    }
    return -1;
}

void CGuildCargo::SetGuildCargoDBInfo(STGuildCargoDBInfo& info)
{
    memcpy(&m_info, &info, sizeof(m_info));
    ((GuildCargoLayout*)this)->m_load = 1;
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0005 | guild | 与ORIG差异=DIFF | CGuildCargo::SetGuildCargoHistory | 详见 function_reports/guild/_ZN11CGuildCargo20SetGuildCargoHistoryEjP15STGuildCargoLog.md
void CGuildCargo::SetGuildCargoHistory(unsigned int idx, STGuildCargoLog* log)
{
    // [2026-09-15 修复] ORIG 0x8063d??：`mov 0xc(%ebp),%eax; cmp $0x32,%eax; jbe L;
    //   mov $0x32,%eax; L: mov %eax,-0x10(%ebp)` ⇒ 原码是**三目求值一次再落槽**，
    //   拆成 `int c = idx; if (idx > 0x32) c = 0x32;` 会多出参数槽直比 + 双写槽（ndiff 6）。
    int c = (idx > 0x32) ? 0x32 : (int)idx;
    for (int i = 0; i < c; i++)
    {
        m_history.push_front(log[i]);
    }
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0002 | guild | 与ORIG差异=DIFF | CGuildCargo::SendGuildCargo | 详见 function_reports/guild/_ZN11CGuildCargo14SendGuildCargoEP5CUser.md
void CGuildCargo::SendGuildCargo(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    Packet_Notice_Guild_Cargo pkt;
    pkt.m_a = user->GetIdByChannel();
    pkt.m_b = user->GetUniqCharNo();
    // [2026-09-15 GuildA] 根因不是 sizeof 写法，而是 memcpy **源实参的表达式形态**：
    //   ORIG 先载 this 到 eax 再压 size ⇒ 源码写的是成员 `&m_info`（不是 `this`）。
    //   改 `&m_info` 后 ndiff 2→0（sizeof 用 sizeof(m_info) 或 sizeof(STGuildCargoDBInfo) 均可）。
    memcpy(&pkt.m_cargo, &m_info, sizeof(m_info));
    user->SendTcpGameserver(&pkt);
}

void CGuildCargo::PrintCargo(ENUM_GUILD_CARGO_BEHAVIOR behavior)
{
    DNF_LOG_SCOPE_LINE(0x18d, "./log/GuildCargo", "CARGO - g:%d,capa:%d,behavior:%d",
         m_guildKey, m_info.m_capacity, (int)behavior);
    for (int i = 0; (int)m_info.m_capacity > i; i++)
    {
        if (m_info.m_items[i].m_itemId != 0)
        {
            DNF_LOG_SCOPE_LINE(0x195, "./log/GuildCargo", "SLOT - %d,%s",
                i, PrintDnfItemInfo(m_info.m_items[i]));
        }
    }
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0003 | guild | 与ORIG差异=DIFF | CGuildCargo::PrintDnfItemInfo | 详见 function_reports/guild/_ZN11CGuildCargo16PrintDnfItemInfoER11DnfItemInfo.md
const char* CGuildCargo::PrintDnfItemInfo(DnfItemInfo& info)
{
    struct AttrBits { unsigned char pad[5]; unsigned char lo:5; unsigned char hi:3; };
    static char szBuffer[0x400];
    memset(szBuffer, 0, sizeof(szBuffer));
    sprintf(szBuffer,
            "id:%d,s:%d,sc:%d,up:%d,add:%d,en:%d,ex:%d,at:%d,av:%d,sp:%d",
            info.m_itemId,
            (int)info.m_seal,
            (int)(((AttrBits*)&info)->hi),
            (int)(((AttrBits*)&info)->lo),
            info.m_addInfo,
            info.m_endurance,
            info.m_extendInfo,
            (int)info.m_abilityType,
            info.m_abilityValue,
            (unsigned int)info.m_up.GetUpgradeSeparate());
    return szBuffer;
}

// [DNF-IDENTICAL（2026-09-15 复核：已 identical ✓）] DNF-GLD-DIFF-0004 | guild | 与ORIG差异=DIFF | CGuildCargo::SendGuildCargoToDBMW | 详见 function_reports/guild/_ZN11CGuildCargo20SendGuildCargoToDBMWEP14CServerHandleri.md
void CGuildCargo::SendGuildCargoToDBMW(CServerHandler* handler, int slot)
{
    CTcpDBServer* db = handler->GetTcpDBServer();
    struct GuildCargoToDBMWPkt {
        char hdr[0xa];
        unsigned int guildKey;
        unsigned int slot;
        char cargo[0x18d8];
    } __attribute__((packed));
    void* raw = db->makePacketHeader(0x710, 0x18ea);
    GuildCargoToDBMWPkt* p = (GuildCargoToDBMWPkt*)raw;
    p->guildKey = m_guildKey;
    p->slot = slot;
    // [P319] 整型加法形（`add $0x12` 对齐 ORIG）与 char* 形 ndiff 同为 6，但后者
    //   行级 equal 更高（34 vs 32）⇒ 取后者（guild11 交叉验证）
    // [2026-09-15 GuildA] 残余 6 条并非 add/lea，而是 memcpy 源实参写成 `this`：
    //   改成成员 `&m_info` 后 ORIG 的「先载 this 再压 size」顺序重现 ⇒ ndiff 6→0。
    memcpy((char*)p + 0x12, &m_info, 0x18d8);
    db->SendToServer((char*)p);
}

bool CGuildCargo::IsEmpty()
{
    for (int i = 0; (int)m_info.m_capacity > i; i++)
    {
        if (m_info.m_items[i].m_itemId != 0)
        {
            return 0;
        }
    }
    return 1;
}

// [DNF-NONIDENTICAL] DNF-GLD-NEAR-0002 | guild | 与ORIG差异=NEAR | CGuildCargo::InsertHistory | 详见 function_reports/guild/_ZN11CGuildCargo13InsertHistoryE25ENUM_GUILD_CARGO_BEHAVIORiPKciiPK12RandomOption.md
void CGuildCargo::InsertHistory(ENUM_GUILD_CARGO_BEHAVIOR behavior, int slot, const char* name, int count,
                                int param, const RandomOption* option)
{
    STGuildCargoLog log;
    strncpy(log.name, name, 0x14);
    log.count = count;
    log.behavior = behavior;
    log.param = param;
    // 权威定义（Game/Inven_Item.h）的 RandomOption 无 dword 视图（成员改由
    // m_fields[]/m_seed9/m_fielda/m_seedd 表达），故按 ORIG 0x080a0182 的读取口径
    // 逐偏移取值：+0/+4/+8 = dword、+0xc = word（与 STGuildCargoLog
    // opt0@+0x22/opt1@+0x26/opt2@+0x2a/opt3@+0x2e 一一对应）。用结构视图而非
    // 指针算术，方产出 ORIG 的单基址连续偏移装载形（byte* 的 `o+4` 会生物化 add）。
    struct OptionDwordView
    {
        unsigned int opt0;        // +0x00
        unsigned int opt1;        // +0x04
        unsigned int opt2;        // +0x08
        unsigned short opt3;      // +0x0c
    } __attribute__((packed));
    // [2026-09-15 GuildA] 本处残余 ndiff=8 = 寄存器选择族（死胡同，勿再投入）。
    //   取证（ORIG df_guild_r）：`RandomOption::reset()`（W 0x08089620）逐条访问
    //   +0/+3/+6（RandomOptionField，3 字节步长）/ +9（Seed）/ +0xa（Field）/ +0xd（Seed），
    //   与 Game/Inven_Item.h 定义逐字段一致 ⇒ 布局正确、sizeof==0xe，**不是结构体错误**；
    //   所以此处 ORIG 的 `+0/+4/+8` dword + `+0xc` word 读法在源码里就是一次 cast/union 视图，
    //   不可用「改共享头补字段」来消除。
    //   ORIG 单次载入 option 到 %eax 作基址、结果走 %edx；我方同结构但基址落 %ebx。
    //   已试形态（均 ≥8，无一改善）：present 指针 register 形（8，保留）/ C 风格 cast（8）/
    //   `unsigned int*` + o[0..2] + *(unsigned short*)(o+3)（12）/ const 引用形 o.optN（24）/
    //   去 register（24）。
    //   [W3-GuildRest 2026-09-15 复核] 唯一可行路径（给 STGuildCargoLog 增 14 字节视图成员）需改
//   **ServerCommon/DNFFunctionLib.h**（跨服务共享头，非本次授权的 Game/DnfItemInfo.h）
//   ⇒ 维持不投入 ✓（本 TU 内确无靶点）。
//   [NEAR-Guild 2026-09-15 追加 3 形态，均劣化 ⇒ 已回退，全部读数取自链接后 ELF]
    //   V1 `*(OptionDwordView*)(void*)&log.opt0 = *(const OptionDwordView*)option;`（结构体整体赋值）
    //      ⇒ GCC 走 memcpy 路径：先生成 -0x44 临时对象（82 条、ndiff 10）✗
    //   V2 `memcpy(&log.opt0, option, 0xe);` ⇒ 直出 `call memcpy@plt`（GCC -O0 不内联常量拷贝，78 条、ndiff 10）✗
    //   V3 `register const OptionDwordView* o __asm__("eax")`（邻件 GuildMemLogout 的成功手法）
    //      ⇒ **基址正确落 %eax、值走 %edx**，但每条访问多一条 `mov %eax,%edx`（83 条、ndiff 7）✗
    //      ⇒ 与 ORIG「形参单次载入 %eax + 4 次同基址 disp 访问」仅差该 3 条 ⇒ ORIG 形态
    //        只能由「LHS 是**真实结构体成员**的整体赋值」产生（需给 STGuildCargoLog 增一个 14 字节
    //        视图成员，位于 GuildCargo.h ⇒ **出本泳道白名单**）⇒ 本 TU 内无靶点 ✓
    register const OptionDwordView* o = reinterpret_cast<const OptionDwordView*>(option);
    log.opt0 = o->opt0;
    log.opt1 = o->opt1;
    log.opt2 = o->opt2;
    log.opt3 = o->opt3;
    time_t t;
    time(&t);
    log.time = t;
    m_history.push_back(log);
    DNF_LOG_SCOPE_LINE(0x202, "./log/GuildCargo",
            "InsertLog - GUILD:%d, CHARAC:%d, ITEM:%d, BEHAVIOR:%d, ETC:%d",
            m_guildKey, slot, count, (int)(char)behavior, param);
    if (0x32 < m_history.size())
    {
        m_history.pop_front();
    }
}

void CGuildCargo::GetHistory(STGuildCargoLog* out)
{
    std::deque<STGuildCargoLog>::iterator it = m_history.begin();
    int i = 0;
    while (it != m_history.end())
    {
        memcpy(out + i, &(*it), 0x30);
        ++it;
        ++i;
        if (i > 0x32)
        {
            break;
        }
    }
}

void CGuildCargo::SendHistoryToDBMW(CServerHandler* handler, ENUM_GUILD_CARGO_BEHAVIOR behavior, int slot,
                                    const char* name, int count, int param, DnfItemInfo& info)
{
    Packet_DB_Insert_Guild_Cargo_History pkt;
    pkt.m_b = m_guildKey;
    pkt.m_c = slot;
    pkt.m_d = (unsigned char)behavior;
    pkt.m_e = count;
    pkt.m_f = param;
    pkt.m_a = handler->GetServerGroupNo();
    strncpy(pkt.m_pad, name, 0x14);
    memcpy(&pkt.m_item, &info, 0x35);
    handler->SendToDB(&pkt);
}

Packet_Notice_Guild_Cargo::Packet_Notice_Guild_Cargo()
    : PacketHeader(0x712, 0x18ee), m_a(0xffffffff), m_b(0)
{
    memset(&m_cargo, 0, sizeof(m_cargo));
}

Packet_DB_Insert_Guild_Cargo_History::Packet_DB_Insert_Guild_Cargo_History()
    : PacketHeader(0x711, 0x66), m_a(0), m_b(0), m_c(0), m_d(0xff), m_e(0), m_f(0)
{
    memset(m_pad, 0, 0x15);
    memset(&m_item, 0, 0x35);
}
