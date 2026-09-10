// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMailBox/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [b28d-38 判词] 本 TU 3 件最终状态：
//  RemoveMail 0x082f.. DIFF（p167 CLEAR 16/16；byte 层 N 封顶）
//  AddNewMail 0x082f.. DIFF（p167 余 make_pair 首参 const& 形：ORIG make_pair<unsigned int const&, CMail*&>）
//  CMail::MakePacket 0x082f.. DIFF（p167 CLEAR 45/45；byte 层 N 封顶）
// df_game_r WongWork::CMailBox / CMailBox::CMail 方法实现（U2 批次）。
// 依据 docs/class_func_reports/WongWork__CMailBox/ 与 WongWork__CMailBox__CMail/
// 各报告 + ORIG objdump 逐函数转写；类布局见 CGameManager.h（CMail 真实字段）。
// CMailBox 对象仍按 ORIG 偏移访问（+0x00..+0x90，与 CGameManager.cpp 中
// ctor/dtor/Init 的偏移视图一致）。

#include "CGameManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "Inven_Item.h"

#include <algorithm>
#include <cstring>
#include <list>
#include <map>
#include <vector>

namespace
{

template <typename T>
inline __attribute__((always_inline)) T& sig_at(void* base, int off)
{
    return *reinterpret_cast<T*>(static_cast<char*>(base) + off);
}

template <typename T>
inline __attribute__((always_inline)) const T& sig_at(const void* base, int off)
{
    return *reinterpret_cast<const T*>(static_cast<const char*>(base) + off);
}
#define SIG_OFF(T, base, OFF) \
    (*reinterpret_cast<T*>(const_cast<char*>(reinterpret_cast<const char*>(base)) + (OFF)))

typedef std::map<unsigned int, WongWork::CMailBox::CMail*> MailMap;
typedef std::map<int, bool> KeepMap;

const int OFF_LOADED      = 0x00;  // bool 加载状态（IsLoaded/SetLoadState）
const int OFF_LASTLOADIDX = 0x04;  // unsigned int（GetLastLoadIdx/SetLoadState 时间）
const int OFF_NOTLOADED   = 0x08;  // int（Get/SetNotLoadedMailCount）
const int OFF_MAILPOOL    = 0x10;  // StaticPool<CMail,20>
const int OFF_MAILMAP     = 0x40;  // map<unsigned int, CMail*>
const int OFF_KEEP        = 0x64;  // map<int,bool>（LetterKeepCount）
const int OFF_LASTLETTER  = 0x7c;  // unsigned int（GetLastLoadLetterIdx）
const int OFF_LOADCNT     = 0x80;  // int（Get/SetLoadedLetterCount）

// b22 桥清零（P201，Model22）：原「CAvatarItemMgr.h 与 CInventory.h 镜像声明
// 冲突」注记过时——CInventory.h:37 已改 include 权威 CAvatarItemMgr.h（本地
// 最小镜像退役，m_avatarItemMgr 即权威类型 0x84），GetAvatarItemMgrR() 直接
// 返回 const CAvatarItemMgr*，两个 asm 标签桥转真 const 成员调用
// （getJewelSocketData W 0x.../GetExpansionInfo，CAvatarItemMgr.h:58-59 声明，
// mangled 与 ORIG 逐字符一致）。

}  // namespace

