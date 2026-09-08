// df_game_r Game/ WongWork::CHackAnalyzer 实现（G6 missing 批次，权威头
// CHackAnalyzer.h；Secu_AccountHacking 全部方法 + CHackAnalyzer 缺失方法）。
// 依据 docs/class_func_reports/WongWork__CHackAnalyzer/*.md 与
// Secu_AccountHacking/*.md 逐函数还原（##汇编 + ##反编译C 两段均已读）；
// 报告缺失的 4 个小符号（getServerHackCnt / resetServerHackCnt /
// setServerHackAccumulatedCnt / checkCollectedHack）以 ORIG nm + objdump
// 区间取证后转写并明确标注「报告缺失」。附带落 DB_SelectCharacMid /
// DB_ReqSaveHackUserPunish::makeRequest（各自报告在手，此前无 owner）。
#include "CHackAnalyzer.h"

#include <algorithm>
#include <iostream>   // b28d-solo-CHackAnalyzer: ORIG TU 静态初始化含
                      // std::ios_base::Init C1+__cxa_atexit（0x080f97b4 实证）
                      // ⇒ 原 TU 经头链引入流头；补此 include 复现该 TU 级符号。
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameRand.h"
#include "CParty.h"
#include "CDungeon.h"
#include "CBattle_Field.h"
#include "CSystemTime.h"
#include "PacketBuf.h"
#include "DB_SelectCharacMid.h"
#include "DB_ReqSaveHackUserPunish.h"
#include "W6_HackPunishDeps.h"

// ---- 静态概率表（ORIG .rodata 0x8b3e094 / 0x8b3e09c，objdump -s 实测）----
static const short _S_HACK_CHECK_RATE_BY_PARTYCOUNT[4] = {100, 50, 33, 25};
static const short _S_HACK_CHECK_RATE_BY_DIFFICULT[5] = {25, 20, 15, 10, 0};

// 浮点比率系数（ORIG flds 0x8b3e2a0 / 0x8c00fd0，objdump 实测均 100.0f）
static float _S_HACK_RATIO_COEF_100F = 100.0f;  // 非 const：ORIG flds 内存读形（const 会被折叠成 fldl）

