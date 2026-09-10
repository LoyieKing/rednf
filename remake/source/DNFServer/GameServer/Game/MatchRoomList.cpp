// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MatchRoomList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>

#include "MatchRoomList.h"
#include "CUser.h"
#include "CFairMatch.h"   // fair_pvp::CFairMatch 完整定义（原嵌套 stub 已迁）
#include "LogManager.h"

// ---- CSwitchLog（PvP 日志；与 PvP_Room.cpp 同款局部声明） ----
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);

private:
    char m_pad[0x10];
};

// ============================================================================
// CWaitingRoomList
// ============================================================================

#include "MatchRoomList/CWaitingRoomList.inc"
{
}

#include "MatchRoomList/dtor_CWaitingRoomList.inc"
{
}

#include "MatchRoomList/find.inc"
{
    return m_roomSet.find(room) != m_roomSet.end();
}

#include "MatchRoomList/erase.inc"
{
    m_roomSet.erase(room);
}

#include "MatchRoomList/insert.inc"
{
    return m_roomSet.insert(room).second;
}

#include "MatchRoomList/doMatch.inc"
{
    // b28d-solo-MatchRoomList VERDICT: NEAR（447，110=110；446 后端 AE）| 修法：
    // if(!empty)包裹 + 死局部 found/bestGap/bestIt + gap 具名左值 make_pair +
    // goto 尾块布局 | N7（虚调 GetGap/CheckMatchable 目标 %eax↔%edx，4.4.7 恒 %eax）
    // b28d-solo-MatchRoomList: 还原 ORIG 0x08566d9c——
    // ①`if (!m_roomSet.empty()) { 主流程 } return 0;`（ORIG call empty 后 xor$1/test/je
    //   直跳 return 0；正极性 if 少该 xor 段）；
    // ②ORIG 保留三个未参与主流程的死局部（P171/P232：-O0 仍发初始化 store）：
    //   found(-0xd)/bestGap(-0xc)/bestIt=end()(-0x2c)；
    // ③gap 具名左值 → make_pair<float&,IMatch*&>（ORIG 085694f9 实例），
    //   而非 GetGap 右值直传的 make_pair<float,IMatch*&>。
    if (!m_roomSet.empty()) {
        bool found = false;
        float bestGap = 0.0f;
        std::set<IMatch*>::iterator bestIt = m_roomSet.end();
        (void)found;
        (void)bestGap;
        (void)bestIt;
        for (std::set<IMatch*>::iterator it = m_roomSet.begin();
             it != m_roomSet.end(); ++it) {
            IMatch* cand = *it;
            if (cand == me)
                continue;
            if (!me->CheckMatchable(cand))
                continue;
            float gap = me->GetGap(cand);
            out.insert(std::make_pair(gap, cand));
        }
        goto check_out;
    }
    return 0;
check_out:
    if (out.empty())
        return 0;
    return 1;
}

// ============================================================================
// CLeagueMatchRoomList
// ============================================================================

#include "MatchRoomList/AddRoom.inc"
{
    m_roomMap.insert(std::make_pair(room, 1));
    std::pair<std::map<IMatch*, int>::iterator, bool> ins =
        m_roomMap.insert(std::make_pair(room, 1));
    if (!ins.second)
        ++(ins.first->second);
    return true;
}

#include "MatchRoomList/DelRoom.inc"
{
    std::map<IMatch*, int>::iterator it = m_roomMap.find(room);
    if (it != m_roomMap.end()) {
        m_roomMap.erase(it);
        return true;
    }
    return false;
}