namespace WongWork
{

// ============================================================================
// CMailBox::CMail —— 访问器（ORIG 反汇编直译）
// ============================================================================

// ORIG 0x085574d4 (W)
#include "CMailBox/GetLetterId.inc"

// ORIG 0x085574f2 (W)
#include "CMailBox/GetPeriod.inc"

// ORIG 0x085574e0 (W)：m_period <= nowSec
#include "CMailBox/IsTimeOver.inc"

// ORIG 0x08557590 (W)：movzbl 直返（bool 字段直返无归一化，P9 勘误）+
// 消费端 0x08552486 `call; xor $0x1,%eax; test %al,%al; je`（P292 bool 形）。
#include "CMailBox/isUnlimit.inc"

// ORIG 0x085574fe (W)
#include "CMailBox/GetSenderName.inc"

// ORIG 0x08557516 (W)
#include "CMailBox/GetPackageMoney.inc"

// ORIG 0x08557522 (W)：sret 按值拷贝 Inven_Item（0x3d 字节）
#include "CMailBox/GetPackageItem.inc"

// ORIG 0x0855750a (W)
#include "CMailBox/GetSenderCharacNo.inc"

// ---- CMailBox::CMail::._398（命名类 t_398；成员 +0x00 unsigned int m_packageMoney /
//      +0x04 Inven_Item m_item）----
// ORIG _ZN8WongWork8CMailBox5CMail5._398C1Ev t 0x085519cc (0x16)：仅 Inven_Item 默认
// 构造（切片 9 条无 packageMoney 清零——原码不初始化该字段，P131 口径）。GCC 对
// ctor/dtor 的 asm 别名无效（硬限制），故以命名类 `t_398`（自然 mangled `5t_398*` >
// 别名 `5._398*`，P203 附注条件②）重建，末尾 `.set`+四件套令点号别名成标注名。
#include "CMailBox/t_398.inc"
{
    // ORIG 0x085519cc：仅 Inven_Item 默认构造（成员自动构造）。
}

// ORIG 0x0855759c (W)：m_packageType == 1
#include "CMailBox/isAvatarPackage.inc"

// ORIG 0x085575ae (W)：m_packageType == 2
#include "CMailBox/isCreaturePackage.inc"

// ORIG 0x085519fe (T)：senderName[0]=0、money=0、item.reset、type=0、unlimit=0
#include "CMailBox/ResetPackage.inc"
{
    m_senderName[0] = '\0';
    m_package.m_item.reset();
    m_package.m_packageMoney = 0;
    m_unlimit = 0;
    m_packageType = 0;
}

// ORIG 0x08551a36 (T)——docs/class_func_reports/WongWork__CMailBox__CMail/MakePacket.md
#include "CMailBox/MakePacket.inc"
                                 CUser const* user) const
{
    packet->put_int((int)m_mailIndex);

    packet->put_int((int)std::strlen(m_senderName));
    packet->put_str(m_senderName, (int)std::strlen(m_senderName));

    packet->put_int(m_package.m_packageMoney);
    packet->put_int(m_package.m_item.m_addInfo);            // item idx（Inven_Item+0x02）
    packet->put_byte(m_package.m_item.m_field0 != 0 ? 1 : 0);         // Inven_Item+0x00

    if (isAvatarPackage())
    {
        // ORIG 8551b25/8551b34：GetRemainDate 返回即 put_int（avatar 分支也 put）
        // ORIG 8551b01：m_addInfo2 先装 ebx、链式调用直传（无具名局部，P52/P183）
        packet->put_int(const_cast<CInventory*>(user->getCurCharacInvenR())
                            ->GetAvatarItemMgrR()->GetRemainDate(
                                m_package.m_item.m_addInfo2, (int)nowSec));
    }
    else
    {
        packet->put_int(m_package.m_item.m_addInfo2);           // avatar/creature uid 域
    }

    packet->put_short((int)m_package.m_item.m_fieldb);

    packet->put_byte((int)(m_package.m_item.GetItemAttr() & 0xff));

    packet->put_int(m_package.m_item.m_fieldd);

    // +0x3d 即 Inven_Item 内嵌 stAmplifyOption_t（m_package.m_item +0x11）
    packet->put_byte((int)(m_package.m_item.m_amp.getAbilityType() & 0xff));
    packet->put_short((int)(m_package.m_item.m_amp.getAbilityValue() & 0xffff));

    if (m_package.m_item.m_field1 == 5)                               // ITEM_TYPE creature
    {
        // ORIG 8551be3：同一 m_addInfo2 先装 ebx 再链式直传（无具名局部）
        packet->put_int(const_cast<user_creature::CCreatureMgr*>(
                            const_cast<CInventory*>(user->getCurCharacInvenR())
                                ->GetCreatureMgrR())
                            ->getRemainTempPeriod(
                                m_package.m_item.m_addInfo2, (int)nowSec));
    }

    packet->put_packet(m_package.m_item);

    if (isAvatarPackage())
    {
        packet->put_int(0x1e);
        // ORIG 8551c64/8551ca2：m_addInfo2 两次独立装载（ebx），getCurCharacInvenR/
        // GetAvatarItemMgrR 各点重取（P48 去缓存，uid 不设具名局部）
        packet->put_binary(const_cast<CInventory*>(user->getCurCharacInvenR())
                               ->GetAvatarItemMgrR()->getJewelSocketData(
                                   m_package.m_item.m_addInfo2),
                           0x1e);

        const void* exp = const_cast<CInventory*>(user->getCurCharacInvenR())
                              ->GetAvatarItemMgrR()->GetExpansionInfo(
                                  m_package.m_item.m_addInfo2);
        packet->put_int(4);
        // [b28d 二轮 L6S6] ORIG 0x08551cde `je 08551cfc<+0x2c6>`：then=非空分支
        //   （put_binary(exp,4) 直落 0x2aa..0x2c4，空分支 init 后置 0x2c6）
        //   ⇒ 正极性 `exp != 0`（P59/P222）。
        if (exp != 0)
        {
            packet->put_binary(reinterpret_cast<const char*>(exp), 4);
        }
        else
        {
            stAvatarExpansionInfo_t init;
            init.init();
            packet->put_binary(reinterpret_cast<const char*>(&init), 4);
        }
    }

    // ORIG 8551d21：call isUnlimit; test %al,%al; je <max 段> ⇒ 正极性直判、then=put_int(0)
    if (isUnlimit())
    {
        packet->put_int(0);
    }
    else
    {
        // ORIG 8551d45..8551d65：无 remain 具名槽，仅 max 的两枚引用临时(-0x14/-0x10)
        // [b28d 二轮 L6S6] ORIG 先算 remain 再 store 常量 1（`sub; mov %eax,-0x14;
        //   movl $0x1,-0x10; lea -0x14→0x4(%esp); lea -0x10→(%esp)`）⇒ cdecl 右起
        //   求值要求常量作 **第二** 实参：`max(1, remain)`（4.4 才先求 remain）。
        packet->put_int(std::max<int>(1, (int)(m_period - nowSec)));
    }

    packet->put_int((int)m_letterId);
}

