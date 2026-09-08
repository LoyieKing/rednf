// df_game_r 称号簿实现（G3-3 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CTitleBook.md（Ghidra C）+ ORIG 反汇编。
#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CTitleBook.h"
#include "CAchievement.h"
#include "CItemAmplifier.h"
#include "CSystemTime.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号（ORIG 真实符号）
// ============================================================================

// ORIG 0x80f7845（W，mangled _Z25GetInvenTypeFromItemSpace14ENUM_ITEMSPACE）：
// switch 跳表（0x8b3c7b0），返回 INVEN_TYPE：0→1, 1→2, 2→4, 3→0, 7→3, 18→5，
// 其余/越界(>0x12)→6。原 GameStubs.cpp 定义迁移。
int GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space)
{
    int result = 6;
    switch ((int)space) {
    case (ENUM_ITEMSPACE)3:   result = 0; break;
    case (ENUM_ITEMSPACE)1:   result = 2; break;
    case (ENUM_ITEMSPACE)0:   result = 1; break;
    case (ENUM_ITEMSPACE)7:   result = 3; break;
    case (ENUM_ITEMSPACE)2:   result = 4; break;
    case (ENUM_ITEMSPACE)0x12: result = 5; break;
    // b28d-163：ORIG 为 0..0x12 全覆盖跳表（未列项与 default 共块）；
    // 补全 case 使 4.4 密度判定选跳表而非 if 链（P163 反向应用）。
    case (ENUM_ITEMSPACE)4:  case (ENUM_ITEMSPACE)5:  case (ENUM_ITEMSPACE)6:
    case (ENUM_ITEMSPACE)8:  case (ENUM_ITEMSPACE)9:  case (ENUM_ITEMSPACE)0xa:
    case (ENUM_ITEMSPACE)0xb: case (ENUM_ITEMSPACE)0xc: case (ENUM_ITEMSPACE)0xd:
    case (ENUM_ITEMSPACE)0xe: case (ENUM_ITEMSPACE)0xf: case (ENUM_ITEMSPACE)0x10:
    case (ENUM_ITEMSPACE)0x11:
    default:  result = 6; break;
    }
    return result;
}
// ---- ORIG 真实符号（自由函数 / 跨类方法，普通 C++ 声明，无 extern "C" 桥） ----
// _checkTimeoutItem（ORIG 0x867cf8d T）定义于 CDataManagerScripts.cpp。
// b28d-solo-CTitleBook（P248 档①）：ORIG 调用点 0x086412ba 为 `call; test %al,%al;
// jne`（bool callee 裸形）；原 int 声明使 4.4 生 `test %eax,%eax`。
bool _checkTimeoutItem(const Inven_Item& item);
// NumberToString（ORIG 0x810904b T，定义于 LinkFinalDeps.cpp）——失败日志用。
char* NumberToString(unsigned int value, int radix);
// GetIntegratedPvPItemAttr（ORIG 0x84fc5ff T）定义于 CDataManagerScripts.cpp。
unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
// DeleteInvalidItemScript::IsInvalidItem2Delete（ORIG 0x8ac2096）定义于
// CDataManagerScripts.cpp（类权威声明见 CDataManager.h，此处本地最小视图）。
class DeleteInvalidItemScript
{
public:
    bool IsInvalidItem2Delete(int idx);
};

// GameWorld（本地最小视图；IsEnchantRevisionChannel 定义于 GameWorld.cpp:3048，
// 符号 _ZN9GameWorld24IsEnchantRevisionChannelEv）
class GameWorld
{
public:
    bool IsEnchantRevisionChannel();
};
GameWorld* G_GameWorld();
class CExpandEquipslot;

// ---- 最小依赖声明（完整 CUser/CDataManager/CInventory 属其它 TU） ----
class CItem
{
public:
    virtual ~CItem();
    virtual void make_item(Inven_Item& item) const = 0;
    bool is_stackable() const;
};

class CDataManager
{
public:
    CItem* find_item(int idx) const;
};

CDataManager* G_CDataManager();

int IsEquipable(CUser* user, const CItem* item, int slot);

// INVEN_TYPE / eItemDelReason 定义于 GameEnums.h（经 CTitleBook.h include）。
class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);
    Inven_Item GetInvenSlot(int invenType, int slot) const;   // ORIG _ZNK10CInventory12GetInvenSlotEii
    int tryInsertItemIntoInventory(Inven_Item item) const;
    int tryInsertItemIntoEquipment(Inven_Item item) const;
    int insertItemIntoInventory(Inven_Item item, eItemAddReason reason,
                                bool bLog, bool b);
    int insertItemIntoEquipment(Inven_Item item, eItemAddReason reason,
                                bool bLog, bool b);
    // b28d-solo-CTitleBook（P248 档①）：ORIG putTitle 调用点 0x0864227d 为
    // `call; xor $0x1,%eax; test %al,%al; je`（bool callee 裸形）⇒ 返回 bool。
    bool delete_item(INVEN_TYPE invenType, int slot, int count,
                     eItemDelReason reason, bool bLog);
};



