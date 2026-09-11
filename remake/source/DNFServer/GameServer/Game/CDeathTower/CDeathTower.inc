CDeathTower::CDeathTower()
{
    // [b28d-solo-CDeathTower] verdict=DIFF（orig 45 / our 53，P167：EXTRA CStage::D1 ×1）：
    // 4.4 在 CPlayData 子对象构造点布 ~CStage cleanup（ORIG 只在 CStage 构造点布
    // ~CDungeonMgr）。根因 = CPlayData::CPlayData() 定义在 WongWorkDeathTower_A.cpp
    // 且未加 throw()（P216：callee ctor throw() ⇒ 4.4 判 nothrow 不布 landing）；
    // 该 .cpp 非本泳道（R1 单文件铁律）。**已实测负例（S171 收口）**：给 CPlayData
    // ctor/dtor 加 throw() 后 4.4 把 ctor call 一并折叠（C1 53→29 vs ORIG 45，净负）
    // ⇒ 已回退；P216「纯空体外联 ctor」反例适用，本件 N91 封顶。
    // ORIG 0x82a75da：CDungeonMgr/CStage/CPlayData 子对象 ctor 由编译器生成；
    // 函数体仅将 m_timerKeys[TIMER_MESSAGE 0x48..0x4e] 清零。
    // [b27 封顶] 唯一残差 = EH cleanup 段：4.4 对 CStage 子对象 ctor 异常路径
    // 恒发 ~CStage call + _Unwind_Resume（N91：4.1 对 non-trivial dtor 元素
    // 隐式构造不生成 EH 残段），禁删子对象 dtor ⇒ 无源码解。主体/槽/循环全等。
    for (int i = 0x48; i <= 0x4e; ++i)
        m_timerKeys[i - 0x48] = 0;
}