// ============================================================================
// CMailBox —— 查询 / 计数（偏移视图与 CGameManager.cpp 一致）
// ============================================================================

// ORIG 0x08234566 (W)：map::size @+0x40
#include "CMailBox/GetRecvSize.inc"
{
    return (int)m_mailMap.size();
}

// ORIG 0x08557922 (W)：StaticPool<CMail,20>::Remain @+0x10
#include "CMailBox/GetRemainSize.inc"
{
    return m_mailPool.Remain();
}

// ORIG 0x0823454c (W)
#include "CMailBox/GetLoadedLetterCount.inc"

// ORIG 0x0823455a (W)
#include "CMailBox/GetNotLoadedMailCount.inc"

// ORIG 0x08557944 (W)
#include "CMailBox/GetLastLoadIdx.inc"
{
    return (int)m_lastLoadIdx;
}

// ORIG 0x0855784c (W)
#include "CMailBox/GetLastLoadLetterIdx.inc"
{
    return (int)m_lastLoadLetterIdx;
}

// ORIG 0x08557886 (W)：keep map::size @+0x64
#include "CMailBox/GetLetterKeepCount.inc"
{
    return (int)m_letterKeepCount.size();
}

// ORIG 0x08557866 (W)：clear keep map 并清零 lastLoadLetterIdx
#include "CMailBox/ClearLetterKeepCount.inc"
{
    m_letterKeepCount.clear();
    m_lastLoadLetterIdx = 0;
}

// ORIG 0x084ed222 (W)：map.insert(make_pair(letterId,true))，返回 insert 的 bool
//（新插入 true / 已存在 false；ORIG movzbl insert 返回 pair.second）。
#include "CMailBox/InsertLetterKeepCount.inc"
{
    return m_letterKeepCount
        .insert(std::make_pair(letterId, true)).second;
}

// ORIG 0x08557938 (W)
#include "CMailBox/IsLoaded.inc"

// ORIG 0x08557950 (W)：写加载状态；置 true 时同时写 lastLoadIdx ← nowSec
#include "CMailBox/SetLoadState.inc"
{
    m_bInit = loaded;                              // ORIG mov %dl,(%eax) 直写
    if (loaded)
        m_lastLoadIdx = (unsigned int)nowSec;      // ORIG mov %edx,0x4(%eax)
}

