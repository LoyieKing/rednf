// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + docs/class_func_reports/PerfMgr/{README,inc,get,clear}.md
#ifndef DNF_GAME_PERFMGR_H_
#define DNF_GAME_PERFMGR_H_

// ORIG 符号：
//   T 0x085c3c44 _ZN7PerfMgr3incENS_8TASK_IDXE
//   T 0x085c3c5c _ZN7PerfMgr3getENS_8TASK_IDXE
//   T 0x085c3c6a _ZN7PerfMgr5clearEv
// inc/get 均为 this + idx*4 的 int 计数读写（报告汇编）；
// clear 循环 `cmpl $0x3,-0x4(%ebp); setle`（i<=3）⇒ 计数数组恰 4 个 int（硬证据）。
// TASK_IDX 枚举成员名无 ORIG 证据【推断-占位】，值域 0..3 由 clear 循环与 inc/get 索引证实。
class PerfMgr
{
public:
    enum TASK_IDX
    {
        TASK_IDX_BEGIN = 0,   // 0x0
        TASK_IDX_1,           // 0x1【推断】名称无证据
        TASK_IDX_2,           // 0x2【推断】名称无证据
        TASK_IDX_3,           // 0x3【推断】名称无证据
        TASK_IDX_END          // 计数槽数 4（clear cmpl $0x3/setle 硬证据）
    };

    void inc(TASK_IDX);   // ORIG T 0x085c3c44：m_count[idx] += 1
    int get(TASK_IDX);    // ORIG T 0x085c3c5c：返回 m_count[idx]（undefined4→int【推断】）
    void clear();         // ORIG T 0x085c3c6a：m_count[0..3] 清零

private:
    int m_count[TASK_IDX_END];
};

#endif  // DNF_GAME_PERFMGR_H_