#include "MatchRoomList/Match.inc"
{
    // b28d-solo-MatchRoomList VERDICT: DIFF（447/446 均 136 vs 133）| 修法：
    // found/bestGap 前置 size 检查、bestIt!=end 收口、赋值序 bestGap→bestIt |
    // N7 + 浮点比较 lowering（ORIG flds/fxch/fucompp/fnstsw/sahf vs 4.4 恒
    // fucomip/fstp，TU 旗标不可动 ⇒ 封 N）
    // b28d-M2S3 推翻「旗标不可动」：本 TU 路由 = 446h47 后端 + -march=i486
    // （dnf_game_cxx.sh），x87 比较 lowering 已达 ORIG 形。实测残余只剩
    // 「操作数求值向」：ORIG 0x08567dd7 `flds -0x14(bestGap); flds -0xc(gap);
    // fxch; fucompp; fnstsw; sahf; seta` = `>` 形（探针 w2/w3/w5 直证），
    // 旧源 `gap < bestGap` 发 `fucompp; test $0x45,%ah; sete`（`<` 形）。
    // 两者语义等价（ST0>ST1 @ fxch 后 = bestGap>gap），改写法即可对齐。
    // 复测：135/136，唯一残差 = 尾部 1 条 `ret` 后对齐 nop（§6.1 豁免）⇒ identical 级。
    // b28d-solo-MatchRoomList: 还原 ORIG 0x08567d00——
    // ①found/bestGap 初始化前置于 size() 检查（ORIG movb$0/mov$0 在 call size 之前）；
    // ②循环尾用 `bestIt != end()` 收口（ORIG 无 `if(!found)` 物化）；
    // ③两处汇聚块赋值序 = bestGap 先、bestIt 后（ORIG mov gap→-0x14 在前）。
    bool found = false;
    float bestGap = 0.0f;
    if (m_roomMap.size() == 0)
        return false;
    std::map<IMatch*, int>::iterator bestIt = m_roomMap.end();
    for (std::map<IMatch*, int>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it) {
        IMatch* cand = it->first;
        if (cand == me)
            continue;
        if (!me->CheckMatchable(cand))
            continue;
        float gap = me->GetGap(cand);
        if (!found) {
            bestGap = gap;
            bestIt = it;
            found = true;
        }
        if (bestGap > gap) {
            bestGap = gap;
            bestIt = it;
        }
    }
    if (bestIt != m_roomMap.end()) {
        *out = bestIt->first;
        m_roomMap.erase(bestIt);
        return true;
    }
    return false;
}

// ============================================================================
// CMultiMatchRoomList
// ============================================================================

