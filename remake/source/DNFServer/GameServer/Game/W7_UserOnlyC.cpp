// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W7_UserOnlyC/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// W7_UserOnlyC.cpp —— W7C_UserOnly 链接收敛独占 TU（2026-08-27）
//
// 承载此前无 owner 的 ORIG 真实符号定义；函数体依 docs/class_func_reports
// 对应报告或 objdump 逐指令忠实转写，偏移/常量来自汇编。符号清单：
//   CPacketTimeChecker::{reset,check} + validCount_E/validSecond_E
//   CPacketVerifier::CPacketVerifier/Init/UpdatePacket/GetSequence + 4 statics
//   _NS_PI_2ND_GetHackType（b28d-solo：本 TU static 副本已删，改用 GameSecurity.cpp
//     权威全局 W 0x08277359；符号名对齐 _Z22…）
//   PIReceiverManager::CheckHashReceived                    T 0x0808bf7e
//   _NS_PI_2ND_RecvHash_CS                                  T 0x08274f73
//   CUser::Parsing(int)                                     T 0x08647890
//   CUser::SetSkillLevel(int,int)                           T 0x0867b51c
//   CUser::getReturnUserLevelKey(int,int)                   T 0x0869230a
//   CUser::GiveItemsReturnUser()                            T 0x08692000
//   CUser::GiveEquipItemsReturnUser()                       T 0x08692140
//   CUser::setCharacLinkMessageFlag(bool)                   W 0x084ece20
//   CUser::SendLimitItemUsageNotiInfo(int,int,int)          T 0x08688f02
//   CUser::SendCeraShopGiftStatisticLog(uint,uint,ulong,uint) T 0x0868a87a
//   LimitItemUsage::{C1,D1,isAnotherDay,isUsable,useIt}     W 0x8695ef0..
//   UserMercenaryInfoMgr::UpdateCharacName(uint,string const&) T 0x081b462e
//   CUserEventUtil::updateCharacterDayEvent(CUser*,int)     T 0x08688230
//   CDungeon::VerifyMaze() const                            T 0x0834c9f0
//   CDungeon::VerifyMaze(MazeScript const&) const           T 0x0834cb7c
//   CStackableItem::GetMonsterCardId()                      W 0x0849f530

#include "CUser.h"
// ============================================================================
// b28d-52 判词表（本 TU 剩余 DIFF）：
//   已修：CPacketVerifier::GetSequence 0x08696e48 —— P159/SIGFIX 返回类型 int
//     （ORIG 尾双段 movzwl；探针 /tmp/b28d_l52/p_ushort.cpp f2 实证）。CUser.h 冻结，
//     Main 已代落 ⇒ 本件转 AE。
//   已修（头部）：CUser::SetSkillLevel 0x0867b51c —— P248② 裸偏移改具名字段
//     `!m_clientSpec.m_gmUserFlag` + P214① 短路汇聚（头部 20 条与 ORIG 逐条对齐）；
//     残差 N82（帧 0x4c vs 0x44 + 后续槽组/push edi/esi）。
//   CPacketVerifier::UpdatePacket 0x08696e58 —— 残差仅 call/跳转目标重定位（N7 寄存器
//     轮转 + P264 口径）
//   PIReceiverManager::CheckHashReceived 0x0808bf7e —— P0/P183（我方 so 指针缓存槽 -0x14
//     + `add $0x4` 地址形；ORIG 表达式直读）
//   CUser::SetSkillLevel 0x0867b51c —— P248② 字段 bool 化候选（ORIG 0x0867b52d xor $1）
//     （见上「已修（头部）」）
//   CUser::GiveItemsReturnUser 0x08692000 —— N82（帧 0x8c vs 0x98）
//   CUser::GiveEquipItemsReturnUser 0x08692140 —— N82（帧 0x9c vs 0xb4）
//   CUser::SendCeraShopGiftStatisticLog 0x0868a87a —— N82 + P264（Acquire 立即数重定位）
//   CUser::Parsing 0x08647890 —— N82（帧 0x63c vs 0x61c）
//   CDungeon::VerifyMaze() 0x0834c9f0 —— N82（帧 0xa0 vs 0x90）
//   CDungeon::VerifyMaze(MazeScript) 0x0834c990 —— N82（帧 0x94 vs 0x84）
//   _NS_PI_2ND_RecvHash_CS 0x08274f73 —— N82（帧 0x64 vs 0x74）
// ============================================================================
#include "CDungeon.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "CSkill.h"
#include "Cipher.h"
#include "Encryption.h"
#include "PacketSecurity.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"
#include "CHackAnalyzer.h"
#include "CEnvironment.h"
#include "CMailBoxHelper.h"
#include "SkillSlot.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <utility>

extern CDataManager* G_CDataManager();
extern char* NumberToString(unsigned int value, int radix);
extern char* GetPacketName(ENUM_PACKETCLASS packetClass, unsigned short packetId);
extern bool CheckDailyScheduleTime(int scheduleTime, long lastPlayTime,
                                   long curTime);
unsigned char MakeHash(unsigned char* data, unsigned int len,
                       unsigned char seed);          // 定义 PIHash.cpp
#include "W7_UserOnlyC/_NS_PI_2ND_GetPacketIndex.inc"
// ORIG W 0x08277359（权威定义 GameSecurity.cpp，去 static 全量重建 0x17b6）；
// 本 TU 原先的 static 副本符号名错（_ZL…）且实现为查表版 ⇒ 删除改用全局符号。
#include "W7_UserOnlyC/_NS_PI_2ND_GetHackType.inc"
                           unsigned short c);