// CUserCharacInfo::getCurCharacInvenW/R 实际返回 CInventory*（头文件占位为
// Inven_Item*，按 ORIG ABI 使用）
class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenW();
    const CInventory* getCurCharacInvenR() const;
    int getCurCharacNo() const;
    int get_charac_level() const;
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
    int get_charac_no(int type);
    int get_unique_id() const;
    CExpandEquipslot* GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE eType) const;
    void Send(PacketGuard& packet);
    void send_equip(int slot);
    // RewardItem2DeleteInvalidItem（ORIG 0x086931c4，定义于 CUser.cpp:4080）
    void RewardItem2DeleteInvalidItem(
        const std::string& title,
        const std::vector<std::pair<int, int> >& list);
};


// ORIG _ZL14TITLE_BOOK_MAX @0x8e2e484：TU-local static（P204③）
static const int TITLE_BOOK_MAX[4] = { 0x46, 0xc8, 0x78, 0x3c };

// ---- CDataManager 称号表（+0xa788 stTitleBookInfo，0x58 字节） ----
struct CTitleBookInfoRaw
{
    char m_pad0[0xc];                                            // +0x00
    std::vector<stTitleSection> m_sections;                      // +0x0c
    std::map<int, std::vector<stTitleElement> > m_elements;      // +0x18
    char m_pad34[0x8];                                           // +0x34
    std::vector<stAdjust> m_adjustList;                          // +0x3c
};

// CDataManager 局部代理：m_titleBook@+0xa788（成员链让 GCC 折叠为单 lea）
struct CTitleBookMgrRaw
{
    char m_pad0[0xa788];                                         // +0x00
    CTitleBookInfoRaw m_titleBook;                               // +0xa788
};

// 称号簿 DB 存档（stTitleBook_Save，0x6b3e 字节；ORIG 未对齐写 +0x6b3a → pack(1)）
#pragma pack(push, 1)
struct stTitleBook_Save
{
    char m_data[0x6b3a];   // +0x00
    int m_characNo;        // +0x6b3a
};
#pragma pack(pop)
// ============================================================================
// stTitleBook
// ============================================================================

stTitleBook::stTitleBook()
{
    clear();
}

void stTitleBook::clear()
{
    std::memset(m_category0, 0, sizeof(m_category0));
    std::memset(m_category1, 0, sizeof(m_category1));
    std::memset(m_category2, 0, sizeof(m_category2));
    std::memset(m_category3, 0, sizeof(m_category3));
}

CTitleBook::CTitleBook() : m_user(0)
{
    m_category[0] = m_book.m_category0;
    m_category[1] = m_book.m_category1;
    m_category[2] = m_book.m_category2;
    m_category[3] = m_book.m_category3;
}

CTitleBook::~CTitleBook()
{
}

bool CTitleBook::loadData(CUser* pUser, char* pData)
{
    // [b28c-6] N7：ORIG 虚调用 `mov (%eax),%edx; mov this,%eax; mov %eax,(%esp);
    // call *%edx` vs 4.4 恒目标 %eax + this 走 %edx（N7 寄存器互换，临时/引用/
    // 限定名均无效）。ndiff 8 封顶。
    _reset();
    m_user = pUser;
    std::memcpy(&m_book, pData, 0x6b3a);
    return 1;
}

void CTitleBook::getData(char* pData) const
{
    char* dst = pData;   // ORIG: 先将出参 pData 落 -0xc 局部槽再直传 (%esp)
    std::memcpy(dst, (const char*)this + 0xc, 0x6b3a);
}
bool CTitleBook::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("TitleBook.cpp", 0x9c), true);
    **guard << 0x285;
    **guard << (int)pUser->GetUID();
    stTitleBook_Save* pData = guard->GetInBuffer<stTitleBook_Save>();
    if (pData == 0)
        return 0;
    {
        std::memset(pData, 0, 0x6b3e);
        pData->m_characNo = pUser->get_charac_no(-1);
        std::memcpy(pData->m_data, m_book.m_category0, 0x10ae);
        std::memcpy(pData->m_data + 0x10ae, m_book.m_category1,
                    0x2fa8);
        std::memcpy(pData->m_data + 0x4056, m_book.m_category2,
                    0x1c98);
        std::memcpy(pData->m_data + 0x5cee, m_book.m_category3,
                    0xe4c);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}