namespace WongWork
{

// 计数区寻址（ORIG 实测：A 表槽 = this + (t-0x65)*4 + 0x28，即
// m_cntRegion(=this+0x28) + (t-0x65)*4；B 表槽 = this + (t-0x65+0xf44)*4 + 8，
// 即 m_cntRegion + (t-0x65+0xf44)*4 + 8 - 0x28。getServerHackCnt 0x08695086 /
// addServerHackCnt 0x080f8c7e 反汇编核实）。
#define HACK_CNT_A(T) (m_serverHackCntA[(int)(T) - 0x65])
#define HACK_CNT_B(T) (m_serverHackCntB[(int)(T) - 0x65])
// 数组成员式槽访问（ORIG resetServerHackCnt 0x0869509c / setServerHackAccumulatedCnt
// 0x084eb57c / resetServerHackAccumulatedCnt 0x086950b6 的 lea idx+disp8 复合寻址形）。
#define HACK_SLOT_A(T) (m_serverHackCntA[(int)(T) - 0x65])
#define HACK_SLOT_B(T) (m_serverHackCntB[(int)(T) - 0x65])

// ORIG WongWork::CAutoPunishRuleHackTypeMgr 布局镜像（权威定义在
// GlobalData.cpp；此处仅按偏移只读访问 m_exceptionMap @+0x58）。「推断」标注：
// 偏移来自 mgr ctor 反汇编 0x82a5b1a。
// 报告缺失符号（nm W 0x080f9876，objdump 逐指令转写，0x1c 字节）：
// return ((m_flag04 & flag) == flag) —— sete 规范化 bool。
bool CHackAnalyzer::checkCollectedHack(ENUM_REPORT_4_HACK_FLAG flag)
{
    return (((unsigned char)m_flag04 & flag) == flag);
}

CHackAnalyzer::CHackAnalyzer()
{
    // ORIG C1 0x086950d2：隐式构造五个容器成员，再 resetHackInfo、
    // memset(this+0x28,0,0x3cf0)、AddCheckRateHackTypeList。
    resetHackInfo();
    memset(m_serverHackCntA, 0, 0x3cf0);
    AddCheckRateHackTypeList();
}

CHackAnalyzer::~CHackAnalyzer() {}

ENUM_HACKTYPE CHackAnalyzer::getBaseHackType() { return (ENUM_HACKTYPE)m_field7a08; }

// 旧判词 N（装载序互换）已翻案：P95 视图访问复现 ORIG「先 this 后值」序
//   （探针 /tmp/setter_probe.cpp S2::set 逐条同形，ORIG 0x080f9856）。
namespace {
struct CHackAnalyzer7a08View { char m_pad[0x7a08]; int m_field7a08; };
}
void CHackAnalyzer::setBaseHackType(ENUM_HACKTYPE t)
{
    reinterpret_cast<CHackAnalyzer7a08View*>(this)->m_field7a08 = (int)t;
}

void CHackAnalyzer::setUserP(CUser* user)
{
    // ORIG T 0x080f79c0（setUserP.md）：*(this+0) = user（与 beginCollectHackInfo
    // 共用首槽槽位复用）。
    // 指针赋值形（ORIG 先 load this 再 load 实参；int cast 形会先物化实参）。
    *(CUser**)&m_baseHackType = user;
}

CUser* CHackAnalyzer::getUserP()
{
    // ORIG T 0x080f79ce（getUserP.md）：非零路径 fallthrough（回读同槽），
    // 空值返回置函数尾（与 ORIG 分支排布一致）。
    if (m_baseHackType != 0)
        return (CUser*)m_baseHackType;
    return (CUser*)0;
}

int CHackAnalyzer::getServerHackCnt(ENUM_HACKTYPE type)
{
    // 报告缺失（nm W 0x08695086）：读 A 表槽（ORIG：lea -0x65 先于 this 载入，
    // 复合寻址 0x8(%eax,%edx,4)）。
    return HACK_SLOT_A(type);
}

void CHackAnalyzer::resetServerHackCnt(ENUM_HACKTYPE type)
{
    // 报告缺失（nm W 0x0869509c）：A 表槽清零。
    HACK_SLOT_A(type) = 0;
}

void CHackAnalyzer::setServerHackAccumulatedCnt(ENUM_HACKTYPE type,
                                                unsigned int cnt)
{
    // 报告缺失（nm W 0x084eb57c）：B 表累计槽直写（lea idx-0x65+0xf44 ×4 +8）。
    HACK_SLOT_B(type) = (int)cnt;
}

void CHackAnalyzer::resetServerHackAccumulatedCnt(ENUM_HACKTYPE type)
{
    // ORIG W 0x086950b6：B 表累计槽清零（与 addServerHackCnt 同寻址）。
    HACK_SLOT_B(type) = 0;
}

void CHackAnalyzer::setLastMonsterDeadTime(long t)
{
    // ORIG W 0x085df98c：写 +0x24 时间戳（=m_field24，resetHackInfo 清零项）。
    m_field24 = (int)t;
}

void CHackAnalyzer::beginCollectHackInfo(CUser* user)
{
    // ORIG 0x080f79e8：+0x00 收 user、+0x20=系统当前秒、+0x24 清零、
    // +0x14 vector<int> clear（m_checkTypes）。
    *(CUser**)&m_baseHackType = user;  // ORIG *(int*)this = user（槽位复用，指针赋值序）
    m_field20 = GlobalData::s_systemTime_.getCurSec();
    m_field24 = 0;
    m_checkTypes.clear();
}

bool CHackAnalyzer::CheckExceptionCnt(ENUM_HACKTYPE type, int cnt)
{
    // ORIG T 0x080f972a：GetExceptionCnt sret 调用，命中复制 {cnt,flag}，否则置零。
    stHacktypeExceptionInfo_t info =
        GlobalData::g_autoPunishRuleHackTypeMgr->GetExceptionCnt((int)type);

    if (info.m_cnt != 0)
    {
    // ORIG：res 先默认构造，再 insert 结果移动赋值；make_pair(type,1) 无 cast
    // （first 以 ENUM_HACKTYPE& 绑参数槽）。
        std::pair<std::map<int, int>::iterator, bool> res;
        res = m_exceptionCntMap.insert(std::make_pair(type, 1));
        if (!res.second)
        {
            res.first->second = res.first->second + cnt;
            if (res.first->second >= (int)info.m_cnt)
                return false;
        }
        return true;
    }
    if (info.m_flag != 0 && (int)info.m_flag >= cnt)
        return true;
    return false;
}

// b28d-solo-CHackAnalyzer: AE（const_iterator 转换构造 + (int)type 内联实参
// 复现 ORIG 0x080f8320 的 P96/P236 形；旧「N 封顶 sret lowering」判词作废）。
bool CHackAnalyzer::CheckHackRateByPartyCount(ENUM_HACKTYPE type, int count)
{
    // ORIG 0x080f8320：std::find 于 m_checkRateHackTypes(+0x7a24) 查 type；
    // 未命中返回 true；idx=count-1 越界或费率==100 返回 true；
    // 否则命中当 get_rand_int(100) <= 费率。
    std::vector<int>::const_iterator it = std::find(
        m_checkRateHackTypes.begin(), m_checkRateHackTypes.end(), (int)type);
    if (it == m_checkRateHackTypes.end())
        return true;

    int idx = count - 1;
    if (idx < 0 || idx > 3 || _S_HACK_CHECK_RATE_BY_PARTYCOUNT[idx] == 100)
        return true;
    // ORIG 尾段 if 常量岛（同 CheckHackRateByDifficult b27 判定）。
    if (get_rand_int(100) <= _S_HACK_CHECK_RATE_BY_PARTYCOUNT[idx])
        return true;
    return false;
}

// b28d-solo-CHackAnalyzer: AE（同 CheckHackRateByPartyCount 的 P96/P236 形；
// ORIG 0x080f842e）。
bool CHackAnalyzer::CheckHackRateByDifficult(ENUM_HACKTYPE type, int lvDiff,
                                             int difficult)
{
    // ORIG 0x080f842e：std::find 查 m_vec7a30(+0x7a30)；未命中返回 true；
    // type==0x86 时 lvDiff<<1；difficult 越界返回 true；chance=100-lvDiff-表值，
    // <=0x18 且非 0x86 时抬到 0x19；命中当 get_rand_int(100) <= chance。
    std::vector<int>::const_iterator it = std::find(m_vec7a30.begin(),
                                                    m_vec7a30.end(), (int)type);
    if (it == m_vec7a30.end())
        return true;

    if ((int)type == 0x86)
        lvDiff = lvDiff << 1;
    int chance = 100 - lvDiff;   // ORIG 初始化在界检查前，-= 表值在检查后
    if (difficult < 0 || difficult > 4)
        return true;
    chance -= _S_HACK_CHECK_RATE_BY_DIFFICULT[difficult];
    if (chance <= 0x18 && (int)type != 0x86)
        chance = 0x19;
    // ORIG 尾段 if 常量岛（setle 后 je mov$1/jmp/mov$0 块形，非 setcc 直返）。
    if (get_rand_int(100) <= chance)
        return true;
    return false;
}

// b28d-solo-CHackAnalyzer: AE（299/299，无实质残差；ORIG 0x080f8550）。
void CHackAnalyzer::AddCheckRateHackTypeList()
{
    // ORIG 0x080f8550：前 18 项压入 m_checkRateHackTypes(+0x7a24)，
    // 后 24 项压入 m_vec7a30(+0x7a30)；均为栈上 int 变量逐值 push_back。
// P0 推定：ORIG 42 个独立栈槽（-0xb0..-0xc 各用一次）= 42 个具名局部；
// 4.4 首声明占最浅槽 ⇒ 声明序倒排（v42..v1）使 v1 落 -0xb0 与 ORIG 同序。
    int v42, v41, v40, v39, v38, v37, v36, v35, v34, v33, v32, v31, v30, v29, v28, v27, v26, v25, v24, v23, v22, v21, v20, v19, v18, v17, v16, v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1;
    v1 = 0x84; m_checkRateHackTypes.push_back(v1);
    v2 = 0xdc; m_checkRateHackTypes.push_back(v2);
    v3 = 0x138; m_checkRateHackTypes.push_back(v3);
    v4 = 0x139; m_checkRateHackTypes.push_back(v4);
    v5 = 0x86; m_checkRateHackTypes.push_back(v5);
    v6 = 0xde; m_checkRateHackTypes.push_back(v6);
    v7 = 0x1a4; m_checkRateHackTypes.push_back(v7);
    v8 = 0x1a5; m_checkRateHackTypes.push_back(v8);
    v9 = 0x1a7; m_checkRateHackTypes.push_back(v9);
    v10 = 0xdf; m_checkRateHackTypes.push_back(v10);
    v11 = 0xe0; m_checkRateHackTypes.push_back(v11);
    v12 = 0xe1; m_checkRateHackTypes.push_back(v12);
    v13 = 0x1a4; m_checkRateHackTypes.push_back(v13);
    v14 = 0x1a5; m_checkRateHackTypes.push_back(v14);
    v15 = 0x1a7; m_checkRateHackTypes.push_back(v15);
    v16 = 0xdf; m_checkRateHackTypes.push_back(v16);
    v17 = 0xe0; m_checkRateHackTypes.push_back(v17);
    v18 = 0xe1; m_checkRateHackTypes.push_back(v18);
    v19 = 0x84; m_vec7a30.push_back(v19);
    v20 = 0xca; m_vec7a30.push_back(v20);
    v21 = 0xcd; m_vec7a30.push_back(v21);
    v22 = 0xce; m_vec7a30.push_back(v22);
    v23 = 0xd0; m_vec7a30.push_back(v23);
    v24 = 0xd1; m_vec7a30.push_back(v24);
    v25 = 0xd7; m_vec7a30.push_back(v25);
    v26 = 0xd8; m_vec7a30.push_back(v26);
    v27 = 0xd9; m_vec7a30.push_back(v27);
    v28 = 0xda; m_vec7a30.push_back(v28);
    v29 = 0xdb; m_vec7a30.push_back(v29);
    v30 = 0xdc; m_vec7a30.push_back(v30);
    v31 = 0x138; m_vec7a30.push_back(v31);
    v32 = 0x139; m_vec7a30.push_back(v32);
    v33 = 0x193; m_vec7a30.push_back(v33);
    v34 = 0x19e; m_vec7a30.push_back(v34);
    v35 = 0x86; m_vec7a30.push_back(v35);
    v36 = 0xde; m_vec7a30.push_back(v36);
    v37 = 0x1a4; m_vec7a30.push_back(v37);
    v38 = 0x1a5; m_vec7a30.push_back(v38);
    v39 = 0x1a7; m_vec7a30.push_back(v39);
    v40 = 0xdf; m_vec7a30.push_back(v40);
    v41 = 0xe0; m_vec7a30.push_back(v41);
    v42 = 0xe1; m_vec7a30.push_back(v42);
}

void CHackAnalyzer::ReqDBSaveHackInfo(unsigned int uid, unsigned int type,
                                      unsigned short cnt, int a, int b)
{
    // SIG_SAVE_CLIENT_HACK_LOG（0x12 字节）：uid/type/cnt/a/b。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("HackAnalyzer.cpp", 0x249), true);
    **guard << 0x9a;
    **guard << (int)-1;
    SIG_SAVE_CLIENT_HACK_LOG* sig =
        guard->GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG>();
    sig->m_uid = uid;
    sig->m_type = type;
    sig->m_cnt = cnt;
    sig->m_a = a;
    sig->m_b = b;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// b28d-solo-CHackAnalyzer: AE（const_iterator + 独立 begin/end 局部 + `it->`
// 实参（ORIG 4 次 operator->）+ 死拷贝 info + 尾部显式 return；旧 N89/P197
// 判词作废）——ORIG 0x080f8080。
void CHackAnalyzer::reportHackInfo()
{
    if (m_hackInfoList.empty())
        return;
    // b28d-solo-CHackAnalyzer: ORIG 0x080f8080 用 const_iterator + 各自独立
    // temp/拷贝构造的 begin/end 局部（P13 缓存 end、P50 直构形），循环为
    // while；循环体先 `info = *it` 死拷贝（P171 照抄），实参改读 `it->`
    // （ORIG 4 次 operator-> 调用，operator* 仅 1 次）。
    std::vector<stHackInfo_t>::const_iterator it = m_hackInfoList.begin();
    std::vector<stHackInfo_t>::const_iterator e = m_hackInfoList.end();
    while (it != e)
    {
        stHackInfo_t info = *it;
        (void)info;
        ReqDBSaveHackInfo(it->m_uid, it->m_characNo, it->m_hackType,
                          it->m_value, 1);
        ++it;
    }
}

void CHackAnalyzer::resetHackInfo()
{
    m_hackInfoList.clear();
    m_flag04 = 0;
    m_baseHackType = 0;
    m_checkTypes.clear();
    m_field20 = 0;
    m_field24 = 0;
    m_field7a08 = 0;
}

// b28d-solo-CHackAnalyzer: AE（stHackInfo_t 平凡析构 + m_value unsigned +
// `m_flag04 | flag` 左操作数序；旧 N82/P254 判词作废）——ORIG 0x080f7fd2。
void CHackAnalyzer::_addHackInfo(ENUM_REPORT_4_HACK_FLAG flag,
                                 ENUM_HACKTYPE type, int value)
{
    // ORIG 0x080f7fd2（_addHackInfo.md）：取 acc_id / curCharacNo 作记录前两域，
    // {uid, hackNo, type(u16), value(int)} 落栈后 push_back 进 m_hackInfoList，
    // 随后 m_flag04 |= flag。
    stHackInfo_t info;
    info.m_uid = ((CUser*)m_baseHackType)->get_acc_id();
    info.m_characNo = (unsigned int)((CUser*)m_baseHackType)->getCurCharacNo();
    info.m_hackType = (unsigned short)(unsigned int)type;
    info.m_value = value;
    m_hackInfoList.push_back(info);

    m_flag04 = (unsigned char)(m_flag04 | flag);
}

int CHackAnalyzer::analyzeHackReport(CUser* user, PacketBuf* pkt)
{
    // ORIG 0x080f7a22（analyzeHackReport.md）：头 2 字节标志位驱动各检测分支；
    // 失败读包路径一律 return 0，成功路径 return 1；flags&0x100 时对 param_1
    // 直连其分析器加 0x32c 计数。
    // ORIG 局部 IO 字段统一位于 -0x4c 基址的 packed 缓冲：+0xd flags(u16)、
    // +0xf speedMs(u32)、+0x13 byteVal(u8)、+0x40 memVal(u32)；地址链
    // lea -0x4c;add off 形（帧 0x78）。
    struct __attribute__((packed))
    {
        unsigned char pad0[0xd];
        unsigned short flags;      // +0xd
        unsigned int speedMs;      // +0xf
        char byteVal;              // +0x13
        unsigned char pad1[0x40 - 0x14];
        unsigned int memVal;       // +0x40
    } buf;
    if (!stGetHackTypeOperator<unsigned short, 2>::getHackType(
            pkt, *(unsigned short*)(char*)&buf.pad0[0xd]))
        return 0;

    if (m_baseHackType == 0)
        return 1;

    if ((buf.flags & 0x1) != 0)
    {
        if (!pkt->get_int(*(unsigned int*)(char*)&buf.pad0[0xf]))
            return 0;
        _analyzeSpeedHack(buf.speedMs);
    }
    if ((buf.flags & 0x2) != 0)
    {
        _analyzeDifficultyHack('\0');
        ((CUser*)m_baseHackType)->DisConnSig((DISCONN_SIG)0x18, false, false);
    }
    if ((buf.flags & 0x8) != 0)
    {
        if (!pkt->get_int(*(unsigned int*)(char*)&buf.pad1[0x2c]))
            return 0;
        _analyzeMemoryHack(buf.memVal);
    }
    if ((buf.flags & 0x4) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE,
                     (ENUM_HACKTYPE)0x3c5, 0);
        ReqDBSaveHackUserPunish(((CUser*)m_baseHackType)->get_acc_id(), 0xa8);
        ((CUser*)m_baseHackType)->DisConnSig((DISCONN_SIG)0x18, false, false);
    }
    if ((buf.flags & 0x80) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MONSTER_DIE, (ENUM_HACKTYPE)0x3bd, 0);
    }
    if ((buf.flags & 0x10) != 0)
    {
        if (!pkt->get_byte(*(char*)&buf.byteVal))
            return 0;
        _addHackInfo(REPORT_4_HACK_FLAG_PAYLOAD_BYTE, (ENUM_HACKTYPE)0x3be,
                     (int)*(char*)&buf.byteVal);
    }
    if ((buf.flags & 0x20) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MISC, (ENUM_HACKTYPE)0x3c6, 0);
    }
    if ((buf.flags & 0x100) != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (ENUM_HACKTYPE)0x32c,
                                                  1, 0, 0);
    }
    return 1;
}