// _header（CUser.h 冻结）m_checksum 为 unsigned int；ORIG UpdatePacket 赋值段
// `mov 0x7(%eax),%edx` 要求与 int 字段 m_lastChecksum 同型（P211）。
// pack(1) 具名视图提供 int 视角，仅本 TU 使用，无成员函数 ⇒ 零符号发射。
namespace {
#pragma pack(push, 1)
struct HeaderChecksumIntView
{
    char m_pad[7];
    int m_checksum;   // +0x07
};
#pragma pack(pop)
}  // namespace

// ============================================================================
// ---- CPacketTimeChecker（ORIG W 0x8696d5c reset / 0x8696d74 check）----
// ============================================================================

// [L32 GI 调序] ORIG `_GLOBAL__I__ZN15CPacketVerifier13m_nVaridCountE`（CU=user.cpp）
//   ⇒ 本 TU 首个强符号须为 CPacketVerifier::m_nVaridCount。两组 int 静态定义互换先后，
//   零语义/零布局改动（皆为静态初始化 POD，不产生构造序）。
// ============================================================================
// ---- CPacketVerifier（ORIG 0x8696fe0 C1 / 0x8696de4 Init / 0x8696e58
//      UpdatePacket / 0x8696e48 GetSequence）----
// ============================================================================

int CPacketVerifier::m_nVaridCount = 5;         // D 0x093ace38
int CPacketVerifier::m_nVaridSecond = 1;        // D 0x093ace3c
int CPacketVerifier::m_nAuctionVaridCount = 6;  // D 0x093ace40
int CPacketVerifier::m_nAuctionVaridSecond = 1; // D 0x093ace44

int CPacketTimeChecker::validCount_ = 100;     // D 0x093ace48
int CPacketTimeChecker::validSecond_ = 1;      // D 0x093ace4c
// reset/check 定义迁至 CUser.cpp（unit-at-a-time outgoing 塌缩修复）。

#include "W7_UserOnlyC/CPacketVerifier.inc"
{
    Init();                    // ORIG C1: checker ctor(空体) + Init
}

#include "W7_UserOnlyC/Init.inc"
                                       // C1 对 Init call 不再布 EH cleanup
{
    m_lastClass = 0;
    m_lastId = 0;
    m_lastChecksum = 0;
    m_sameCount = 0;
    m_sameTime = 0;
    m_sequence = 0;
    m_auctionCount = 0;
    m_auctionTime = 0;
    m_checker.reset();
}

#include "W7_UserOnlyC/GetSequence.inc"
{
    // b28d-D52 终裁（推翻旧 N18a 判词）：ORIG 0x08696e4e/0x08696e52 双段
    // `movzwl 0x14(%eax),%eax; movzwl %ax,%eax` = 字段 unsigned short 读 + 返回
    // 类型 int 的零扩展（探针 p_ushort：ushort 返回单段、int 返回双段全等）。
    return (unsigned short)m_sequence;
}

// 返回错误码：0=正常；1=同包洪水；3=timechecker 节流；4=拍卖包洪水。
// b28d-solo-W7_UserOnlyC 判词：P225 while+break 复现 setg 物化 + m_checksum 经
// HeaderChecksumIntView int 视图同型直存（P211）⇒ 120=120，TU 层 AE。
// 本件无 class_func_reports 报告（报告缺失，按 nm 切片 + objdump 兜底）。
#include "W7_UserOnlyC/UpdatePacket.inc"
{
    if (!m_checker.check())
        return 3;

    // 拍卖类包（id 0xb9..0xc2）独立计数窗口。
    // P0：ORIG 无 id 槽，movzwl 后直接 sub/cmp。
    if ((unsigned int)(hdr.m_id - 0xb9) <= 8)
    {
        ++m_auctionCount;
        // P225：ORIG 0x8696ead `setg %al; test %al,%al; je` 为 4.1 的 bool 物化条件；
        // 4.4 对裸 if 恒折 `jle` ⇒ 用 while+break 单趟形复现（b28 BL5 实证）。
        while (m_auctionCount > m_nAuctionVaridCount)
        {
            long now = time(0);
            if (now - m_auctionTime < m_nAuctionVaridSecond)
                return 4;
            m_auctionCount = 0;
            m_auctionTime = now;
            break;
        }
    }

    // ORIG 比较按 int 直读 class/id/checksum。
    if (static_cast<int>(hdr.m_class) == m_lastClass &&
        static_cast<int>(hdr.m_id) == m_lastId &&
        hdr.m_checksum == m_lastChecksum)
    {
        ++m_sameCount;
        if (m_sameCount > m_nVaridCount)
        {
            long now = time(0);
            if (now - m_sameTime < m_nVaridSecond)
                return 1;
            m_sameCount = 0;
            m_sameTime = now;
        }
    }
    else
    {
        m_lastClass = hdr.m_class;
        m_lastId = hdr.m_id;
        // P211：int 具名视图令 RHS 与 int 字段同型 ⇒ 复现 ORIG `mov 0x7(%eax),%edx`。
        m_lastChecksum =
            reinterpret_cast<const HeaderChecksumIntView*>(&hdr)->m_checksum;
        m_sameCount = 0;
        m_sameTime = time(0);
    }
    return 0;
}

// ============================================================================
// ---- PI_2ND 反作弊 hash 族 ----
// ============================================================================

// b28d-solo-W7_UserOnlyC：本 TU 原 static _NS_PI_2ND_GetHackType{Enum} 副本已删
// （ORIG 权威符号为 GameSecurity.cpp 的全局 W 0x08277359/0x08275af3；副本符号
// `_ZL22…`/`_ZL26…` 与 ORIG `_Z22…`/`_Z26…` 不符且实现不同）。声明见文件头。

