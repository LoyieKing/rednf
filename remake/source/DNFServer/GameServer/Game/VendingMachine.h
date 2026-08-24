#ifndef DNF_GAME_VENDING_MACHINE_H_
#define DNF_GAME_VENDING_MACHINE_H_

// ============================================================================
// df_game_r 还原 —— CVendingMachineStatistic / TimerVendingMachineItemNotify
// 权威声明（唯一）。
//
// 依据 docs/class_func_reports/CVendingMachineStatistic/（README + addOutputItem +
// updateDataIntoDB）与 TimerVendingMachineItemNotify/（README + registTimer +
// dispatch_sig），并以 ORIG nm/objdump（df_game_r）逐指令核对：
//
//   CVendingMachineStatistic（ORIG 尺寸 0x28 字节；c6root 4.4.7/m32 实测）：
//     +0x00 std::vector<unsigned long> m_usedItems   （ctor 构造，_init 填 3 项）
//     +0x0c unsigned int             m_useCount      （incUseCount 自增 +0x0c）
//     +0x10 std::map<unsigned long,unsigned int> m_output（_Rb_tree=0x18，addOutputItem 累加）
//
//   GetInstanceVendingMachineStatistic（ORIG 0x0860f8ad）为函数内静态单例：
//     __cxa_guard_acquire 惰性构造 + __cxa_atexit 析构，实例位于 ORIG BSS 0x9473380。
//
//   TimerVendingMachineItemNotify::registTimer(int,unsigned long,unsigned int,
//     unsigned int)（ORIG 0x08638074）：static 方法（函数体从 ebp+8 读参数、无 this），
//     内部仅转调 G_TimerQueue()->InsertTimer(OBJ_TYPE_0, uid, TIMER_MESSAGE(0xa),
//     sec=param_4, param_2=param_2, param_3=param_3)。
//     注意 ORIG 实参次序：InsertTimer(pTVar1,0,param_1,10,param_4,param_2,param_3)，
//     即 sec=param_4（+0x14），param2=param_2（+0x0c），param3=param_3（+0x10）。
//
// 消费方（Dispatcher_UseVendingMachine_impl.cpp）原先以 C 包装前置声明引用，本头
// 落地后应改为 include 并直调真实 C++ 成员（见各方法注释）。
// ============================================================================

#include <map>
#include <vector>

class CVendingMachineStatistic
{
public:
    CVendingMachineStatistic();   // 0x0861062c（C1/C2 同址）
    ~CVendingMachineStatistic();  // 0x08610694（D1/D2 同址）

    // 0x0822d714：自增 +0x0c 使用次数。
    void incUseCount();

    // 0x0860f5a4：若 itemIdx 已存在于 m_output 则累加 count，否则插入。
    void addOutputItem(unsigned long itemIdx, unsigned int count);

    // 0x0860f6de：把 m_output 打包成 Stream（SIG=0x163, count=m_useCount）
    // 经 CStreamGuard/LogDB_Q 入队，然后 _init() 清零。
    void updateDataIntoDB();

    // 0x086105ac：m_useCount=0，m_usedItems 填 0x1d1b/0x1d1c/0x1d1d 三项，
    // m_output.clear()。
    void _init();

    // ---- 布局（ORIG ctor/dtor 逐指令；c6root 4.4.7/m32 实测 sizeof=0x28）----
    std::vector<unsigned long> m_usedItems;                 // +0x00
    unsigned int               m_useCount;                  // +0x0c
    std::map<unsigned long, unsigned int> m_output;         // +0x10
};

// ORIG ctor 0x0861062c：vector@+0x00 + map@+0x10，且 incUseCount 写 +0x0c；
// c6root 4.4.7/m32 下 vector=0x0c、_Rb_tree(map)=0x18 ⇒ sizeof=0x28。
typedef char df_game_vm_size_assert[sizeof(CVendingMachineStatistic) == 0x28 ? 1 : -1];

// 0x0860f8ad：函数内静态单例（__cxa_guard 构造-on-first-use + __cxa_atexit 析构）。
CVendingMachineStatistic* GetInstanceVendingMachineStatistic();

class TimerVendingMachineItemNotify
{
public:
    // 0x08638074：static。仅转调 G_TimerQueue()->InsertTimer(OBJ_TYPE_0, uid,
    // TIMER_MESSAGE(0xa), sec, param2, param3)。
    static void registTimer(int uid, unsigned long itemIdx,
                            unsigned int count, unsigned int type);
};

#endif  // DNF_GAME_VENDING_MACHINE_H_