// b28d-solo-CHackAnalyzer: NEAR 封顶（N82 族：4.1 保留死局部槽）——ORIG 0x080f7cca。
void CHackAnalyzer::_analyzeSpeedHack(unsigned int ms)
{
    // ORIG 0x080f7cca（_analyzeSpeedHack.md）：ratio=(int)(100.0f*ms/elapsed)；
    // ratio<=0x45 或 >0x82 时记 0x3c1。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_SPEED))
        return;

    if (m_field24 == 0)
    {
        _reportIncurrectReport();
        return;
    }

    long long numMs;                                     // ORIG 声明在前（槽最深），赋值在后
    int elapsed = m_field24 - m_field20;                 // ORIG fildl（先物化 elapsed）
    numMs = ms;                                          // ORIG fildll（有符号直转，无 2^64 修正段）
    // ORIG flds 在 fdivrp 之后 ⇒ 左操作数为内存 float 变量（乘法左序）。
    int ratio = (int)(((double)numMs / (double)elapsed) *
                      _S_HACK_RATIO_COEF_100F);

    if (ratio <= 0x45 || ratio > 0x82)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_SPEED, (ENUM_HACKTYPE)0x3c1, ratio);
    }
}

// 判词 N（N82 族：4.1 死局部槽位策略）：ORIG numMs@-0x20/CW@-0x22/CW2@-0x24 vs
//   本工具链 numMs@-0x18/CW@-0x1a/CW2@-0x1c —— ORIG numMs 与 elapsed(-0x10) 间
//   存在 8 字节死槽(-0x18..-0x11)。本轮实测：①未使用 `long long`/`char[8]` 局部
//   恒被置于最深槽（pad@-0x28）并抬帧 0x38→0x48（反劣）；②16 字节 POD 局部
//   （packed/非 packed）同样落 -0x30 或多出结构拷贝。4.4 无「按声明序保留死槽」
//   能力 ⇒ 帧 0x38 两侧等齐、助记符序列全等、仅 3 处槽号差，N 封顶不追。