void CTitleBook::_reset()
{
    m_user = 0;
    m_book.clear();
}

void CTitleBook::_gm_clear()
{
    alter();
    m_book.clear();
}

Inven_Item* CTitleBook::getTitleSlot(ENUM_TITLE_BOOK_CATEGORY category,
                                     int slot)
{
    alter();
    return m_category[(int)category] + slot;
}

Inven_Item CTitleBook::getTitleSlotR(ENUM_TITLE_BOOK_CATEGORY category,
                                     int slot)
{
    return _getTitleR(category, slot);
}

bool CTitleBook::findItemLock(unsigned char lock, ENUM_ITEMSPACE& space,
                              int& slot) const
{
    // b28d-solo-CTitleBook（P2/P25）：ORIG 0x08641124 为双层标准 for 布局
    // （外层 init → jmp 条件 → 内层 init → jmp 条件；外层 `cmpl $3; setle`，
    // 内层 `mov TITLE_BOOK_MAX(,%eax,4); cmp i; setg`）。原 `for(;;)+continue`
    // 形在 4.4 多一条 nop 中继（ndiff 5 NEAR）。
    for (int category = 0; category <= 3; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            if (m_category[category][i].m_amp.GetLock() == lock)
            {
                space = (ENUM_ITEMSPACE)(category + 0x13);
                slot = i;
                return 1;
            }
        }
    }
    return 0;
}

void CTitleBook::getItemLockList(
    std::vector<item_lock::stItemLockRef>& out)
{
    // b28d-183/198：ORIG 0x086411ba 实证——无 item 指针缓存（P183 地址重物化：
    // 每次全量展开 m_category[category][i]）；lock 用「声明预清 + 独立赋值」两句式
    // （P198①，ORIG 头部 movb $0x0,-0x11(%ebp)）；ref 赋值序 = lock → slot → space。
    unsigned char lock = 0;
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            lock = m_category[category][i].m_amp.GetLock();
            if (lock != 0)
            {
                item_lock::stItemLockRef ref;
                ref.m_lock = lock;
                ref.m_slot = (short)i;
                ref.m_space = (char)category + 0x13;
                out.push_back(ref);
            }
        }
    }
}

void CTitleBook::sendList()
{
    for (int category = 0; category < 4; ++category)
    {
        _sendCategoryData(m_user, 0, (ENUM_TITLE_BOOK_CATEGORY)category,
                          ENUM_TITLE_BOOK_INFO_TYPE_0);
    }
}

void CTitleBook::sendListOtherUser(CUser* user,
                                   ENUM_TITLE_BOOK_INFO_TYPE type)
{
    for (int category = 0; category < 4; ++category)
    {
        // P0/P200：ORIG `call get_unique_id; movzwl %ax,%eax; …` 返回值全程
        // 寄存器直传（无 unsigned short 局部槽），category 先入 %ebx 跨 call
        // 保活 ⇒ 表达式直传形。
        _sendCategoryData(user, (unsigned short)m_user->get_unique_id(),
                          (ENUM_TITLE_BOOK_CATEGORY)category, type);
    }
}

bool CTitleBook::isAbleItemLock(unsigned long itemIdx)
{
    // b28d-solo-CTitleBook 终判（N93 槽序封顶）：C28L8 具名 fitFound 已把助记符
    // 序列对齐 ORIG（120=120，NEAR）——ORIG find sret -0x1c / 具名迭代器 it -0x24；
    // 4.4 把 fitFound 落最深处 -0x24、it 退 -0x20（4.1/4.4 具名局部与匿名 sret 的
    // 槽分配方向相反）。裸实参/语句表达式/声明后赋值/头部声明 6 形实测均不可复现。
    for (std::map<int, std::vector<stTitleElement> >::iterator it =
             ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.begin();
         it != ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.end();
         ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            // ORIG 0x8641468 的 std::find 以 unsigned long（itemIdx 原类型）实例化，
            // 不做 (int) 转换。
            // b28d-solo-CTitleBook（C28L8 具名左操作数）：ORIG 0x08642f5a 先求值
            // find（sret -0x1c）再求值比较端 end()（-0x10）；裸形 4.4 先求值右操作数。
            std::vector<int>::iterator fitFound =
                std::find(it->second[i].m_fitJobs.begin(),
                          it->second[i].m_fitJobs.end(),
                          itemIdx);
            if (fitFound != it->second[i].m_fitJobs.end())
            {
                if (it->second[i].m_achievementIdx == -1)
                {
                    return 1;
                }
                return 0;
            }
        }
    }
    return 1;
}

