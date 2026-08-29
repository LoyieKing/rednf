// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// ORIG mangled _ZN16StackablePackage19EventProcessManager23EventProcessManagerImplC1Ev
// = StackablePackage::EventProcessManager 的嵌套类 EventProcessManagerImpl
// （mangled 与 namespace 形式相同，但 StackablePackage.h 中 EventProcessManager 是
// class，此处必须用嵌套类前向声明，不能开同名 namespace）。
// ctor W 0x08281c26 / dtor W 0x08281d0a；完整定义在 StackablePackage.cpp。
#ifndef DNF_GAME_EVENTPROCESSMANAGERIMPL_H_
#define DNF_GAME_EVENTPROCESSMANAGERIMPL_H_

namespace StackablePackage {

struct EventProcessManager::EventProcessManagerImpl;

}  // namespace StackablePackage

#endif  // DNF_GAME_EVENTPROCESSMANAGERIMPL_H_