void CHackAnalyzer::_analyzeDifficultyHack(char level)
{
    // ORIG 0x080f7d82（_analyzeDifficultyHack.md）。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_DIFFICULTY))
        return;
    _addHackInfo(REPORT_4_HACK_FLAG_DIFFICULTY, (ENUM_HACKTYPE)0x3bf,
                 (int)level);
}

void CHackAnalyzer::_analyzeMemoryHack(unsigned int val)
{
    // ORIG 0x080f7dce（_analyzeMemoryHack.md）。
    // ORIG je/jne 双侧汇入统一 leave（P63 共享出口）：if(!=0&&val){body} 形
    // 消除 4.4 jmp/nop 中继（探针 f2 形）。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_MEMORY))
        return;
    if (val != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MEMORY, (ENUM_HACKTYPE)0x3bc, (int)val);
    }
}

// b28d-solo-CHackAnalyzer: N 封顶（N1+N18a，复核 b27）——ORIG 0x080f7e18。
void CHackAnalyzer::_analyzeEquipmentUpgradeHack(unsigned short const* table)
{
    // ORIG 0x080f7e18（_analyzeEquipmentUpgradeHack.md）：当前角色背包装备栏
    // 前 0x16 格 Inven_Item::GetUpgrade 低字节须逐一等于客户端上报表值。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE))
        return;

    const CInventory* inv = ((CUser*)m_baseHackType)->getCurCharacInvenR();
    for (int i = 0; i < 0x16; ++i)
    {
        // [hc 收] ORIG 0x080f7e18..：inv 为 CInventory 起始 +0x3d 步长 Inven_Item
        // 槽数组、+0x1c 为背包 ptr 槽区基址裸偏移跳入槽项；语义承载 = 槽序列
        // （§5.3 未建模子对象占位 / 序列化缓冲豁免，GetUpgrade 具名访问槽内字段）。
        // ORIG 地址链三步（imul;+0x10;+inv;+0xc）无槽中转 ⇒ this 内联表达式。
        // [b27 封顶 + b28d-solo 复测] 唯一残差 = 寻址链求值序：ORIG
        // i→imul→+0x10→+inv→+0xc（eax 单承载、两常量分列），4.4 恒 inv 先载 +
        // 0x1c 常量合并 + edx 双承载。本轮新增两形实测：「整数侧先算
        // (i*0x3d+0x10+inv)+0xc」被折回 lea+0x1c、「(char*)(i*0x3d+0x10)+inv+0xc」
        // 被折回原形 ⇒ 前端 fold/canonicalize 不可绕过，N1+N18a 族封顶。
        if (((const Inven_Item*)((char const*)inv + (i * 0x3d + 0x10) + 0xc))
                ->GetUpgrade() != table[i])
        {
            _addHackInfo(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE,
                         (ENUM_HACKTYPE)0x3c3, 0);
            return;
        }
    }
}

// b28d-solo-CHackAnalyzer: AE（窗口检查改正向包裹 `if (<= 0xa) {...}`，
// ORIG 0x080f7eb6 双出口岛形逐条对齐；旧 P223 边界判词作废）。
void CHackAnalyzer::_analyzeMonsterDieHack()
{
    // ORIG 0x080f7eb6（_analyzeMonsterDieHack.md）：采集窗口 <11 秒且 kill 记录
    // 全为 0xffff 填充时记 0x3c0。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_MONSTER_DIE))
        return;

    // b28d-solo-CHackAnalyzer: 窗口检查改正向包裹（P222/P59 补强）——ORIG
    // 0x080f7eec `jg 0x119(leave)` 直落共享出口（无岛），而 checkCollectedHack
    // 的早退走 0x118 单个 nop 岛；早返形会让 4.4 给两个出口各发 [nop;jmp]
    // （87→89）。包裹后窗口出口直跳 leave、早退保留单岛，逐条对齐。
    if (m_field24 - m_field20 <= 0xa)
    {
        std::vector<int>::const_iterator it = m_checkTypes.begin();
        std::vector<int>::const_iterator e = m_checkTypes.end();
        while (it != e)
        {
            if (*it != 0xffff)
                break;      // ORIG body 形：setne+test+jne 退出（等价 && 短路）
            ++it;
        }
        if (it == e)
        {
            _addHackInfo(REPORT_4_HACK_FLAG_MONSTER_DIE, (ENUM_HACKTYPE)0x3c0,
                         m_field24 - m_field20);
        }
    }
}