// ORIG T 0x0808bf7e PIReceiverManager::CheckHashReceived(uchar*,uint,uint,bool)
// this+0x04 为接收 hash key 字节状态。
// b28d-solo-W7_UserOnlyC 判词：P0/P183 删 pThis 指针缓存（this 表达式直读）+
// P211 cur 同型化（L/S 同型 ⇒ LHS-first）⇒ 72=72，TU 层 AE。
#include "W7_UserOnlyC/CheckHashReceived.inc"
                                          unsigned int len, unsigned int key,
                                          bool bNewKey)
{
    // P0/P183：ORIG 无 this 指针缓存槽（每处 `mov 0x8(%ebp),%eax` 表达式直读）。
    // CUser.h 冻结：+0x04 现为 char m_pad[4]（hash key 字节），以 m_pad[0] 具名访问。
    if (bNewKey)
        m_pad[0] = 0x27;

    const unsigned short packed = static_cast<unsigned short>(key >> 8);
    const unsigned char oldKey = m_pad[0];

    // 新 key 低字节重组：位段 12..13→0..1、4..5→2..3、8..9→4..5、整体左移 6 取低 8 位。
    // P211 同型化：字段 char 与 RHS 同型 ⇒ store 装载序 LHS-first（ORIG 形）。
    const char cur = static_cast<char>(
        ((packed & 0x3000) >> 12) | ((packed & 0x30) >> 2) |
        ((packed & 0x300) >> 4) | (packed << 6));
    m_pad[0] = cur;      // ORIG 无条件写回新 key（在 expect 计算之前）

    // 校验字节：位段 6..7→0..1、14..15→2..3、2..3→4..5、10..11→6..7。
    // P198①：ORIG 先 movb $0,-0xb / movb $0,-0xa 预清 ok/hash 两句式。
    bool ok = false;
    unsigned char hash = 0;
    const unsigned char expect = static_cast<unsigned char>(
        ((packed & 0xc0) >> 6) | ((packed & 0xc000) >> 12) |
        ((packed & 0xc) << 2) | ((packed & 0xc00) >> 4));
    if (len == 0)
    {
        ok = true;
    }
    else
    {
        hash = MakeHash(data, len, oldKey);
        ok = (hash == expect);
    }

    return ok;
}

// ORIG T 0x08274f73：接收 hash 登记 / 异常路径报 hack。
// b28d-solo-W7_UserOnlyC 判词（含 3 处真 bug 修正）：①CheckHashReceived 的 key
// 实参应为 hashDword>>8（原写 hashDword）；②record = !check && !existing（原写
// !check && existing，极性反）；③GetHackType 第一实参为常量 0（原写 hackPackId）。
// 另：make_pair 单次构造复用、index 改 unsigned short（对齐 ORIG `IRhRtE`）⇒
// p167 14/14 CLEAR，132=132，TU 层 AE。
#include "W7_UserOnlyC/_NS_PI_2ND_RecvHash_CS.inc"
{
    // 非 const：ORIG make_pair 实例为 <uchar&, ushort&>（`IRhRtE`，P61；t=unsigned short）。
    unsigned char cls = static_cast<unsigned char>(packet[0]);
    unsigned short index =
        static_cast<unsigned short>(_NS_PI_2ND_GetPacketIndex(packet));

    std::set<std::pair<unsigned char, unsigned short> >* seen =
        static_cast<std::set<std::pair<unsigned char, unsigned short> >*>(
            user->GetCSHashSet());

    // ORIG 只构造一次 make_pair（-0x44），find 与 insert 复用同一 pair（-0x44）。
    std::pair<unsigned char, unsigned short> entry = std::make_pair(cls, index);
    const bool existing = (seen->find(entry) != seen->end());

    const unsigned int hashDword =
        *reinterpret_cast<const unsigned int*>(packet + 7);

    // ORIG 0x0827501f `mov $1,%eax; jmp; mov $0,%eax` 短路物化。
    const bool isSpecial = (cls == 1 && index == 1);

    const unsigned int hackPackId = hashDword >> 8;
    unsigned char* body = reinterpret_cast<unsigned char*>(packet + 13);
    const unsigned int bodyLen = recvSize - 13;

    // b28d-solo 真 bug 修正①：ORIG 0x0827505c 传 key=hashDword>>8（hackPackId），
    // 原写 hashDword（实参取错）。
    const bool check =
        static_cast<PIReceiverManager*>(user->GetPIReceiverManager())
            ->CheckHashReceived(body, bodyLen, hackPackId, isSpecial);
    // b28d-solo 真 bug 修正②：ORIG 0x08275080/0827508b 双 `xor $1; test; je`
    // 同落 false 块 ⇒ record = !check && !existing（原写 `!check && existing` 极性反）。
    // ORIG 0x08275092 `mov $1,%eax; jmp; mov $0,%eax; mov %al,-0xd` ⇒ 赋值物化。
    const bool record = (!check && !existing);
    if (record != 0)
    {
        // b28d-solo 真 bug 修正③：ORIG 0x082750c1 `movl $0x0,(%esp)` ⇒ 第一实参为常量 0
        //（原写 hackPackId 取错数据源）。
        const unsigned int hackType = _NS_PI_2ND_GetHackType(
            0, cls, static_cast<unsigned short>(index));
        // P0：ORIG 无 analyzer 局部槽（getHackAnalyzer 结果直传 this）。
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1, 0, 0);
        user->lock();
        seen->insert(entry);
        user->unlock();
    }
}

