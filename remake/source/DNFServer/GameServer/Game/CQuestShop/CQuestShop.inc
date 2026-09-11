// 顶层 stStatusLevelInfo / stQuestShopStatus 类型定义已上移至 CDataManager.h
// （全局作用域，ORIG mangled 无 stQuestShop 前缀）；本 TU 仅保留方法定义
// （C1 0x891b948 / clear 0x891b8cc / D1 0x83cd826 / C1ERKS_ 0x85f07fe /
// getMaxCount / getNextLevel，见文件后部）。
// ORIG C1 0x85eebc8 size 0x97：CData 基类（vptr）、map@+0x08、map@+0x20 构造，
// _reset() 收尾；异常清理路径按 ORIG try/leave 版式由编译器生成。
CQuestShop::CQuestShop()
{
    _reset();   // ORIG +0x3e：map 构造后直调 _reset（085efd74）
}