bool CTitleBook::_checkFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                               int slot)
{
    // b28d-solo-CTitleBook（P96/P270）：ORIG 0x08642d42 map::find 为非 const 版
    // （`findERS7_` 无 K），it 即 `iterator`；写 const_iterator 会多一次转换构造
    // （4.4 额外 call + 临时槽 -0x24，帧 0x38→0x48）。
    // 终判（N93 槽序封顶）：pVec 缓存 + 具名 fitFound 后 133→111 条（=ORIG 111，
    // NEAR）；残余仅 find sret 与 it 的槽号互换（ORIG it -0x28 / sret -0x1c；
    // 4.4 fitFound -0x28 / it -0x24），6 形源码实测不可翻。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.find(
            (int)category);
    if (it == ((CTitleBookMgrRaw*)G_CDataManager())
                  ->m_titleBook.m_elements.end())
    {
        return 0;
    }
    // b28d-solo-CTitleBook（P157 反向：ORIG 缓存）：ORIG 0x08642daf 把
    // `(*it).second` 的地址缓存到具名局部 -0xc（ptEv + `add $0x4`），后续
    // size()/三次 operator[] 全部复用该局部；逐次展开会多发 ptEv。
    const std::vector<stTitleElement>* pVec = &(*it).second;
    if ((int)slot >= (int)pVec->size())
    {
        return 0;
    }
    // b28d-solo-CTitleBook（C28L8 具名左操作数）：ORIG 0x08642e2d 先求值 find
    // （sret -0x1c）再求值比较端 end()（-0x10）；裸 `find() != end()` 4.4 先求值
    // 右操作数 ⇒ 具名 find 结果固定求值序与槽序。
    std::vector<int>::const_iterator fitFound =
        std::find((*pVec)[slot].m_fitJobs.begin(),
                  (*pVec)[slot].m_fitJobs.end(),
                  itemIdx);
    if (fitFound != (*pVec)[slot].m_fitJobs.end())
    {
        return 1;
    }
    if ((*pVec)[slot].m_achievementIdx == -1)
    {
        return 1;
    }
    return 0;
}

bool CTitleBook::_getFitSlot(int itemIdx, ENUM_TITLE_BOOK_CATEGORY& category,
                             int& slot)
{
    // [b28c-6] N82/P75：同 isAbleItemLock——std::find 实参装载序 + 迭代器槽基
    // 跨版本差，115=115 全等仅槽/序差。ndiff 24 封顶。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.begin();
    for (; it != ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.end();
         ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            // b28d-solo-CTitleBook（P278 具名返回值，N93 槽序封顶）：ORIG
            // 0x08642baa find sret 落 -0x1c（深于实参临时 end -0x18/begin -0x14）；
            // 4.4 把具名 fitFound 落最深处 -0x24、匿名形落 -0x18（均与 ORIG 差 4）。
            // 实测 6 形（裸实参/具名初始化/声明后赋值/语句表达式/实参具名/头部声明）
            // 均不可复现，判 N93 槽分配方向封顶（115=115 助记符全等）。
            std::vector<int>::iterator fitFound =
                std::find(it->second[i].m_fitJobs.begin(),
                          it->second[i].m_fitJobs.end(),
                          itemIdx);
            if (fitFound != it->second[i].m_fitJobs.end())
            {
                category = (ENUM_TITLE_BOOK_CATEGORY)it->first;
                slot = (int)i;
                return 1;
            }
        }
    }
    return 0;
}

