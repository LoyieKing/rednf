// b28d-50：ORIG 0x085e2ffc 发射序 = 声明序（标量 +4..+14 → +0x18/+0x20 两处
// 8 字节**整体拷贝** → +0x28/+0x2c → vector +0x30 → +0x3c）；旧「vector 只写
// meminit、标量留函数体」使 4.4 先发 vector C1（P239）。全部移入初始化列表且
// 按声明序书写，+0x18/+0x20 经 union 的 pair 视图整体拷贝（P246/P247）。
CMission::CMission(const MissionScript& script)
    : m_conditionType(script.m_kind)              // script+0x78（byte）
    , m_kind(script.m_field08)
    , m_missionIndex(script.m_field0c)
    , m_field10(script.m_field00)
    , m_prevMissionIndex(script.m_prevMissionIndex)
    // [b28d-70] pair 成员化后 C1 仍 DIFF 61/59（残余 2×add-vs-lea，N18a，零回归）。
    // ORIG C1 0x085e304b/0x085e305f 两处 8 字节逐字段内联拷贝：成员与 MissionScript
    // +0x10/+0x18 同为 std::pair<int,int>（pair 拷贝构造对 <int,int> trivial ⇒ 内联，
    // 无 call；get_WithinMissionIndex 0x085e2f00 的 pair 消费实证该真实类型，b28d-70）。
    // [2026-09-10 round5 复核] 残余精确定位：ORIG `mov 0x8(%ebp),%eax; lea 0x18(%eax),%ecx;
    //   …; mov %eax,(%ecx); mov %edx,0x4(%ecx)`（dest 缓存 %ecx 的块拷贝，P246 判据）
    //   vs 我方 4.4 恒 `mov %eax,0x18(%ecx)` disp 直存（2 处各少 1 条 lea）。
    //   唯一已知翻法 = P246 把 MissionScript::m_grade/m_rate 改 POD union 双视角
    //   （共享头 + 两 TU 联判），收益 2 条 ⇒ 本轮不做，按 N18a 封顶。
    , m_rankRange(script.m_grade)
    , m_rateRange(script.m_rate)
    , m_grade(script.m_field20)
    , m_expRate(script.m_expRate)
    , m_rewardItems(script.m_rewardItems)
    , m_job(script.m_job)
{
}