// ORIG 0x085577ac (W)：id==0 → false；否则在 loadLack 列表中查找
#include "CMailBox/FindPackageLoadLack.inc"
{
    if (id == 0)
        return 0;
    return std::find(m_loadLackList.begin(), m_loadLackList.end(), id)
               != m_loadLackList.end();
}

// ORIG 0x08557608 (W)：逐项拷贝 lack 列表前 n 项到 out（越界即停）
#include "CMailBox/GetPackageLoadLack.inc"
{
    for (unsigned int i = 0; i < n && i < m_loadLackList.size(); ++i)
        out[i] = m_loadLackList[i];
}

// ORIG 0x085522c4 (T, _ZNK)：map find，未找到返回 0
#include "CMailBox/GetMail.inc"
{
    MailMap::const_iterator it = m_mailMap.find(mailIndex);
    if (it == m_mailMap.end())
        return 0;
    return it->second;
}

// ORIG 0x08552138 (T)——docs/class_func_reports/WongWork__CMailBox/RemoveMail.md
#include "CMailBox/RemoveMail.inc"
{
    CMail* mail = 0;                       // ORIG movl $0x0 提前清零
    MailMap::iterator it;                  // ORIG 先默认构造再拷贝赋值
    it = m_mailMap.find(mailIndex);
    if (it == m_mailMap.end())
        return 0;

    mail = it->second;
    m_mailMap.erase(it);

    if (mail->GetLetterId() != 0)
    {
        // 若剩余邮件中不再有同 letterId 者 → 已加载计数 +1
        // ORIG 0x085521f0：begin() sret 复用 find 临时槽 -0x2c，随后
        // `mov -0x2c,%eax; mov %eax,-0x1c` 拷回 it（P50/P314 先声明后赋值形）
        // ⇒ 循环必须复用同一具名 it（赋值形），不能另声明 jt（直构进自身槽，
        // 少 2 条拷贝且槽号连锁平移）。
        bool stillExists = false;
        it = m_mailMap.begin();
        while (it != m_mailMap.end())
        {
            if (it->second->GetLetterId() == mail->GetLetterId())
                stillExists = true;
            ++it;
        }
        if (!stillExists)
            m_loadedLetterCount += 1;
    }

    mail->ResetPackage();
    m_mailPool.Free(mail);
    return 1;
}

// ORIG 0x08551f5a (T)——docs/class_func_reports/WongWork__CMailBox/AddNewMail.md
// stAddNewMailInput 布局（AddNewMail 装载证据，与 CMailBoxHelper 写入一致）：
//   +0x00 mailIndex   +0x04 senderCharacNo  +0x08 packageMoney
//   +0x0c letterId    +0x10 remainSec       +0x14 packageType
//   +0x18 period      +0x1c unlimit(byte)   +0x20 const char* senderName
//   +0x24 Inven_Item*（可空）
#include "CMailBox/AddNewMail.inc"
{
    CMail* mail = m_mailPool.Acquire();
    if (mail == 0)
        return 0;

    // 具名成员直访（ORIG 0x08551f86/0x08551f90/0x08551f9c 均为单条
    // `mov K(%edx),%edx` disp 折叠；SIG_OFF 的 char*+offset 形会多发 `add $K`）。
    mail->m_mailIndex      = inputRaw.m_mailIndex;
    mail->m_senderCharacNo = inputRaw.m_senderCharacNo;
    mail->m_package.m_packageMoney   = inputRaw.m_packageMoney;

    // ORIG：srcItem 分支逐字段展开拷贝；空时构造 Inven_Item() 后逐字段搬移。
    // ORIG 0x08551fa8/0x08551fb2 对该指针两次独立装载（`mov 0xc(%ebp),%eax;
    // mov 0x24(%eax),%eax` + `test %eax,%eax` 无栈槽）⇒ 表达式直读两遍（P0）。
    // [b28d 二轮 L6S6] ORIG 0x08551fb0 `je 0855201a<+0xc0>`：**then=非空分支**
    //   （拷贝体直落 0x58..0xbe，空分支后置 0xc0）⇒ 必须写正极性 `!= 0`
    //   （P59/P222；负极性 4.4 恒 `jne` 逆序）。
    if (inputRaw.m_packageItem != 0)
        mail->m_package.m_item = *inputRaw.m_packageItem;
    else
        mail->m_package.m_item = Inven_Item();

    mail->m_period = inputRaw.m_period;

    std::strncpy(mail->m_senderName, inputRaw.m_senderName, 0x1d);

    mail->m_packageType = inputRaw.m_packageType;
    // P292 全链 bool：ORIG 0x085520c4 `movzbl 0x1c(%edx),%edx; mov %dl,0x74(%eax)`
    // 单条直存（无 uchar→bool 归一化）⇒ 源类型同为 bool。
    mail->m_unlimit     = inputRaw.m_unlimit;
    mail->m_letterId    = inputRaw.m_letterId;

    // ORIG 0x085520d7 首实参装载 = `mov 0xc(%ebp),%edx`（&input.m_mailIndex 即
    // input 自身）⇒ 源用 inputRaw.m_mailIndex 而非 mail->m_mailIndex（后者多 1 条）。
    m_mailMap.insert(std::make_pair(inputRaw.m_mailIndex, mail));

    mail->m_remainSec = inputRaw.m_remainSec;
    return mail;
}

}  // namespace WongWork