void CTitleBook::deleteTimeoutItem(
    const std::vector<std::pair<int, int> >& list)
{
    // b28d-solo-CTitleBook 终判（P223 边界封顶）：p167 差集 CLEAR（33=33 调用）、
    // 175 vs 177 仅差 else 体尾一条冗余 `jmp .+1; nop`（4.4 jump-threading 失效，
    // jmp 目标即下一条 nop）。6 形实测（if/else-if、else{}+嵌套、continue×2、
    // goto 尾标签×2、空 else）均恒多该 2 条 ⇒ 判结构性伪影封顶。
    std::vector<std::pair<int, int> > toDelete;
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            // b28d-solo-CTitleBook：ORIG 0x08641285 缓存 `const Inven_Item* item`
            //（-0xc）——make_pair 实参取 `lea 0x2(%eax)`（const 左值 ⇒ 模板参
            // `<unsigned long const&, int const&>`）；reset 则重新完整展开
            // `m_category[category][i]`（P183，故 item 必须为 const 指针）。
            const Inven_Item* item = m_category[category] + i;
            if (item->m_addInfo == 0)
            {
                continue;
            }
            if (_checkTimeoutItem(*item) ||
                (_isMatchedItem(item->m_addInfo, list) != -1))
            {
                m_category[category][i].reset();
                alter();
            }
            // §4.3 跨TU隔离例外：本 TU CDataManager 为最小镜像类（仅 find_item）；
            // 真定义 CDataManager.h:3665 `DeleteInvalidItemScript m_deleteInvalidItem; // +0xb658`。
            // 镜像内补该字段会把其落 +0x00 而非 +0xb658 → 真实地址漂移 bug，故保留
            // 具名视图 + 文档化基址访问（同 CItemLock/CMailBoxHelper 判例）。
            else if (reinterpret_cast<DeleteInvalidItemScript*>(
                         (char*)G_CDataManager() + 0xb658)
                         ->IsInvalidItem2Delete(item->m_addInfo))
            {
                // ORIG make_pair<unsigned long const&, int const&>：实参直接取
                // m_addInfo 地址（+0x02）⇒ 用同尺寸 unsigned long 左值视图
                //（P266 先例；布局/取值不变）。
                toDelete.push_back(
                    std::make_pair(
                        *reinterpret_cast<const unsigned long*>(
                            &item->m_addInfo),
                        item->m_addInfo2));
                m_category[category][i].reset();
                alter();
            }
        }
    }
    if (m_user)
    {
        m_user->RewardItem2DeleteInvalidItem(
            std::string("TitleBook"), toDelete);
    }
}

int CTitleBook::_isMatchedItem(
    int itemIdx, const std::vector<std::pair<int, int> >& list)
{
    // P13/P28：ORIG 先 call begin()（槽 -0xc）再 call end()（槽 -0x10），
    // 且循环头只比较缓存槽 ⇒ 同一 for-init 声明 it、e。
    for (std::vector<std::pair<int, int> >::const_iterator it = list.begin(),
             e = list.end();
         it != e; ++it)
    {
        if (it->first == itemIdx)
        {
            return it->second;
        }
    }
    return -1;
}

int CTitleBook::checkError(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                           int slot)
{
    if (!user)
    {
        return 3;
    }
    int result = 0;
    // b28d-46/214：ORIG 0x08641fac 为 switch 决策树（case 0..3 顺序）+ result 局部，
    // 每 case 只写 result 不早返；越界条件合并为 `slot<0 || slot>上限`（短路两跳同落
    // result=3 块，ORIG 0x08641fe8/0x08641ff4 实证）。上限取 ORIG 立即数（0x45/0xc7/
    // 0x77/0x3b = 源码 <0x46/<=199/<0x78/<0x3c 的等价边界）。
    switch ((int)category)
    {
    case 0:
        if ((slot < 0) || (slot > 0x45)) result = 3;
        break;
    case 1:
        if ((slot < 0) || (slot > 199)) result = 3;
        break;
    case 2:
        if ((slot < 0) || (slot > 0x77)) result = 3;
        break;
    case 3:
        if ((slot < 0) || (slot > 0x3b)) result = 3;
        break;
    default:
        result = 3;
        break;
    }
    return result;
}