// ============================================================================
// ---- CUser::Parsing（ORIG T 0x08647890，报告 Parsing.md 转写）----
// b28d-solo-W7_UserOnlyC 判词：本件 p167 调用点差集 46/46 CLEAR（0 missing /
// 0 extra），语义完成；byte 层残差 4 条为 N82/N95/N7 族：帧 0x63c vs 0x62c
// （槽基差 0x10）+ cMyTrace 实参寄存器角色（4.1 esi/edi/ebx vs 4.4 ecx/edx/eax）
// + zero 槽载入（4.1 `mov -0x34,%edx` vs 4.4 `mov $0x0,%edx`）。已还原：
// net 引用去缓存（this 直读）、tick/zero/headerLen 具名槽、m_lastRecvGapPair
// u64 整写、size 32 位读。不再耗轮次。
// ============================================================================

#include "W7_UserOnlyC/Parsing.inc"
{
    _header hdr;
    // P0/P183：ORIG 无 net 引用局部（每处 `mov 0x8(%ebp),%eax; mov 0xf8(%eax),…`
    // this 表达式直读）⇒ 全部改 m_network 具名成员访问。

    int remain = static_cast<int>(m_network.m_recvOffset) + received;
    // ORIG 骨架（CUserOnlyOrig.cpp:4524-4529）：tick/zero/headerLen 三具名局部；
    // `m_lastRecvTick=tick; m_recvGapTick=zero;` 由 4.1 合并 8 字节写。
    unsigned int tick =
        static_cast<unsigned int>(GlobalData::s_systemTime_.getCurTickCount());
    unsigned int zero = 0;
    (void)zero;
    // [N1B 2026-09-11 探针] ORIG 槽序（地址递增）：remain、tick、zero、size、
    // headerLen、死槽、verdict ⇒ 4.4 声明序=地址递减，size 须声明在 zero 之后、
    // headerLen 之前（原在循环内声明 ⇒ 得最高槽 -0x24，与 ORIG -0x2c 不符；
    // 提前到 tick 之前 ⇒ 得 -0x34，亦不符）。
    unsigned int size;
    // ORIG 4.1 把相邻两 int 字段合并为单次 8 字节写（`mov eax,0xe4; mov edx,0xe8`）；
    // CNetwork.h 已备 u64 整写视图 m_lastRecvGapPair（P206 先例），4.4 逐条重载
    // this 的问题由此消解。
    m_network.m_lastRecvGapPair = static_cast<unsigned long long>(tick);
    int headerLen = 0xd;                  // ORIG -0x28（照抄；memcpy 用立即数）
    (void)headerLen;

    if (remain <= 0xc)
    {
        m_network.m_recvOffset += received;                 // +0xf8
        m_network.m_recvBuf += received;                    // +0x10c
        cMyTrace t(__PRETTY_FUNCTION__, 0x69d, 0);
        t("(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
          reinterpret_cast<unsigned int>(m_network.m_recvBuf),
          // [N1B 2026-09-11 负结论] `&m_network.m_recv[0]` 形已实测（ORIG +0x8c
          // `lea 0x110(%eax),%edi` = 地址直算；我方 add+mov 两段）⇒ n1_v 归一后
          // 残差**逐字节不变**，回滚。
          reinterpret_cast<unsigned int>(m_network.m_recv), m_network.m_recvOffset,
          received);
        m_network.m_bBufOffsetTrace = 1;                    // bufoffset 标记 +0x109
        return true;
    }

    // [M2S1] ORIG 0x08647ee1/0x08647ee9：尾条件为 `cmpl $0xc,-0x3c; jg +0x10a`
    // （直接回跳循环头，无 setcc 物化）+ 入口 `jmp +0x10b` ⇒ 原码是
    // `for (;;) { ...; if (!(remain > 0xc)) break; }`；旧 do-while/while 形在
    // 4.4.7 恒产 `setg %al; test %al,%al`（探针 /tmp/m2probe_dw.o g1/g2 vs g3）。
    // [N1B 2026-09-11] 探针：ORIG +0x101 `movl $0x0,-0x24(%ebp); jmp +0x10b`
    //（循环入口前一个清零槽，函数其余处不再读）⇒ 试以具名局部复现。
    int zeroPad = 0;
    (void)zeroPad;
    for (;;)
    {
        if (m_network.m_recvOffset != 0)
            m_network.m_recvBuf -= m_network.m_recvOffset;

        if (m_network.m_bBufOffsetTrace != 0)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x6b2, 0);
            t("[BUFOFFSET TRACE](offset:%x - buf:%x) = remainlen:%d, Recv Size[%d]",
              reinterpret_cast<unsigned int>(m_network.m_recvBuf),
              reinterpret_cast<unsigned int>(m_network.m_recv),
              m_network.m_recvOffset,
              received);
        }

        std::memcpy(&hdr, m_network.m_recvBuf, 13);

        // ORIG 0x08647a59 `mov -0x76(%ebp),%eax; mov %eax,-0x2c(%ebp)`：m_size 以
        // 32 位整写入具名 unsigned size 局部（P171 照抄 ORIG 的 32 位读形）。
        size = *reinterpret_cast<const unsigned int*>(&hdr.m_size);

        if (size < 13 || size > 0x578)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x6ce,
                "Recv Size[%d], Parsing Packet Size[%d], recvbuf_offset[%d] is Too Large",
                received, static_cast<int>(size),
                reinterpret_cast<int>(m_network.m_recvBuf));
            return false;
        }

        if (remain < size)
            break;

        int verdict = m_packetVerifier.UpdatePacket(hdr);
        if (verdict != 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x6f4,
                "[Packet Varifier Error %d][classification %d][packet id %d][ServerSequence: %d][ClientSequence: %d](m_id: %s)",
                verdict, static_cast<int>(hdr.m_class),
                static_cast<int>(hdr.m_id),
                static_cast<int>(m_packetVerifier.GetSequence()),
                static_cast<int>(hdr.m_seq), NumberToString(get_acc_id(), 0));
            if (verdict != 4)
                return false;
        }

        if (verdict == 0)
        {
            _NS_PI_2ND_RecvHash_CS(m_network.m_recvBuf, this,
                                   size);

            if (hdr.m_id != 0x25c)
            {
                unsigned char decrypted[0x578];
                int outLen = 0x578;
                G_Cipher()->Decrypt(
                    static_cast<int>(hdr.m_id),
                    reinterpret_cast<const unsigned char*>(
                        m_network.m_recvBuf + 13),
                    size - 13, decrypted, &outLen);
                if (outLen > 0)
                    std::memcpy(m_network.m_recvBuf + 13, decrypted, outLen);

                unsigned int crc = Encryption::crc32N(
                    reinterpret_cast<void*>(m_network.m_recvBuf + 11),
                    size - 11);
                _NS_PI_2ND_MakeChecksumTo1Byte2(crc, hdr.m_checksum);
                if (hdr.m_checksum != crc)
                {
                    cMyTrace t(__PRETTY_FUNCTION__, 0x73a, 5);
                    t("[CHECK SUM] check_sum error PACKET : %s",
                      GetPacketName((ENUM_PACKETCLASS)hdr.m_class, hdr.m_id));
                    return false;
                }
            }

            // [M2S1] ORIG 0x08647d05 `mov -0x2c(%ebp),%eax; add $0x8,%eax`
            // = 用前面已解出的 32 位 size 局部（-0x2c），不是重读 16 位 m_size
            // （旧形多一条 movzwl，P0 槽-局部对应）。
            const int streamLen = static_cast<int>(size) + 8;
            CStreamGuard guard(true);
            // 每个分支各自 Set（ORIG 3 处 Set 调用点，P48）。
            if (streamLen <= 0xfff)
                guard.Set(GlobalData::s_small_stream_pool->Acquire("user.cpp", 0x758));
            else if (streamLen > 0x3fff)
                guard.Set(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x760));
            else
                guard.Set(GlobalData::s_stream_pool->Acquire("user.cpp", 0x75c));

            // P48：每次写入均重调 operator*；put_binary 经 operator->。
            (**guard) << 0;
            (**guard) << static_cast<int>(GetUID());
            guard->put_binary(reinterpret_cast<void*>(m_network.m_recvBuf),
                              size);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            m_network.m_bBufOffsetTrace = 0;
        }

        remain -= size;
        m_network.m_recvBuf += size;
        m_network.m_recvOffset = 0;

        if (remain == 0)
        {
            m_network.m_recvBuf = m_network.m_recv;
            break;
        }
        if (!(remain > 0xc))
            break;
    }

    m_network.m_bBufOffsetTrace = 0;

    if (remain > 0)
    {
        if (remain > 0x1000)
        {
            // ORIG：0x793 logFormat（varargs 可抛）在 try 内；catch(...) 记 0x7a1 后返 0。
            try {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x793,
                    "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                    remain);
            } catch (...) {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x7a1,
                    "[PARSING EXCEPTION] memmove : parsinglength = %d", remain);
                return false;
            }
            return false;
        }
        memmove(m_network.m_recv, m_network.m_recvBuf, remain);
        m_network.m_recvOffset = remain;
        m_network.m_recvBuf = m_network.m_recv + remain;
    }
    return true;
}