void CHackAnalyzer::analyzeMonsterDieHack()
{
    // ORIG W 0x080f7cb6（analyzeMonsterDieHack.md）：纯转发。
    _analyzeMonsterDieHack();
}

void CHackAnalyzer::_reportIncurrectReport()
{
    // ORIG W 0x080f8032（_reportIncurrectReport.md）。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_INCURRECT))
        return;
    _addHackInfo(REPORT_4_HACK_FLAG_INCURRECT, (ENUM_HACKTYPE)0x3c4,
                 ((CUser*)m_baseHackType)->get_state());
}

void CHackAnalyzer::ValidateFixMobDiePos(CUser* user, unsigned short mobId,
                                         unsigned short x, unsigned short y)
{
    // ORIG T 0x080f908c（ValidateFixMobDiePos.md）：查 FixMobInfo 白名单表；
    // mobId 命中而坐标出框（或 0xd6ed 特例坐标不等于框边界 xMin/xMax）时
    // addServerHackCnt(user, 0xd2, 1, 0, 0)。
    // 静态表（ORIG nm d @0x0936f9c0 size 0x84 / FixMobInfoSize @0x0936fa44
    // =11；内容 objdump -s .data 实测，第 11 项全零不参与比较循环）。
    struct FixMobInfoRec
    {
        unsigned int m_mid;       // +0x00
        unsigned short m_xMin;    // +0x04
        unsigned short m_xMax;    // +0x06
        unsigned short m_yMin;    // +0x08
        unsigned short m_yMax;    // +0x0a
    };
    static FixMobInfoRec FixMobInfo[] = {
        {0xbba,  0x468, 0x468,  0xed,  0xed },
        {0xd6ed, 0x80,  0x337,  0x9d,  0x139},
        {0x2f0,  0xb,   0xb,    0x82,  0x15e},
        {0x2ef,  0x378, 0x378,  0x82,  0x15e},
        {0xdb29, 0x4f9, 0x4f9,  0xff,  0xff },
        {0xf010, 0xffee, 0xffee, 0x120, 0x120},
        {0xf108, 0x57,  0x57,   0x175, 0x175},
        {0xef9c, 0x384, 0x384,  0xf0,  0xf0 },
        {0x3f2,  0x377, 0x377,  0xca,  0xca },
        {0xdcbb, 0x552, 0x552,  0xfb,  0xfb },
        {0,      0,     0,      0,     0    },
    };
    static unsigned short FixMobInfoSize = 11;   // ORIG 0x936fa44 movzwl 零扩展（unsigned）

    for (int i = 0; i < FixMobInfoSize - 1; ++i)
    {
        if (mobId == FixMobInfo[i].m_mid)
        {
            if (x < FixMobInfo[i].m_xMin ||
                FixMobInfo[i].m_xMax < x ||
                y < FixMobInfo[i].m_yMin ||
                FixMobInfo[i].m_yMax < y)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (ENUM_HACKTYPE)0xd2, 1, 0, 0);
                return;
            }
            if (mobId != 0xd6ed)
                return;
            if (FixMobInfo[i].m_xMin == x)
                return;
            if (FixMobInfo[i].m_xMax == x)
                return;
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0xd2, 1, 0, 0);
            return;
        }
    }
}

void CHackAnalyzer::onUpdateHackCount(CUser* user, ENUM_HACKTYPE type, int a,
                                      unsigned int b, unsigned int c)
{
    user->onUpdateHackCount(type, a, b, c);
}

// CParty 子对象视图（b28d-solo-CHackAnalyzer）：ORIG addServerHackCnt 0x080f8db5
// 对 party+0xcac 直读指针（`mov 0xcac(%eax),%eax`）、对 party+0xb24 取内嵌子对象
// 地址（`add $0xb24,%eax; call get_dungeon_diff`）。按 §8.3 未建模子对象具名化，
// 避免 char* 加法把 disp 折成 add 指令（P82/P301）。
namespace {
struct CPartyHackView
{
    char m_pad0[0xcac];
    CDungeon* m_pDungeon;                    // +0xcac（CDungeon*；0xb24 内嵌
                                             // CBattle_Field 仍用真实类型 cast 取址）
};
}

// b28d-solo-CHackAnalyzer: AE（CParty 视图 + 嵌套 if + 三目 lvDiff + 去
// (int)count cast + 去 dungeon/memberCount 局部；旧 N82/P183 判词作废）
// ——ORIG 0x080f8c7e。
void CHackAnalyzer::addServerHackCnt(CUser* user, ENUM_HACKTYPE type,
                                     unsigned int count, unsigned int a,
                                     unsigned int b)
{
    // ORIG：直接用形参比较，不落局部（保持指令形态）。
    if ((int)type < 0 || 0xfa1 < (int)type)
    {
        // ORIG：SetETC 返回值直通实参（无中间变量物化）。
        addServerHackCnt(user, (ENUM_HACKTYPE)0x34e, count,
                         (unsigned int)user->SetETC(0x34e, (int)getBaseHackType()),
                         0u);
        setBaseHackType((ENUM_HACKTYPE)0);
        return;
    }

    // ORIG：CheckExceptionCnt 返回真（异常额度内）→ 跳过统计；假 → 继续
    // 并在尾部推进计数器（见 addServerHackCnt.md 汇编 +0x93/+0x98）。
    if (CheckExceptionCnt(type, (int)count))
        return;
    if (Secu_AccountHacking::checkAccountHackingHackType(type) &&
        HACK_SLOT_B(type) != 0)
        return;


    CParty* party = (CParty*)user->GetParty();
    // b28d-solo-CHackAnalyzer: ORIG 0x080f8d66 为嵌套 if（第二条件发
    // `cmp $5; sete; test; je`，探针 /tmp/p_and.cpp f2 实证 4.4 唯一同形；
    // `&&` 形 f1 恒产 mov$1/mov$0 岛）。
    if (party != 0)
    {
        if (user->get_state() == 5)
        {
            // ORIG：get_member_count 结果直通实参（无 memberCount 局部）。
            if (!CheckHackRateByPartyCount(type, party->get_member_count()))
                return;
            // [hc 收] ORIG：party+0xcac 为 CDungeon*（指针）、party+0xb24 为
            // 内嵌 CBattle_Field（§8.3 未建模子对象具名视图，P82/P301：视图
            // 成员访问保 disp 折叠 `mov 0xcac(%eax),%eax`；char* 加法会退化成
            // add $0xcac）。ORIG dungeon 不落槽、两臂各重读一次（P48/P183）。
            if (((CPartyHackView*)party)->m_pDungeon != 0)
            {
                // ORIG 三目形（test/js 直跳，P47；if/else 赋值会发 shr$31 物化）。
                int lvDiff =
                    (((CUserCharacInfo*)user)->get_charac_level() -
                         ((CPartyHackView*)party)->m_pDungeon->get_standard_level() <
                     0)
                        ? 0
                        : ((CUserCharacInfo*)user)->get_charac_level() -
                              ((CPartyHackView*)party)->m_pDungeon
                                  ->get_standard_level();
                if (!CheckHackRateByDifficult(
                        type, lvDiff,
                        ((CBattle_Field*)((char*)party + 0xb24))->get_dungeon_diff()))
                    return;
            }
        }
    }

    if (((int)type == 0x25b) || ((int)type == 0x25c) ||
        ((int)type == 0x25d) || ((int)type == 0x25e) ||
        ((int)type == 0x25f) || ((int)type == 0x260) ||
        ((int)type == 0x325) || ((int)type == 0x6f))
        return;

    {
        // b28d-solo-CHackAnalyzer: 勿加 (int)count 冗余 cast——ORIG 0x080f8eb9
        // `movl 0x8(%eax,%edx,4),%eax; addl 0x14(%ebp),%eax; mov %eax,%edx` 为
        // int 字段 += unsigned 形参（异型 ⇒ add mem+中转，P274 反向；探针
        // /tmp/p_add.cpp f2 逐条同形；加 cast 变同型恒产 lea 三操作数 f1）。
        HACK_CNT_A(type) += count;
        HACK_CNT_B(type) += count;
        onUpdateHackCount(user, type, count, a, b);
    }
}