int CTitleBook::getTitle(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                         int slot, int itemIdx, ENUM_ITEMSPACE space,
                         int& outSlot)
{
    // b28d-solo-CTitleBook 终判：389=389、p167 差集 CLEAR（27=27 调用）⇒ AE。
    // 本件修三处：① cMyTrace 全表达式临时链（P227，ORIG ctor 前右起求 varargs）；
    // ② GetLock 谓词内联（P198②，去具名 uchar 槽往返）；③ 装备分支补
    // `title.m_field0 = 0`（ORIG 0x08642a49 `movb $0x0,-0x91(%ebp)`，真语义缺口）。
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 3;
    }
    if (space == (ENUM_ITEMSPACE)0x12)
    {
        space = (ENUM_ITEMSPACE)3;
    }
    int result = checkError(user, category, slot);
    if (result != 0)
    {
        return result;
    }

    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    Inven_Item title = *book->_getTitle(category, slot);
    if ((itemIdx != 0) && (title.m_addInfo != itemIdx))
    {
        return 2;
    }
    CItem* pItem = G_CDataManager()->find_item(title.m_addInfo);
    if (!pItem)
    {
        return 2;
    }

    if (invenType == 1)
    {
        if (!CTitleBook::_checkMovetoInven(category, slot))
        {
            return 5;
        }
        if (inven->tryInsertItemIntoInventory(title) < 0)
        {
            return 0xb;
        }
        book->_deleteTitle(category, slot);
        int ret = inven->insertItemIntoInventory(
            title, eItemAddReason_7, true, false);
        if (ret < 0)
        {
            // P227：ORIG 0x08641e88 段在 cMyTrace ctor 前右起求 varargs 并跨
            // ctor 保活 ⇒ 全表达式临时链形。
            cMyTrace(__PRETTY_FUNCTION__, 0x2a5, 5)(
                "insertItemIntoInventory failed(title) %s %d %d",
                NumberToString(user->get_acc_id(), 0),
                user->getCurCharacNo(), title.m_addInfo);
        }
        outSlot = ret;
    }
    else if (invenType == 0)
    {
        if (outSlot != 0xb)
        {
            return 7;
        }
        if (inven->tryInsertItemIntoEquipment(title) < 0)
        {
            return 7;
        }
        if (IsEquipable(user, pItem, outSlot) != 0)
        {
            return 6;
        }
        // P198②：ORIG 0x08642a1b 段为 `call GetLock; test %al,%al; je`（直测返回
        // 寄存器、无槽）⇒ 谓词内联；提具名 uchar 局部会多 `mov %al,-off; cmpb $0`。
        if ((title.m_amp.GetLock() != 0) && (title.m_field0 != 0))
        {
            return 1;
        }
        // 真语义缺口（ORIG 0x08642a49 `movb $0x0,-0x91(%ebp)`）：装备分支把称号
        // 移入背包前先清本地副本 title.m_field0（bool +0x00），再 _deleteTitle。
        title.m_field0 = 0;
        book->_deleteTitle(category, slot);
        int ret = inven->insertItemIntoEquipment(
            title, eItemAddReason_7, true, false);
        if (ret < 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2c7, 5)(
                "insertItemIntoEquipment failed(title) %s %d %d",
                NumberToString(user->get_acc_id(), 0),
                user->getCurCharacNo(), title.m_addInfo);
        }
        user->send_equip(ret);
    }
    return 0;
}

int CTitleBook::putTitle(CUser* user, ENUM_ITEMSPACE space, int slot,
                         int itemIdx, ENUM_TITLE_BOOK_CATEGORY category,
                         int titleSlot)
{
    // b28d-solo-CTitleBook 终判（P75/N7 封顶）：p167 差集 CLEAR（33=33 调用）、
    // 335=335 条；已修 Inven_Item C1Ev（sret 直构）、send_equip(slot)、cMyTrace 临时链、
    // delete_item→bool、sectionIdx int 化（消 `lea` vs `add`）。残余 2 处纯装载时机/
    // 寄存器选择差：① GetInvenSlot 的 sret/invenType/slot 装载序（ORIG edx/eax/ecx）；
    // ② delete_item 在压完 3 立即数后才载 invenType，4.4 提前载入 %eax 保活。
    // 属跨版本 O0 调度差（语义/ABI/调用点全等）⇒ P75 封顶。
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 3;
    }
    if (space == (ENUM_ITEMSPACE)0x12)
    {
        space = (ENUM_ITEMSPACE)3;
    }
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);

    // b28d-solo-CTitleBook（P0/P183）：ORIG 0x086420b6 把 GetInvenSlot 的 sret
    // 直构进 invenItem 槽（-0x8d）——「先声明后赋值」会多发一次 Inven_Item::C1Ev
    //（p167 EXTRA +1）。
    Inven_Item invenItem = inven->GetInvenSlot(invenType, slot);
    if ((itemIdx != 0) && (invenItem.m_addInfo != itemIdx))
    {
        return 2;
    }

    // P218：ORIG 0x08642115 先把 sectionIdx 载入 %esi 跨 G_CDataManager 调用保活
    //（`mov -0x24(%ebp),%esi; call; add $0xa794,%eax`）；unsigned 局部使 4.4 改为
    // 调用后重载（`lea 0xa794(%eax),%edx; mov -0x24(%ebp),%eax`）⇒ 索引局部 int 化。
    int sectionIdx = CAchievement::_getSectionIdx(category, titleSlot);
    if (sectionIdx != -1)
    {
        // ORIG 0x8642129：按值拷贝 sections[sectionIdx]（析构后判定），
        // 检查 +0x30 m_lockMessage 非空即锁定、返回 0xc。
        stTitleSection section =
            ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_sections[sectionIdx];
        if (!section.m_lockMessage.empty())
        {
            return 0xc;
        }
    }
    if (user->get_charac_level() < 10)
    {
        return 8;
    }
    if ((category == 2) && (user->get_charac_level() < 0x46))
    {
        return 9;
    }
    if (!CTitleBook::_checkFitSlot(itemIdx, category, titleSlot))
    {
        return 2;
    }
    if (!book->_checkInsertTitle(category, titleSlot, &invenItem))
    {
        return 2;
    }
    if (!inven->delete_item((INVEN_TYPE)invenType, slot, 1,
                            eItemDelReason_0x2b, true))
    {
        return 2;
    }
    if ((invenType == 0) && (slot == 0xb))
    {
        // ORIG 0x0864229a 传 slot 形参（0x10(%ebp)）而非立即数 0xb。
        user->send_equip(slot);
    }

    Inven_Item old = book->getTitleSlotR(category, titleSlot);
    if (old.m_addInfo != 0)
    {
        if (invenType == 1)
        {
            int ret = inven->insertItemIntoInventory(
                old, eItemAddReason_7, true, false);
            if (ret < 0)
            {
                // P227：ORIG 0x086423bb 在 cMyTrace ctor 前右起求 varargs 并跨
                // ctor 以 %esi/%ebx 保活 ⇒ 全表达式临时链形。
                cMyTrace(__PRETTY_FUNCTION__, 0x24f, 5)(
                    "insertItemIntoInventory failed(title) %s %d %d",
                    NumberToString(user->get_acc_id(), 0),
                    user->getCurCharacNo(), old.m_addInfo);
            }
        }
        else if (invenType == 0)
        {
            int ret = inven->insertItemIntoEquipment(
                old, eItemAddReason_7, true, false);
            if (ret < 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x256, 5)(
                    "insertItemIntoEquipment failed(title) %s %d %d",
                    NumberToString(user->get_acc_id(), 0),
                    user->getCurCharacNo(), old.m_addInfo);
            }
            else
            {
                user->send_equip(slot);
            }
        }
    }
    book->_insertTitle(category, titleSlot, &invenItem);
    return 0;
}