// ============================================================================
// ---- CUser::SetSkillLevel（ORIG T 0x0867b51c，报告 SetSkillLevel.md）----
// ============================================================================

// 返回 int：ORIG 0x0867b51c 尾 `mov eax,0x1; ret`（成功）/早退 `mov eax,0x0`
// （runnable 假 / skill==0）——Model22 桥清零证据；签名与 CUser.h 同步（Main 代落）。
// b28d-solo-W7_UserOnlyC 判词：P0/P36 内联 job（%ebx 跨 G_CDataManager 保活）、
// secondGrow（两次独立调用，setle 物化）、treeIdx（%esi 保活）三处具名槽 ⇒
// 131=131，TU 层 AE。
#include "W7_UserOnlyC/SetSkillLevel.inc"
{
    // b28d-52：ORIG 0x0867b528 `movzbl 0x711d8(%eax); xor $1; test; jne` =
    // `!m_gmUserFlag`（P248② bool 字段 + 裸 !field；该偏移 = CUser+0x711d8，
    // CUser+0x705dc 的 m_clientSpec）。原裸偏移 `!= 1` 违反 §4.3 且产 cmp $1/jne。
    // 短路汇聚形（P214①）：`if (!flag || getCurCharacR() == 0) return 0;`
    if (!m_gmUserFlag || getCurCharacR() == 0)
        return 0;

    // P0/P36：ORIG job 无槽（%ebx 跨 G_CDataManager 保活）⇒ 内联实参。
    CSkill* skill = G_CDataManager()->find_skill(get_charac_job(), skillIdx);
    if (skill == 0)
        return 0;

    // P0：ORIG 0x0867b5a2 `test %al,%al; setle` 无 secondGrow 槽（两次独立调用）。
    if (getCurCharSecondGrowType() < 1)
    {
        level = std::max(
            0, std::min(skill->get_type_max_level(getCurCharFirstGrowType()),
                        level));
    }
    else
    {
        int maxLevel = skill->get_second_growtype_max_level(
            getCurCharFirstGrowType(), getCurCharSecondGrowType());
        if (maxLevel == 0)
            maxLevel = skill->get_type_max_level(getCurCharFirstGrowType());
        level = std::max(0, std::min(maxLevel, level));
    }

    // P0：ORIG treeIdx 无槽（%esi 跨调用保活）⇒ 内联实参（右起求值序与 ORIG 一致）。
    reinterpret_cast<SkillSlot*>(getCurCharacSkillW())->growtype_skill(
        get_charac_job(), skillIdx, level,
        (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex());
    return 1;
}

// ============================================================================
// ---- CUser 回馈用户物品族 ----
// ============================================================================

// ORIG T 0x0869230a：等级段映射。
#include "W7_UserOnlyC/getReturnUserLevelKey.inc"
{
    if (mode == 0)
    {
        if (level <= 9) return 5;
        if (level <= 0xe) return 10;
        if (level <= 0x13) return 0xf;
    }
    else
    {
        if (level <= 0x13) return 0xf;
    }
    if (level <= 0x1d) return 0x14;
    if (level <= 0x27) return 0x1e;
    if (level <= 0x31) return 0x28;
    if (level <= 0x3b) return 0x32;
    if (level <= 0x45) return 0x3c;
    return 0x46;
}

// ORIG T 0x08692000，报告 GiveItemsReturnUser.md。
// b28d-solo-W7_UserOnlyC 判词：addInfo 具名槽（去 const 折叠）+ group/msgLen/
// characNo/sender 内联实参（P0/P52）+ P285 空 else 岛 ⇒ 81=81，TU 层 AE。
#include "W7_UserOnlyC/GiveItemsReturnUser.inc"
{
    // ORIG 读 G_CDataManager()+0xb548 = m_returnUserReward.m_field0。
    const int itemIdx = G_CDataManager()->m_returnUserReward.m_field0;
    // ORIG 0x0869201a `movl $0x1,-0x24(%ebp)`：addInfo 为具名局部（P0 槽往返）。
    // 非 const：4.4 对 const 局部会常量传播折成立即数（ORIG 从槽读取）。
    int addInfo = 1;
    Inven_Item item;                                 // ctor 清零
    CItem* base = G_CDataManager()->find_item(itemIdx);
    if (base != 0)
    {
        item.m_addInfo = itemIdx;                    // +0x02 item idx
        base->make_item(item);                       // vtable+8 虚调用
        item.set_add_info(addInfo);

        const char* msgBody =
            g_scriptStringManager_.findString(4, "return_user_msg_1", 0);
        // P0/P52：ORIG 的 group/msgLen/characNo/sender 均无具名槽（esi/edi/ebx/eax
        // 跨调用保活）⇒ 保持内联实参；仅 msgBody 具名（-0x1c 槽）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0),
            item, 100000u, static_cast<unsigned int>(getCurCharacNo()),
            msgBody, static_cast<int>(strlen(msgBody)), 7u, GetServerGroup(),
            false, false);
    }
    else
    {
        return;   // P285：空 else 保留 ORIG 体尾 jmp+nop 岛（4.4 纯正向 if 会 jump-thread）
    }
}