void CHackAnalyzer::ReqDBSaveHackUserPunish(unsigned int accId,
                                            unsigned int punish)
{
    // ORIG T 0x080f906a（ReqDBSaveHackUserPunish.md）：直发 DB 请求（ip=NULL）。
    DB_ReqSaveHackUserPunish::makeRequest(accId, punish, (char const*)0);
}

}  // namespace WongWork

// ============================================================================
// Secu_AccountHacking（全局作用域；声明见 CHackAnalyzer.h）
// ============================================================================

Secu_AccountHacking::Secu_AccountHacking() {}
// ORIG C1/D1 @0x08695aa4/aaa 均为空体（push ebp/mov/pop ebp/ret）。

Secu_AccountHacking::~Secu_AccountHacking() {}

void Secu_AccountHacking::init(CUser* user)
{
    // ORIG T 0x08278a98（init.md）：+0x10 存 user 后 resetInfo。
    m_userP = user;
    resetInfo();
}

void Secu_AccountHacking::resetInfo()
{
    // ORIG T 0x08278ab4（resetInfo.md）：memset(this,0,6)；+8/+0xc 清零。
    memset(&m_flag[0], 0, 6);
    m_sendCnt = 0;
    m_loginTime = 0;
}

void Secu_AccountHacking::setLoginTime()
{
    // ORIG T 0x08279020（setLoginTime.md）：+0xc = s_systemTime_.getCurSec()。
    m_loginTime = GlobalData::s_systemTime_.getCurSec();
}

WongWork::ENUM_HACKTYPE Secu_AccountHacking::getMatchingHacktype(
    AccountHackingType::T t)
{
    // ORIG W 0x0827912a（getMatchingHacktype.md）：线性 cmpl/jne 映射链。
    if (t == AccountHackingType::GOLD_0) return (WongWork::ENUM_HACKTYPE)0x2c6;
    if (t == AccountHackingType::GOLD_1) return (WongWork::ENUM_HACKTYPE)0x2c7;
    if (t == AccountHackingType::GOLD_2) return (WongWork::ENUM_HACKTYPE)0x2c8;
    if (t == AccountHackingType::GOLD_3) return (WongWork::ENUM_HACKTYPE)0x2c9;
    if (t == AccountHackingType::AVATAR_COUNT) return (WongWork::ENUM_HACKTYPE)0x2ca;
    if (t == AccountHackingType::AVATAR_TIME) return (WongWork::ENUM_HACKTYPE)0x2cb;
    return (WongWork::ENUM_HACKTYPE)0xfa1;
}

bool Secu_AccountHacking::isAvatar(unsigned int itemIdx)
{
    // ORIG T 0x08278aec（isAvatar.md）：find_item 命中且虚槽+0x10
    // IsAvatarItem（CItem vtable 第 4 虚，P5 实证）返回真。
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item != 0 && item->IsAvatarItem())
        return true;
    return false;
}

