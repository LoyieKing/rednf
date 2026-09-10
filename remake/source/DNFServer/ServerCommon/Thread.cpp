// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Thread/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Thread.h"

// 原始二进制静态链入 libpthread 弱符号 pthread_equal（08058610，a==b 比较）。
// 此处以弱定义补齐，避免依赖动态 libpthread 的版本化符号。
extern "C" int pthread_equal(pthread_t t1, pthread_t t2) __attribute__((weak));
// ORIG 是 -O2 编译形态（mov 0xc(%ebp),%eax; cmp %eax,0x8(%ebp)），本 TU 其余
// 函数为 -O0。用 optimize 属性只对本函数切 O2 + 保留帧指针，逐条对齐 ORIG。
extern "C" int pthread_equal(pthread_t t1, pthread_t t2) __attribute__((optimize("O2", "no-omit-frame-pointer")));
extern "C" int pthread_equal(pthread_t t1, pthread_t t2) {
    return t1 == t2;
}

#include "Thread/CMutex.inc"

#include "Thread/lock.inc"

#include "Thread/unlock.inc"

template<class T>
CGuard<T>::CGuard(T* mutex) throw() {
    // 原始：先 lock（直接对参数调用），再保存成员
    mutex->lock();
    m_mutex = mutex;
}

template<class T>
CGuard<T>::~CGuard() throw() {
    m_mutex->unlock();
}

template class CGuard<CMutex>;