// ============================================================================
// user_creature::CCreatureMgr —— MakePacket 宠物期限链依赖（U2 批次补定义）
// ============================================================================

namespace user_creature
{

// ORIG 0x08339292 (_ZNK...16FindCreatureItemEi)：m_creatureItems map 查找。
#include "CMailBox/FindCreatureItem.inc"
{
    std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.find(uid);
    if (it != m_creatureItems.end())
        return it->second;
    return 0;
}

// ORIG 0x0833d88c (_ZNK...18getRemainTempPeriodEii)
// docs/class_func_reports/user_creature__CCreatureMgr/getRemainTempPeriod.md：
// m_field30 有 key=uid：v<1 直接返回 v；否则 r=v-nowSec，r==0 时取 1。
// 无记录时委托装备宠的 GetRemainDate（BLOCKED: CCreatureItem 未建模 → -1，推断）。
#include "CMailBox/getRemainTempPeriod.inc"
{
    std::map<int, int>::const_iterator it = m_field30.find(uid);
    if (it != m_field30.end())
    {
        if (it->second > 0)
        {
            int r = it->second - nowSec;
            if (r == 0)
                return 1;
            return r;
        }
        return it->second;
    }
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    // ORIG 委托 CCreatureItem::GetRemainDate(nowSec)。
    return item->GetRemainDate(nowSec);
}

}  // namespace user_creature

// ============================ CMailBox::CMail::._398 别名 ============================
// GCC 对 ctor/dtor 的 asm 别名（asm("...")）无效（硬限制）；匿名嵌套 struct 又按本 TU
// 行号计数命名（如 4._N），与 ORIG `5._398` 不一致。故在类内命名 `t_398`，用 `.set`
// 把自然 mangled `5t_398*` 强制改名为 ORIG 带点号 `5._398*`。
// P203 附注（b28 BL9）：`.set` 别名要成为 objdump 标注名，需 .globl+.type+.size
// 四件套（同址选名 binding 强者胜）且自然名 strcmp > 别名（`5t_398` 第 20 字节
// 't' > `5._398` 的 '.'）。尺寸照 ORIG：C1/C2 0x16。
asm(
    ".globl _ZN8WongWork8CMailBox5CMail5._398C1Ev\n"
    ".type _ZN8WongWork8CMailBox5CMail5._398C1Ev, @function\n"
    ".size _ZN8WongWork8CMailBox5CMail5._398C1Ev, 0x16\n"
    ".set _ZN8WongWork8CMailBox5CMail5._398C1Ev, _ZN8WongWork8CMailBox5CMail5t_398C1Ev\n"
    ".globl _ZN8WongWork8CMailBox5CMail5._398C2Ev\n"
    ".type _ZN8WongWork8CMailBox5CMail5._398C2Ev, @function\n"
    ".size _ZN8WongWork8CMailBox5CMail5._398C2Ev, 0x16\n"
    ".set _ZN8WongWork8CMailBox5CMail5._398C2Ev, _ZN8WongWork8CMailBox5CMail5t_398C2Ev");