// b28d-solo-CHackAnalyzer: NEAR 封顶（P75/N76 跨版本装载序）——ORIG 0x08278b48；
// 循环体 m_entries[i] 恒 base-first（六形实测），仅两 load 序差、指令数全等。
void Secu_AccountHacking::sendMail(char const* name, MSG_CHECK_MAIL_RECEIVER* msg)
{
    // §4.3 裸偏移清零 —— 载荷具名视图（本 TU 私有，不动冻结头）：
    // MSG_CHECK_MAIL_RECEIVER（CGameManager.h:103）的 m_pad 是 set<TYPE>() 于
    // +4 memcpy 写入的字节数据区，对应消息体（MSG_MAILBOX_SEND 等）在本仓仅有
    // 前置声明、权威定义属冻结 PacketTypes.h ⇒ 按 ORIG T 0x08278b48
    // （docs/class_func_reports/Secu_AccountHacking/sendMail.md 汇编段）实证
    // 偏移建 pack(1) 视图：
    //   set<0> 分支：@+0x049 `mov 0x32(%eax),%eax`（金额）/
    //                @+0x081 `mov 0x3d(%eax),%eax`（avatar 物品号）
    //   set<2> 分支：@+0x0e3 `mov 0x33(%eax),%eax`（金额）/
    //                @+0x16f `movzbl 0x37(%eax),%eax`（条目数）/
    //                条目物品号 = base + i*0xb + 0x3b（ Ghidra C 段同形）
#pragma pack(push, 1)
    struct CheckMailReceiverSendView
    {
        int m_checkType;              // +0x00（set<> 写入的 TYPE）
        char m_msgHead[0x32 - 4];     // +0x04..+0x31（本函数不读的消息前缀）
        unsigned int m_money;         // +0x32
        char m_pad36[0x3d - 0x36];    // +0x36..+0x3c
        unsigned int m_avatarItemIdx; // +0x3d..+0x40
    };
    struct CheckMailReceiverListEntry
    {
        unsigned int m_itemIdx;       // 条目 +0x00（ORIG 步长 0xb）
        char m_rest[0xb - 4];         // +0x04..+0x0a（本函数不读）
    };
    struct CheckMailReceiverListView
    {
        int m_checkType;              // +0x00
        char m_msgHead[0x33 - 4];     // +0x04..+0x32（本函数不读的消息前缀）
        unsigned int m_money;         // +0x33
        unsigned char m_entryCount;   // +0x37
        char m_pad38[0x3b - 0x38];    // +0x38..+0x3a
        CheckMailReceiverListEntry m_entries[1];   // +0x3b
    };
#pragma pack(pop)
    static_assert(sizeof(CheckMailReceiverSendView) == 0x41,
                  "CheckMailReceiverSendView ABI drift");
    static_assert(offsetof(CheckMailReceiverListView, m_money) == 0x33 &&
                  offsetof(CheckMailReceiverListView, m_entryCount) == 0x37 &&
                  offsetof(CheckMailReceiverListView, m_entries) == 0x3b,
                  "CheckMailReceiverListView ABI drift");
    static_assert(sizeof(CheckMailReceiverListEntry) == 0xb,
                  "CheckMailReceiverListEntry stride drift");

    // ORIG T 0x08278b48（sendMail.md）：检查类型 0（金币+单 avatar）/2
    // （金币+avatar 列表）时取出金额与 avatar 物品号逐一 sendGold/sendAvatar。
    // MSG_CHECK_MAIL_RECEIVER 各字段偏移（0x33/0x37/0x3b/0x3d/0x32）来自报告
    // 汇编；该类型为 set<T> 写入的字节数据区，具名化见上方视图。
    if (((CUser*)m_userP)->getCurCharacInvenR() == 0)
        return;

    if (*(int*)msg == 0)
    {
        // P0：ORIG getCurCharacInvenR 结果不经栈槽往返（`call; mov %eax,(%esp);
        // call get_money`）⇒ 内联实参，具名指针局部多 2 条 spill。
        sendGold(name, 0, ((CheckMailReceiverSendView*)msg)->m_money,
                 ((CUser*)m_userP)->getCurCharacInvenR()->get_money(),
                 2, (char*)0);
        if (isAvatar(((CheckMailReceiverSendView*)msg)->m_avatarItemIdx))
            sendAvatar(name, 0);
    }
    else if (*(int*)msg == 2)
    {
        sendGold(name, 0, ((CheckMailReceiverListView*)msg)->m_money,
                 ((CUser*)m_userP)->getCurCharacInvenR()->get_money(),
                 2, (char*)0);
        for (int i = 0;
             i < (int)((CheckMailReceiverListView*)msg)->m_entryCount; ++i)
        {
            // b28d-solo-CHackAnalyzer: NEAR 封顶（P75/N76 跨版本装载序）。ORIG
            // 0x08278c69 循环体 `mov -0xc(%ebp),%edx; mov 0x10(%ebp),%ecx`（索引先、
            // msg 后）；本工具链对 m_entries[i] 恒 base-first。本轮实测新增两形
            // （交换下标 i[arr]、索引 (unsigned int)i 中转）仍 base-first，六形
            // 均不可翻；两侧指令数/助记符序列全等，仅两 load 序差，语义等价。
            if (isAvatar(((CheckMailReceiverListView*)msg)->m_entries[i].m_itemIdx))
                sendAvatar(name, 0);
        }
    }
}

// b28d-solo-CHackAnalyzer: AE（正极性 || 守卫 + name!=0 块序 + 真语义 bug
// 修正（addServerHackCnt 实参 (a,money)、GoldTracking 实参 (trackType,a,ip,
// money,fatigue)）+ 内联 fatigue + 无 cast 除法 + 尾部 return 岛形；旧 N82
// 判词作废）——ORIG 0x08278cce。
void Secu_AccountHacking::sendGold(char const* name, unsigned int a,
                                   unsigned int money, unsigned int b,
                                   unsigned char trackType, char* ip)
{
    // ORIG T 0x08278cce（sendGold.md）：同名角色或金额<=500 直返；否则按 name
    // 是否为空分别走 analyze 加计数（附 Hades 金币追踪）/ DB 查询；
    // fatigue<=100 时按比例分档 setPatten(0/1/2)。
    // b28d-solo-CHackAnalyzer: 守卫写正极性 `||`（ORIG 0x08278cee 为
    // mov$1/mov$0 岛形；`!(A)&&B` 反形在 4.4 恒直跳，探针 /tmp/p_or.cpp
    // g1 逐条同形）。
    if (isSameAccountCharacter(name) || money <= 500)
        return;

    // ORIG 0x08278d0f：`cmpl $0,name; je <name==0 块>` ⇒ then 块是 name!=0
    // （P47 块序：先写 makeRequest 分支）。
    if (name != (char const*)0)
    {
        DB_SelectCharacMid::makeRequest(
            (int)((CUser*)m_userP)->GetUID(), 0, name, 0x2ce, money, 0);
    }
    else
    {
        // ORIG 0x08278d6a 实参序 (user, 0x2ce, 1, a, money)（第 4/5 参 = a/money，
        // 旧重建误写 (money, 0)，真语义 bug 已修）。
        ((CUser*)m_userP)->getHackAnalyzer()->addServerHackCnt(
            (CUser*)m_userP, (WongWork::ENUM_HACKTYPE)0x2ce, 1, a, money);
        // ORIG 0x08278db7 实参序 (trackType, a, ip, money, fatigue)（第 2/4 参
        // = a/money，旧重建误写 (money, ..., a)，真语义 bug 已修）；fatigue
        // 内联实参（无具名局部，ORIG 直通 esi）。
        ((XNuclear::CHades*)((CUser*)m_userP)->getHades())
            ->GoldTracking(trackType, a, ip, money,
                           ((CUser*)m_userP)->getCurCharacUsedFatigue());
    }

    // ORIG 0x08278ddc：`&&` 岛形（探针 /tmp/p_and2.cpp h1）；fatigue 为
    // unsigned short ⇒ 16 位比较 `cmp $0x64,%ax`（勿加 (unsigned int) cast）。
    if (m_userP != 0 &&
        ((CUser*)m_userP)->getCurCharacUsedFatigue() <= 100)
    {
        // ORIG 0x08278e0e：两次 fildll 复用同一 8 字节临时槽（-0x28），无
        // unsigned long long 具名局部、无 2^64 修正段 ⇒ 直接 (double)uint 形。
        int ratio = (int)(((double)money / (double)b) *
                          _S_HACK_RATIO_COEF_100F);
        // ORIG 0x08278cce：先无条件 setPatten(GOLD_3)，再按比例分档
        // ratio>0x45→GOLD_0，0x32..0x45→GOLD_1，0x1e..0x31→GOLD_2。
        setPatten(AccountHackingType::GOLD_3, name, a, money);
        if (ratio > 0x45)
            setPatten(AccountHackingType::GOLD_0, name, a, money);
        else if (ratio > 0x31)
            setPatten(AccountHackingType::GOLD_1, name, a, money);
        else if (ratio > 0x1d)
            setPatten(AccountHackingType::GOLD_2, name, a, money);
    }
    // ORIG 尾岛 [nop;jmp]×2 + [nop]（0x23a/0x23b、0x23d/0x23e、0x240）：
    // 显式尾部 return 令 4.4 保留各早退中继块（P63/P285；缺此 return 时
    // 4.4 jump-thread 合并成单 nop，ndiff=2）。
    return;
}