int CTitleBook::putTitleOnly(CUser* user, int itemIdx)
{
    ENUM_TITLE_BOOK_CATEGORY category;
    int slot;
    return putTitleOnly(user, itemIdx, category, slot);
}

int CTitleBook::putTitleOnly(CUser* user, int itemIdx,
                             ENUM_TITLE_BOOK_CATEGORY& category, int& slot)
{
    // [b28c-6] N7 残差（ndiff 22→12）：唯一差异为 pItem->make_item 虚调用
    // 目标寄存器 %edx↔%eax 与 this 中转寄存器互换（N7）。
    // P92 已修：_getFitSlot 首参从局部 item.m_addInfo 读（ORIG -0x4b(%ebp)）。
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 0;
    }
    if (itemIdx == 0)
    {
        return 0;
    }
    Inven_Item item;
    item.m_addInfo = itemIdx;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (!pItem)
    {
        return 0;
    }
    pItem->make_item(item);
    item.set_add_info(1);
    // P92：ORIG 0x8641e88 段实参从局部 item 的 +0x02（-0x4b(%ebp)）读取，
    // 非形参 itemIdx（0xc(%ebp)）——两值相等（item.m_addInfo = itemIdx）。
    if (!_getFitSlot(item.m_addInfo, category, slot))
    {
        return 0;
    }
    if (!book->_checkInsertTitle(category, slot, &item))
    {
        return 0;
    }
    book->_insertTitle(category, slot, &item);
    return 1;
}

// ============================================================================
// 内部辅助
// ============================================================================

Inven_Item* CTitleBook::_getTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    alter();
    return m_category[(int)category] + slot;
}

Inven_Item CTitleBook::_getTitleR(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    // ORIG 0x8641db0：无 out 构造（POD 聚合返回=单 memmove 0x3d）；Inven_Item
    // 现含 union 位域后 out 声明会引入构造调用（新头基线实测）。
    return m_category[(int)category][slot];
}

void CTitleBook::_insertTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot,
                              Inven_Item* item)
{
    alter();
    m_category[(int)category][slot].setCopy(*item);
}

void CTitleBook::_deleteTitle(ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    alter();
    m_category[(int)category][slot].reset();
}

bool CTitleBook::_checkInsertTitle(ENUM_TITLE_BOOK_CATEGORY category,
                                   int slot, Inven_Item* item)
{
    if (!item)
    {
        return 0;
    }
    CItem* pItem = G_CDataManager()->find_item(item->m_addInfo);
    if (!pItem)
    {
        return 0;
    }
    if (pItem->is_stackable())
    {
        return 0;
    }
    if ((category == 3) && (item->m_field0 != 0))
    {
        return 0;
    }
    return 1;
}