#include "MatchRoomList/CMultiMatchRoomList.inc"
{
    // b28d-solo-MatchRoomList VERDICT: AE（447/446 均离开 DIFF 列表）| 修法：
    // 去 playCount 局部（条件每轮重调 GetPlayCountPerOneTeam）、make_pair 第二参
    // 具名左值（复现 make_pair<int,CWaitingRoomList&> 实例）、(unsigned char) cast
    // 复现 ORIG movzbl 零扩展 | p167 的 IRoomList C1/C2 为同址别名（P93 豁免）
    // b28d-solo-MatchRoomList: 还原 ORIG 0x08566efc 形——
    // ①无 playCount 局部槽：条件每轮重调 GetPlayCountPerOneTeam（`call;movzbl;cmp;
    //   setg;test;jne`）；
    // ②make_pair 第二参为具名左值 waiting（ORIG 实例 `make_pair<int,
    //   CWaitingRoomList&>`，实参 &-0x68 循环外构造/循环后析构一次）；
    // ③i+1 物化临时槽后取址传引用。
    // 非静态成员经 NULL this 调用（ORIG 恒返 1，不访问 this），同 MatchRoomLCB.cpp。
    CWaitingRoomList waiting;
    for (int i = 0;
         i < (unsigned char)(((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam());
         ++i)
        m_roomMap.insert(std::make_pair(i + 1, waiting));
}

#include "MatchRoomList/AddRoom_1.inc"
{
    char name[255] = {0};
    // b28d-solo-MatchRoomList VERDICT: NEAR（447 167=167；446 167=165）| 修法：
    // if/else 正极性 + find 具名临时拷贝（tmpIt→it，复现 ORIG sret 中转）|
    // N7（虚调 GetPlayerCount 目标 %eax↔%edx）+ N82（name[255] 槽基 -0x12f vs
    // ORIG -0x127 ⇒ sret 临时 disp8/disp32 长度差 6B，帧布局族）
    // b28d-M2S3：①P139/§6.5 手写签名串 → `__PRETTY_FUNCTION__`（ORIG 0x85679e9
    // 实参 = `_ZZN19CMultiMatchRoomList7AddRoomEP6IMatchE19__PRETTY_FUNCTION__`
    // 具名局部静态数组，旧手写串令该符号「仅 ORIG」）；②复测 167/167 且助记符
    // 序列全等（NEAR），残余 19 块全为帧内槽基差（name[255] -0x12f vs ORIG
    // -0x127 / CSwitchLog -0x20 vs -0x24 / sret 临时 -0x28 vs -0x12c）⇒ N82 维持。
    // b28d-L3：同 DelRoom（ORIG 0x8567c35 形）——GetUserName 返回值经 %ebx 跨
    // CSwitchLog ctor 保活并作 log 的 %s 实参；register 提示令 4.4.6 免落槽。
    register char* pName = GetUserName(room, name);
    // b28d-M2S3（P139/§6.5）：ORIG 0x85679e9 实参 =
    // `&_ZZN19CMultiMatchRoomList7AddRoomEP6IMatchE19__PRETTY_FUNCTION__`
    // 具名局部静态数组；旧源手写串令该符号「仅 ORIG」缺失。
    CSwitchLog log(__PRETTY_FUNCTION__, 0x11ea, 0, 0);
    log("pvp@log match(%x), %s ", room, pName);

    for (std::map<int, CWaitingRoomList>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it) {
        if (it->second.find(room)) {
            LogManager::logFormat(1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__, 0x11f3,
                "error find line(%d)", 0x11f3);
            return false;
        }
    }
    int playCount = room->GetPlayerCount();
    // b28d-solo-MatchRoomList: ORIG 为 4.1 初始化式形（find sret 临时 -0x12c 拷回
    // -0x28）；4.4 直接 sret 到 it 槽（少 1 条 mov+临时槽，P112/N82 族封顶）。
    // 「先声明后赋值」反而引入 _Rb_tree_iterator::C1Ev call（ORIG 无）⇒ 不可用。
    std::map<int, CWaitingRoomList>::iterator tmpIt = m_roomMap.find(playCount);
    std::map<int, CWaitingRoomList>::iterator it = tmpIt;
    // b28d-solo-MatchRoomList: 正极性 if（ORIG test%al;je <error>，then=insert 直落）
    if (it != m_roomMap.end()) {
        it->second.insert(room);
    }
    else {
        LogManager::logFormat(1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0x11ff,
            "error find line(%d), playCount(%d)", 0x11ff,
            room->GetPlayerCount());
    }
    return true;
}

#include "MatchRoomList/DelRoom_1.inc"
{
    char name[255] = {0};
    // b28d-L3：ORIG 0x8567c30 调 GetUserName 后 `mov %eax,%ebx` 跨 CSwitchLog
    // ctor 保活，log 的 %s 实参用该返回值（非 name 数组退化地址，0x8567c62
    // `mov %ebx,0xc(%esp)`）⇒ 具名指针局部承载返回值。
    // P231 纯 register 存储类（非 asm 绑定，P0 合规）：4.4.6 对具名指针局部落槽
    // -0xc 并连锁全帧槽平移；register 提示令值随 %ebx 保活（对齐 ORIG）。
    register char* pName = GetUserName(room, name);
    // b28d-M2S3（P139/§6.5）：同 AddRoom，ORIG 用 `_ZZ...DelRoom...__PRETTY_FUNCTION__`。
    // 复测 99 条 + 尾部 1 nop（§6.1 豁免）、仅余 2 处重定位立即数显示差 ⇒ identical 级。
    CSwitchLog log(__PRETTY_FUNCTION__, 0x1209, 0, 0);
    log("pvp@log match(%x), %s ", room, pName);

    for (std::map<int, CWaitingRoomList>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it)
        it->second.erase(room);
    return true;
}

#include "MatchRoomList/Match_1.inc"
{
    // b28d-solo-MatchRoomList VERDICT: NEAR（447，280=280；446 后端 AE）| 修法：
    // perTeam/myCnt 内联表达式（P0）、失败块直落/成功跳后、*vec.begin() 形、
    // doMatch/GetWaitingRoomCombination 返回类型 bool 化（P292）| N7
    // b28d-solo-MatchRoomList: 还原 ORIG 0x085675d0——
    // ①perTeam/myCnt 无局部槽（P0）：比较与实参均内联表达式，GetPlayerCount
    //   先求值存 %ebx，GetPlayCountPerOneTeam 后求值（movzbl 零扩展）；
    // ②成功/失败块序 = `if (!GetWaitingRoomCombination(...)) { AddRoom 循环; return
    //   false; }` 失败直落、成功跳后（ORIG xor$1/test/je <成功块>）；
    // ③vec 元素一律 `*vec.begin()` 形（ORIG 每处调用 begin()+operator*，非 _M_start 直读）。
    if (m_roomMap.empty())
        return false;
    std::vector<IMatch*> vec;
    if (me->GetPlayerCount() <
        (unsigned char)((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam()) {
        if (!GetWaitingRoomCombination(me,
                (unsigned char)((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam() -
                me->GetPlayerCount(), vec))
            return false;
        for (std::vector<IMatch*>::iterator it = vec.begin();
             it != vec.end(); ++it)
            DelRoom(*it);
    }
    if (!GetWaitingRoomCombination(me,
            (unsigned char)((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam(),
            vec)) {
        for (std::vector<IMatch*>::iterator it = vec.begin();
             it != vec.end(); ++it)
            AddRoom(*it);
        return false;
    }
    if (vec.empty())
        return false;
    if (vec.size() == 1) {
        DelRoom(*vec.begin());
        *out = *vec.begin();
        return true;
    }
    IMatch* prev = *vec.begin();
    *out = prev;
    for (std::vector<IMatch*>::iterator it = vec.begin() + 1;
         it != vec.end(); ++it) {
        DelRoom(*it);
        prev->SetNextMatch(*it);
        prev = *it;
    }
    return true;
}

#include "MatchRoomList/GetWaitingRoomCombination.inc"
                                                    std::vector<IMatch*>& out)
{
    // b28d-solo-MatchRoomList VERDICT: DIFF（447/446 均 424 vs 435）| 修法：
    // for+break 重构、totalPlayers 头声明、min 直读、doMatch 两独立守卫、
    // avgScore 前置、bestAvg/bestIdx 赋值序、count int 物化 | N82（槽布局）
    // + N86b（三维下标强度归约：4.4 恒 imul $0x8c/add 链 vs ORIG imul
    // $0x46/scaled；一维视图探针 g1b 实测净负已回退）
    // b28d-M2S3 复评（旧 N 判词逐条重试）：424 vs 434（gap +10）。已落地：
    // ①语义轴：`avgScore[i] == bestAvg` → `avgScore[i] < bestAvg`（ORIG
    //   0x8567486 `flds avgScore[i]; flds bestAvg; fucompp; fnstsw %ax;
    //   test $0x45,%ah; sete` 判 ST0>ST1 ⇒ bestAvg>avgScore[i] 才赋值 ⇒ 取最小；
    //   旧源 `==` 系 Ghidra 误读，属真语义 bug）；
    // ②P0：combIdx 去声明初值（-0x38 双写）、count 改 int（去 find 实参 int 临时）；
    // ③P329 求值序：两处 float 乘法左操作数换位（先 flds 数组 / mit->first）；
    // ④P222：内层 empty 检查改 continue 形（去 xor $1）；
    // ⑤P339：j/k/found/bestIdx/bestAvg/i 全头部声明（mnemonic 差块 31→17）；
    // ⑥P324：尾块序 `if (resultCount != 0) {…return 1;} return 0;`（17→15）。
    // 残余 15 块：4.4.6 强度归约 `add %eax,%eax` vs ORIG `lea (%eax,%eax,1),%edx`
    // （2i 生成差）×4 + 匿名临时槽阶（-0x68/-0x64、-0x50）。
    // 【A/B 直证】探针 /tmp/m2s3probe/Game/MatchRoomList.cpp：444 后端复现 ORIG
    // 的 lea 形，但整 TU 444 实测 blocks=39（劣于 446 的 17）⇒ 维持 446 路由；
    // 该残余判 N86b（后端 mul 展开差异，源码不可达）。
    // 与 ORIG 同名的函数内静态常量/组合表（数据导出自 0x8ca3200，
    // 0x8ca35d4 起为四个 1 字节常量）。
    static const unsigned char MAX_SAME_COUNT = 2;
    static const unsigned char MAX_WAITING_ROOM_COUNT = 7;
    static const unsigned char MAX_COMBINATION_COUNT = 10;
    static const unsigned char MAX_REMAIN_WAITING_ROOM = 7;
    static const unsigned char CombinationTable[MAX_REMAIN_WAITING_ROOM]
        [MAX_COMBINATION_COUNT][MAX_WAITING_ROOM_COUNT * 2] = {
        /* playCount=0（无效区，全 0） */
        { {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=1：1 队 x 1 人 */
        { {1,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=2：1x2 / 2x1 */
        { {1,2,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=3：1x3 / 2x1+1x1 / 3x1 */
        { {1,3,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=4：1x4 / 2x1+1x2 / 2x1+2x1 / 3x1+1x1 / 4x1 */
        { {1,4,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,2,0,0,0,0,0,0,0,0,0,0},
          {2,1,2,1,0,0,0,0,0,0,0,0,0,0}, {3,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {4,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=5：1x5 / 2x1+1x3 / 2x2+1x1 / 3x1+2x1 / 3x1+1x2 / 4x1+1x1 */
        { {1,5,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,3,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,1,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=6：1x6 / 2x1+1x4 / 2x2+1x2 / 2x3 / 3x1+1x3 /
           3x1+2x1+1x1 / 4x1+1x2 / 4x1+2x1 / 5x1+1x1 */
        { {1,6,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,4,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,2,0,0,0,0,0,0,0,0,0,0}, {2,3,0,0,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,3,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,1,1,0,0,0,0,0,0,0,0},
          {4,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {5,1,1,1,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
    };

    int totalPlayers = 0;
    float avgScore[MAX_COMBINATION_COUNT] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<IMatch*> combination[MAX_COMBINATION_COUNT];
    std::multimap<float, IMatch*> matched;
    int resultCount = 0;
    // b28d-S3（P0）：ORIG 0x85670e2 只写一次 -0x34；旧源声明处 `= 0` + for-init
    // 再写一遍 ⇒ 我方 -0x38 双写（+1 movl）。去声明初值，保留 for-init。
    int combIdx;
    // b28d-M2S3（P339）：ORIG 具名槽阶梯 = combIdx(-0x34) j(-0x30) k(-0x2c)
    // found(-0x25) bestIdx(-0x24) bestAvg(-0x20) i(-0x1c) ⇒ 全部函数头声明，
    // 初始化仍留在原语句位置（ORIG 0x856741d 起才写初值）。旧源把 j/k/i 声明
    // 在块内 ⇒ 槽落到 -0x24/-0x20（与 ORIG 差 0xc）。
    int j;
    int k;
    bool found;
    int bestIdx;
    float bestAvg;
    int i;

    // b28d-solo-MatchRoomList: for 循环 + 首行标志 break（ORIG 条件在底部、
    // 收尾块在循环外）；totalPlayers 函数头声明、每轮重置。
    // 注：一维视图 (ci*7+pc*70+j)*2 探针（/tmp/b28d_gwc_probe.cpp g1b）实测
    // 外层 j=0 被 4.4 常量折叠回 imul $0x8c、内层多 1 条中转 ⇒ 净负，保留
    // 三维下标（4.4 强度归约与 ORIG 不同形，判 N86b）。
    for (combIdx = 0; combIdx <= 9; ++combIdx) {
        if (CombinationTable[playCount][combIdx][0] == 0)
            break;
        totalPlayers = 0;
        for (j = 0; j < MAX_WAITING_ROOM_COUNT; ++j) {
            if (CombinationTable[playCount][combIdx][j * 2] == 0)
                break;
            matched.clear();
            // ORIG：if 检查为表达式直读（test %al），find 侧再算一次并物化 int 槽
            // b28d-S3：ORIG 0x856717f 单槽 `mov %eax,-0x50` + `lea -0x50` 直传
            // find；旧源 `unsigned int` 触发 int 临时转换（多 2 条 mov）。
            int count =
                (unsigned char)CombinationTable[playCount][combIdx][j * 2];
            // ORIG 对 find 结果不做 end() 检查（GetPlayCountPerOneTeam=1 时
            // 组合表只使用 key=1 的列表），此处忠实还原。
            std::map<int, CWaitingRoomList>::iterator it = m_roomMap.find(count);
            // ORIG 两个独立守卫（xor$1/test/jne 与 size<min 各一 break 岛）
            if (!it->second.doMatch(me, matched))
                break;
            if (matched.size() <
                CombinationTable[playCount][combIdx][j * 2 + 1])
                break;
            k = 0;
            for (std::multimap<float, IMatch*>::iterator mit = matched.begin();
                 mit != matched.end() &&
                 k < CombinationTable[playCount][combIdx][j * 2 + 1];
                 ++mit, ++k) {
                // b28d-S3（P329 求值序）：ORIG 0x8567233 先 `mov -0x34,%ebx` +
                // `flds avgScore[combIdx]` 再 `fildl -0x3c` ⇒ 乘法左操作数 =
                // avgScore[combIdx]；第二项 ORIG 0x8567250 先 `flds mit->first`
                // 溢出 -0x128 再取 GetPlayerCount() ⇒ 左 = mit->first。
                // 两处均为 float 乘法，交换律 ⇒ 语义不变，仅求值序对齐。
                avgScore[combIdx] = (avgScore[combIdx] * totalPlayers +
                                     mit->first * mit->second->GetPlayerCount()) /
                                    (float)(mit->second->GetPlayerCount() + totalPlayers);
                totalPlayers += mit->second->GetPlayerCount();
                combination[combIdx].push_back(mit->second);
            }
        }
        if (!combination[combIdx].empty()) {
            if (totalPlayers == playCount)
                ++resultCount;
            else
                combination[combIdx].clear();
        }
    }
    // b28d-M2S3（P324 块序）：ORIG 0x8567413 `cmpl $0x0,-0x38; je <return-0 块
    // +0x137>` ⇒ 成功块直落、return 0 跳后。旧源 `if (resultCount == 0)
    // return 0;` 令 4.4 发 `jne <成功块>`（return-0 块直落），块序相反。
    if (resultCount != 0)
    {
        found = false;
        bestIdx = 0;
        bestAvg = 0.0f;
        for (i = 0; i < MAX_COMBINATION_COUNT; ++i) {
            // b28d-S3：ORIG 0x8567451 `call empty; test %al,%al; jne <循环尾>`
            // （无 `xor $1`）⇒ 源码为 continue 形（旧源 `if (!...)` 多 1 条 xor）。
            if (combination[i].empty())
                continue;
            if (!found) {
                bestIdx = i;
                bestAvg = avgScore[i];
                found = true;
            }
            // [语义轴修正·b28d-S3] 旧源 `avgScore[i] == bestAvg` 是错译：ORIG
            // 0x8567486 形 `flds avgScore[i]; flds bestAvg; fucompp; fnstsw %ax;
            // test $0x45,%ah; sete`（C3C2C0=000 ⇔ ST0>ST1 ⇔ bestAvg>avgScore[i]）
            // 且赋值块 bestAvg=avgScore[i] ⇒ 真语义 = 取最小 avgScore。
            if (avgScore[i] < bestAvg) {
                bestAvg = avgScore[i];
                bestIdx = i;
            }
        }
        out.insert(out.end(), combination[bestIdx].begin(),
                   combination[bestIdx].end());
        return 1;
    }
    return 0;
}

// ============================================================================
// fair_pvp::CFairMatch::GetPlayCountPerOneTeam（ORIG 0x08562834 恒返 1）
// 由 MatchRoomLCB.cpp 唯一定义；GetUserName 及三类房间表方法为
// MatchRoomList.cpp 唯一 owner（key function → vtable 0x8ca3ba8/0x8ca3bc0）。
// ============================================================================
