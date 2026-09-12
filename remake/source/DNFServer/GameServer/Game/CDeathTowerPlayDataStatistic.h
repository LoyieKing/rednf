// df_game_r Game/ CDeathTowerPlayDataStatistic（死亡之塔游玩数据统计上报）。
// ORIG 符号：
//   _ZN28CDeathTowerPlayDataStatistic22SendDeathTowerPlayInfoEicPK30stDeathTowerRecordMemberInfo_tjj
//   T 0x0860f45c
// 报告：docs/class_func_reports/CDeathTowerPlayDataStatistic/SendDeathTowerPlayInfo.md
// 无成员、无用户 ctor/dtor（ORIG 无 C1/D1 符号，trivial）。
#ifndef DNF_GAME_CDEATHTOWERPLAYDATASTATISTIC_H_
#define DNF_GAME_CDEATHTOWERPLAYDATASTATISTIC_H_

class stDeathTowerRecordMemberInfo_t;

class CDeathTowerPlayDataStatistic
{
public:
    // ORIG 0x0860f45c T：构造并发送 Job 包（0xc3d, 0x16，level==0 时仅 log 错误
    // 不发送）与 Party 包（0xc3f, 0x10），恒返回 1。
    // [R11fixE7 2026-09-12] int→bool：ORIG prett 串 `bool CDeathTowerPlayDataStatistic::
    // SendDeathTowerPlayInfo(int, char, const stDeathTowerRecordMemberInfo_t*, unsigned int,
    // unsigned int)`（raw @0x8ce1040，P174）⇒ 返回 bool；mangled 不变、调用点
    // _beginTowerClearProcess 忽略返回值（ORIG 0x8467684 亦不消费）。
    bool SendDeathTowerPlayInfo(int, char, stDeathTowerRecordMemberInfo_t const*, unsigned int, unsigned int);
};

#endif  // DNF_GAME_CDEATHTOWERPLAYDATASTATISTIC_H_