bool Secu_AccountHacking::isSameAccountCharacter(char const* name)
{
    // ORIG T 0x08279182（isSameAccountCharacter.md）：绑定用户的角色列表中
    // strncmp(m_name, name, 0x1e)==0 即命中（列表元素 _Charac_info +0x04 起）。
    if (name == 0)
        return false;

    std::vector<_Charac_info>::const_iterator it =
        ((CUser*)m_userP)->m_characList.begin();
    std::vector<_Charac_info>::const_iterator e =
        ((CUser*)m_userP)->m_characList.end();
    while (it != e)
    {
        if (strncmp(it->m_name, name, 0x1e) == 0)
            return true;
        ++it;
    }
    return false;
}

void Secu_AccountHacking::setPatten(AccountHackingType::T t, char const* name,
                                    unsigned int a, unsigned int b)
{
    // ORIG：a 缓存到 -0x10、b 缓存到 -0xc，但 cmpl $0x0 直测参数槽 0x14
    // （aSpill 声明保留缓存，比较直读形参 a）。
    // ORIG T 0x0827903c（setPatten.md）：未标记且已绑定用户时置位，随后按 a
    // 选择 DB 直查或经分析器加计数。
    if (m_flag[(int)t])
        return;
    if (m_userP == 0)
        return;

    m_flag[(int)t] = 1;

    // ORIG +0x66/+0x6f：两实参先早溢出到 -0x10/-0xc 栈槽（gcc callee 后续
    // 仍需引用时选择 caller-saved 缓存；此处以局部副本驱动同型）
    unsigned int aSpill = a;
    unsigned int bSpill = b;

    if (a == 0)
    {
        // ORIG：getMatchingHacktype 缓存 ebx 后再求 GetUID（P24 实参序），
        // 不引入具名中间变量。
        DB_SelectCharacMid::makeRequest((int)((CUser*)m_userP)->GetUID(), 0,
                                        name,
                                        (unsigned int)getMatchingHacktype(t),
                                        bSpill, 0);
    }
    else
    {
        ((CUser*)m_userP)->getHackAnalyzer()->addServerHackCnt(
            (CUser*)m_userP, getMatchingHacktype(t), 1, aSpill, bSpill);
    }
}

bool Secu_AccountHacking::isGoldPatten(AccountHackingType::T t)
{
    // ORIG W 0x08279258（isGoldPatten.md）：0..3 判金伪型。
    if (t == AccountHackingType::GOLD_0 ||
        t == AccountHackingType::GOLD_1 ||
        t == AccountHackingType::GOLD_2 ||
        t == AccountHackingType::GOLD_3)
        return true;
    return false;
}

bool Secu_AccountHacking::isAvatarPatten(AccountHackingType::T t)
{
    // ORIG W 0x08279282（isAvatarPatten.md）：4/5 判 avatar 伪型。
    if (t == AccountHackingType::AVATAR_COUNT ||
        t == AccountHackingType::AVATAR_TIME)
        return true;
    return false;
}

bool Secu_AccountHacking::isDetectionGoldPatten()
{
    // ORIG W 0x082792a0（isDetectionGoldPatten.md）：前四标志任一置位；
    // 线性测试→共享 true 块（P45），用 || 短路链逐字同构。
    return m_flag[0] || m_flag[1] || m_flag[2] || m_flag[3];
}

void Secu_AccountHacking::sendAvatar(char const* name, unsigned int a)
{
    // ORIG T 0x08278f16（sendAvatar.md）：同名直返；fatigue<=100 时自增计数，
    // 检测过金币伪型→AVATAR_COUNT；计数>=8 且距登录<600 秒→AVATAR_TIME。
    if (isSameAccountCharacter(name))
        return;

    // 条件物化形（ORIG @0x08278f36..0x08278f62 / 0x08278faf..0x08278fe9）：
    // 正条件 if 包裹（非 if(!x)return）令 4.4 产 mov$1/mov$0 汇聚 + test/je
    // 失败岛（探针 /tmp/l12_probe.cpp f3 实证）；getCurSec 内联无具名局部（P0）。
    if ((m_userP != 0) &&
        ((unsigned int)((CUser*)m_userP)->getCurCharacUsedFatigue() <= 100))
    {
        m_sendCnt = m_sendCnt + 1;

        if (isDetectionGoldPatten())
            setPatten(AccountHackingType::AVATAR_COUNT, name, a, 0);

        if ((m_sendCnt >= 8) &&
            ((unsigned int)(GlobalData::s_systemTime_.getCurSec() - m_loginTime) <=
             599))
        {
            setPatten(AccountHackingType::AVATAR_TIME, name, a, 0);
        }
        // b28c-lane12：ORIG 尾有三个独立 `nop; jmp <leave>` 中继岛（+0x100/
        // +0x103/+0x106），4.4 对共享出口合并；显式出口 return 使各出口独立
        // 发射（P63 散布 return 中继块）。
        return;
    }
    return;
}

bool Secu_AccountHacking::checkAccountHackingHackType(WongWork::ENUM_HACKTYPE type)
{
    // ORIG T 0x08278a48（0x4f 字节）：线性 cmpl/je(jne) 判定，
    // hacktype ∈ {0x2c6..0x2cb, 0x2cd} 返回 true，否则 false。（P45）
    if (type == 0x2c6 || type == 0x2c7 || type == 0x2c8 ||
        type == 0x2c9 || type == 0x2ca || type == 0x2cb ||
        type == 0x2cd)
        return true;
    return false;
}

// ============================================================================
// DB_SelectCharacMid / DB_ReqSaveHackUserPunish（本批唯一 owner；报告在手）
// ============================================================================

void DB_SelectCharacMid::makeRequest(int serverGroup, unsigned int field2,
                                     char const* name, unsigned int field4,
                                     unsigned int field5, unsigned int field6)
{
    // ORIG T 0x0844676c（DB_SelectCharacMid/makeRequest.md）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xae42), true);
    **guard << 0x2b9;
    **guard << serverGroup;
    SIG_CHECK_CHAR_MID* sig = guard->GetInBuffer<SIG_CHECK_CHAR_MID>();
    sig->m_field00 = field2;
    sig->m_field04 = field4;
    sig->m_field08 = field5;
    sig->m_field0c = field6;
    strncpy(sig->m_name, name, 0x1e);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ==== 模板实例（ORIG W 0x080f9b82：get_short 转发形态）====
template <typename T, int N>
bool stGetHackTypeOperator<T, N>::getHackType(PacketBuf* pkt, T& out)
{
    return pkt->get_short(out);
}

template struct stGetHackTypeOperator<unsigned short, 2>;