// ORIG T 0x08692140（objdump 直转）：装备回馈走 +0xb550 =
// m_returnUserReward.m_levelUp 表。
// b28d-solo-W7_UserOnlyC 判词：本件已收至 120/121 —— 唯一残差为
// `mov -0x30(%ebp),%eax; mov %eax,%ebx`（4.1 冗余 regmove 跨 G_CDataManager call
// 保活）。4.4 恒折单段 `mov -0x30(%ebp),%ebx`，语句表达式 / unsigned cast 两旋钮
// 实测均无效 ⇒ 按 N18a（反向亦不可造）封顶；调用点差集与语义已对齐。
// [M2S1] p167 复核 ORIG 0x08692140：19/19 调用数相同，差集仅
// LevelUpRewardItemTableKey/LevelUpRewardItemData/Inven_Item 的 **C1↔C2**
// （ORIG 4.1 局部对象 ctor 走 base 版 C2、dtor 走 D2；4.4 恒 C1/D1，同一地址别名，
// 4.4.7 对 local 对象无 C2 选择旋钮）⇒ 工具层编译版本差，封顶。
#include "W7_UserOnlyC/GiveEquipItemsReturnUser.inc"
{
    // P0/P48：ORIG growType/level/growType2/job 均无槽（%ebx/%eax 直传）⇒ 内联实参；
    // key 有具名槽 -0x24。getCurCharFirstGrowType 每次使用均重调（两遍）。
    const int key = getReturnUserLevelKey(get_charac_level(),
                                          getCurCharFirstGrowType());

    LevelUpRewardItemTableKey tableKey(key, get_charac_job(),
                                       getCurCharFirstGrowType());
    LevelUpRewardItemData data;
    // P168a②：ORIG `call; xor $1; test %al; jne` ⇒ bool callee 的否定形
    // `if (!get_data(...))`（4.4 直产 xor 段）。
    if (!G_CDataManager()->m_returnUserReward.m_levelUp.get_data(tableKey,
                                                                 data))
    {
        return;       // P285：外层空 else 保留 ORIG 的 jmp+nop 岛
    }
    else
    {
        Inven_Item item;
        // [N1B 2026-09-11 负结论] ORIG 0x086921f0 段 `mov -0x30(%ebp),%eax;
        // mov %eax,%ebx; call G_CDataManager; mov %ebx,0x4(%esp)` = 实参先算后
        // 跨 call 保活（4.1 冗余 regmove）；`(*G_CDataManager())` 解引用形已实测
        // **无效**（残差逐字不变，仍折单段 `mov -0x30(%ebp),%ebx`）⇒ N7 封顶。
        CItem* base = G_CDataManager()->find_item(data.m_field0);
        if (base != 0)
        {
            item.m_addInfo = data.m_field0;
            base->make_item(item);
            item.set_add_info(data.m_field4);

            const char* msgBody =
                g_scriptStringManager_.findString(4, "return_user_msg_2", 0);
            // P0/P52：group/msgLen/characNo/sender 无具名槽 ⇒ 内联实参。
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "return_user_msg_title",
                                                  0),
                item, 0u, static_cast<unsigned int>(getCurCharacNo()),
                msgBody, static_cast<int>(strlen(msgBody)), 7u,
                GetServerGroup(), false, false);
        }
        else
        {
            return;   // P285：内层空 else 保留 ORIG 的 nop 占位岛
        }
    }
}