bool CTitleBook::_checkMovetoInven(ENUM_TITLE_BOOK_CATEGORY category,
                                   int slot)
{
    // b28d-solo-CTitleBook（P96/P270 + P157 反向）：ORIG 0x08641c52 用非 const
    // map::find（`findERS7_` 无 K）⇒ it 为 iterator；且 `(*it).second` 缓存到具名
    // 局部 -0xc（ptEv + `add $0x4`）后复用，operator[] 为 const 版（ixEj 的 K 版）。
    std::map<int, std::vector<stTitleElement> >::iterator it =
        ((CTitleBookMgrRaw*)G_CDataManager())->m_titleBook.m_elements.find(
            (int)category);
    if (it == ((CTitleBookMgrRaw*)G_CDataManager())
                  ->m_titleBook.m_elements.end())
    {
        return 0;
    }
    const std::vector<stTitleElement>* pVec = &(*it).second;
    if ((int)slot >= (int)pVec->size())
    {
        return 0;
    }
    // P56：ORIG 0x08641d01 为 sete/test/je + mov$1/jmp/mov$0 分支形 ⇒ 写 if-return。
    if ((*pVec)[slot].m_achievementIdx == -1)
    {
        return 1;
    }
    return 0;
}

bool CTitleBook::_getOwnerTitleInfo(ENUM_TITLE_BOOK_CATEGORY& category,
                                    int& slot, Inven_Item* out)
{
    // b28d-solo-CTitleBook（P0 寄存器直传 + P183）：ORIG 0x0864199b
    // `call getCurCharacInvenR; …; mov %eax,0x4(%esp)`——返回值无具名局部槽
    //（原写 `const CInventory* inven` 多一条 `mov %eax,-0xc(%ebp)` 并撑帧 +0x10）。
    *out = ((const CInventory*)m_user->getCurCharacInvenR())
               ->GetInvenSlot(0, 0xb);
    // P133/P214①：ORIG 0x08641a34/0x08641a56 两条 `je` 同落尾 mov$0 出口
    //（成功路径 fall-through 到 mov$1）⇒ 正向嵌套 if 包裹；`&&` 会物化 bool、
    // 否定式 `if(!f())` 会多 xor $1。
    if (out->m_addInfo != 0)
    {
        if (_getFitSlot(out->m_addInfo, category, slot))
        {
            return 1;
        }
    }
    return 0;
}

int CTitleBook::_getCategoryItemCount(ENUM_TITLE_BOOK_CATEGORY category)
{
    int count = 0;
    for (int i = 0; i < TITLE_BOOK_MAX[(int)category]; ++i)
    {
        if (m_category[(int)category][i].m_addInfo != 0)
        {
            ++count;
        }
    }
    return count;
}

bool CTitleBook::_empty()
{
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            if (m_category[category][i].m_addInfo != 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

void CTitleBook::_sendCategoryData(CUser* user, unsigned short extra,
                                   ENUM_TITLE_BOOK_CATEGORY category,
                                   ENUM_TITLE_BOOK_INFO_TYPE type)
{
    ENUM_TITLE_BOOK_CATEGORY ownerCategory = (ENUM_TITLE_BOOK_CATEGORY)4;
    int ownerSlot = 0;
    int count = 0;
    Inven_Item ownerItem;
    ownerItem.reset();
    char hasOwner = 0;

    PacketGuard packet;
    packet.put_header(0, 0x165);
    packet.put_byte((signed char)type);
    packet.put_short((int)extra);
    packet.put_int((int)category);
    count = _getCategoryItemCount(category);
    if (extra != 0)
    {
        if (_getOwnerTitleInfo(ownerCategory, ownerSlot, &ownerItem))
        {
            if (category == ownerCategory)
            {
                ++count;
                hasOwner = 1;
            }
        }
    }
    packet.put_int(count);
    for (int i = 0; i < TITLE_BOOK_MAX[(int)category]; ++i)
    {
        Inven_Item* item = m_category[(int)category] + i;
        if (item->m_addInfo != 0)
        {
            _putItemData(packet, (short)i, *item);
        }
    }
    if (hasOwner)
    {
        _putItemData(packet, (short)ownerSlot, ownerItem);
    }
    packet.finalize(true);
    user->Send(packet);
}

void CTitleBook::_putItemData(PacketGuard& packet, short slot,
                              Inven_Item& item)
{
    packet.put_short((int)slot);
    packet.put_int(item.m_addInfo);
    packet.put_int(item.m_addInfo2);
    packet.put_byte((int)GetIntegratedPvPItemAttr(item) & 0xff);
    packet.put_short((int)item.m_fieldb);
    packet.put_byte((int)item.m_field0);
    if (G_GameWorld()->IsEnchantRevisionChannel())
    {
        packet.put_int(0);
    }
    else
    {
        packet.put_int(item.m_fieldd);
    }
    packet.put_byte((int)item.m_amp.getAbilityType() & 0xff);
    packet.put_short((int)item.m_amp.getAbilityValue() & 0xffff);
}