// ORIG W 0x084ece20：单字节写 this+0x703d8（m_breakAway.m_field14 bool 直写）。
#include "W7_UserOnlyC/setCharacLinkMessageFlag.inc"
{
    m_breakAway.m_field14 = flag;
}

// ORIG T 0x08688f02，报告 SendLimitItemUsageNotiInfo.md。
#include "W7_UserOnlyC/SendLimitItemUsageNotiInfo.inc"
{
    PacketGuard guard;
    guard.put_header(0, 0x11f);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_int(c);
    guard.finalize(true);
    Send(guard);
}

// ORIG T 0x0868a87a，报告 SendCeraShopGiftStatisticLog.md。
// b28d-solo-W7_UserOnlyC 判词：删 raw 裸偏移指针（§4.3），改 SigTypes_E.h 字段化
// 具名成员（sizeof/偏移/mangled 不变，已 R2 广播）⇒ 82=82，TU 层 AE。
#include "W7_UserOnlyC/SendCeraShopGiftStatisticLog.inc"
                                         unsigned long val64,
                                         unsigned int arg4)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xa2c0), true);
    // ORIG 每次写入均重调 operator*（P48），GetInBuffer 经 operator->。
    (**guard) << 0x29c;
    (**guard) << static_cast<int>(GetUID());

    SIG_STATISTIC_CERASHOP_GIFT_LOG* sig =
        guard->GetInBuffer<SIG_STATISTIC_CERASHOP_GIFT_LOG>();
    // §4.3：具名成员访问（SigTypes_E.h 已按 ORIG 偏移字段化），禁裸偏移。
    sig->m_accId = m_accId;
    sig->m_characNo = static_cast<unsigned int>(getCurCharacNo());
    sig->m_buyIdx = arg1;
    sig->m_cost = arg2;
    sig->m_itemAddInfo = static_cast<unsigned int>(val64);
    sig->m_itemAddInfo2 = arg4;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ============================================================================
// ---- LimitItemUsage（ORIG W；字段按汇编偏移访问，头内布局保留 pad 形态）----
// ============================================================================

// 说明：LimitItemUsage 为 packed 布局（+0x15/+0x19 非对齐 int），
// 具名成员直写（gcc packed 自动非对齐访问）即等价 ORIG 行为。

// ORIG C1/C2 0x08695ef0：CommonTime 子对象刷新当前时间，其余字段清零。
#include "W7_UserOnlyC/LimitItemUsage.inc"
{
    reinterpret_cast<CommonTime*>(this)->SetCurTime();   // P134 内联实参
    m_field10 = 0;
    // fresh ORIG C1 为 movl $0,0x11（int 宽清零；旧"四 movb"注记作废，P196）。
    m_field11 = 0;
    m_field15 = 0;
    m_field19 = 0;
}

// ORIG D1/D2/D0 0x086a6480：空析构（ORIG 仅 ret）。
#include "W7_UserOnlyC/dtor_LimitItemUsage.inc"
{
}

// ORIG W 0x08688fba，报告 LimitItemUsage/isAnotherDay.md。
#include "W7_UserOnlyC/isAnotherDay.inc"
{
    // 具名字段单步 mov disp（P0：ORIG 无中间槽/地址算术）。
    // P0：ORIG 仅 serverSchedule/createTime 两槽；curSec 为内联右值直传。
    const long serverSchedule =
        static_cast<long>(G_CEnvironment()->m_serverEnvir.m_fatigueTime);
    const long createTime = static_cast<long>(
        *reinterpret_cast<const CommonTime*>(this));   // operator long()
    // P56：ORIG 0x08688ffa `test %al,%al; je mov$0; mov$1; jmp` 分支形态，
    // 直 return 表达式会折成单条。
    if (CheckDailyScheduleTime(
            serverSchedule, createTime,
            static_cast<long>(GlobalData::s_systemTime_.getCurSec())))
        return true;
    return false;
}

// ORIG W 0x08695f68。
#include "W7_UserOnlyC/isUsable.inc"
{
    // P0：ORIG 三处 movzbl 直读字段，无 state 缓存槽。
    if (m_field10 == 2)
    {
        if (m_field19 >= m_field11)
            return false;
        return true;
    }
    if (m_field10 == 0 || m_field10 == 1)
    {
        if (isAnotherDay())
            m_field19 = 0;
        if (m_field19 >= m_field11)
            return false;
    }
    return true;
}

// ORIG W 0x08695f28。
#include "W7_UserOnlyC/useIt.inc"
{
    if (!isUsable())
        return false;
    m_field19 += 1;
    reinterpret_cast<CommonTime*>(this)->SetCurTime();
    return true;
}

// ============================================================================
// ---- UserMercenaryInfoMgr::UpdateCharacName（ORIG T 0x081b462e）----
// ============================================================================

#include "W7_UserOnlyC/UpdateCharacName.inc"
                                            std::string const& name)
{
    // [b28c-L29] P218：ORIG 0x081b462e 的 operator[] 实参为「先 idx 后 this」
    // （mov -0xc,%edx; mov 0x8(%ebp),%eax）——下标局部声明 int 触发 size_t
    // 转换中转即复现。
    const int idx = static_cast<int>(FindIndexByCharacNo(characNo));
    if (!CheckCharacIndex(static_cast<unsigned char>(idx)))
        return;

    // [b28c-L29] ORIG 0x081b462e：两次 operator[] 独立调用（无 entry 引用缓存，
    // P183/P157），c_str 结果经 %ebx 跨第二次 operator[] 保活。
    std::memset(m_characs[idx].m_name, 0, 0x1e);
    std::strncpy(m_characs[idx].m_name, name.c_str(), 0x1d);
}

// ============================================================================
// ---- CUserEventUtil::updateCharacterDayEvent（ORIG T 0x08688230）----
// ============================================================================

class CUserEventUtil
{
public:
    static void updateCharacterDayEvent(CUser*, int);
};

#include "W7_UserOnlyC/updateCharacterDayEvent.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x9af5), true);
    // ORIG 每次写入均重调 operator*（P48），GetInBuffer 经 operator->。
    (**guard) << 0x16e;
    (**guard) << static_cast<int>(user->GetUID());

    SIG_UPDATE_CHARACTER_DAY_EVENT* sig =
        guard->GetInBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>();
    sig->m_accId = user->get_acc_id();
    sig->m_day = static_cast<short>(day);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// ---- CDungeon::VerifyMaze 两重载（报告 VerifyMaze*.md 转写）----
// ============================================================================

// ORIG T 0x0834cb7c。
// b28d-solo-W7_UserOnlyC 判词：P197④ size()==0 复现 sete；两循环具名 pos 消双
// operator[]；第三段 find 内联 pair<int&,int&> + P223 三岛 goto 尾标签 ⇒ 345=345，
// TU 层 AE。
#include "W7_UserOnlyC/VerifyMaze.inc"
{
    if (maze.m_width == 1 && maze.m_height == 1)
        return true;

    // P197④：ORIG `call size; test; sete; test; je` ⇒ 写 size()==0。
    if (maze.m_vec14.size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x83e,
            "dungeon index(%d) maze(%d) has error. (Start positions were not declared!)",
            m_index, maze.m_64);
        return false;
    }
    for (unsigned int i = 0; i < maze.m_vec14.size(); ++i)
    {
        // ORIG 一次 operator[] 后存 -0x54/-0x50 两槽（具名 pos，非两次下标）。
        const std::pair<int, int> pos = maze.m_vec14[i];
        const GridScript& grid = maze.getGridR(pos.first, pos.second);
        if (m_maplist[grid.m_gridValue].size() == 0)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x847,
                "dungeon index(%d) maze(%d) has error. (Can't found start map(grid: %d,%d)!)",
                m_index, maze.m_64, pos.first, pos.second);
        }
    }

    if (maze.m_vec20.size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp",
            __PRETTY_FUNCTION__, 0x84e,
            "dungeon index(%d) maze(%d) has error. (End positions were not declared!)",
            m_index, maze.m_64);
        return false;
    }
    for (unsigned int i = 0; i < maze.m_vec20.size(); ++i)
    {
        const std::pair<int, int> pos = maze.m_vec20[i];
        const GridScript& grid = maze.getGridR(pos.first, pos.second);
        if (m_dummyMap324[grid.m_gridValue].size() == 0)
        {
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__, 0x857,
                "dungeon index(%d) maze(%d) has error. (Can't found boss map(grid: %d,%d)!",
                m_index, maze.m_64, pos.first, pos.second);
        }
    }

    for (int y = 0; y < maze.m_width; ++y)
    {
        for (int x = 0; x < maze.m_height; ++x)
        {
            const GridScript& grid = maze.getGridR(y, x);
            // ORIG `test;je d01d` / `ne;test;jne d020` / `ne;test;jne d023` 三岛
            // （P223 多岛：正条件 goto + 尾标签串，标签序=守卫序）。
            if (grid.m_gridValue == 0)
                goto L_grid;
            if (std::find(maze.m_vec14.begin(), maze.m_vec14.end(),
                          std::pair<int, int>(y, x)) != maze.m_vec14.end())
                goto L_start;
            if (std::find(maze.m_vec20.begin(), maze.m_vec20.end(),
                          std::pair<int, int>(y, x)) != maze.m_vec20.end())
                goto L_end;
            if (m_map1a4[grid.m_gridValue].size() == 0)
            {
                // ORIG 0x0834cfdf/0x0834cfe3：vararg3=y、vararg4=x（照抄实参序）。
                LogManager::logFormat(
                    1, "data_manager.cpp",
                    __PRETTY_FUNCTION__,
                    0x86b,
                    "dungeon index(%d) maze(%d) has error. (Can't found grid map(grid: %d,%d)!)",
                    m_index, maze.m_64, y, x);
            }
        L_end:;
        L_start:;
        L_grid:;
        }
    }
    return true;
}

// ORIG T 0x0834c9f0。
// b28d-solo-W7_UserOnlyC 判词：第一循环补具名 MazeScript* 局部（ORIG -0x14 槽，
// P0 槽往返）⇒ 113=113，TU 层 AE。
#include "W7_UserOnlyC/VerifyMaze_1.inc"
{
    if (!(m_index <= 0x7cf))
        return true;

    for (unsigned int i = 0; i < m_mazeList.size(); ++i)
    {
        // P0：ORIG -0x14 槽保存 operator[] 返回的指针（具名局部，非内联实参）。
        const MazeScript* maze = &m_mazeList[i];
        if (!VerifyMaze(*maze))
            return false;
    }
    for (int g = 0; g < 2; ++g)
    {
        for (unsigned int i = 0; i < m_mazeList0[g].size(); ++i)
        {
            MazeScript copy(m_mazeList0[g][i]);
            if (!VerifyMaze(copy))
                return false;
        }
    }
    return true;
}

// ============================================================================
// ---- CStackableItem::GetMonsterCardId（ORIG W 0x0849f530）----
// ============================================================================

#include "W7_UserOnlyC/GetMonsterCardId.inc"
{
    return m_int2b8;
}
