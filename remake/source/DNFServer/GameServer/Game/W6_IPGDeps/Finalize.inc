// ORIG T 0x08104600 (0x6c)。
// [b28d-solo-W6_IPGDeps] verdict=NEAR/N7 封顶（447 口径 43=43，残差仅寄存器角色）。
// 修法（三处真修复）：
// ① 循环体 ORIG 0x08104618 无 i*0x10 缩放（`mov this; mov (%eax),%eax; mov (%eax),%edx;
//    call *%edx`）⇒ 原码对首元素 `m_workers->stop()`（非 m_workers[i]），照抄 P171；
// ② ORIG 0x08104648 段单次 D0 调用、无数组 cookie/元素循环 ⇒ 原码 `delete m_workers`
//    （非 delete[]），照抄；③ ThreadInterface.h 虚函数声明序修正（stop 移到虚析构前，
//    vtable 槽0=stop 与 ORIG _ZTV13ThreadInterface@0x8ce6b90 逐槽一致）；
//    ④ 空 else{return;} 复现 ORIG 0x08104667 尾 `jmp;nop` 岛（P285）。
// 残差：间接 call 目标寄存器 ORIG %edx vs 4.4.7 %eax（N7，P230：后端敏感族）。
// **双后端读数**：4.4.7 = NEAR 43=43；4.4.6 后端（-B cc1plus446bin/，头集仍 4.4.7）
// = AE（本 TU 整体 near 3→0 / ae 155→158，零回归）⇒ 已上报 Main 申请将
// W6_IPGDeps.cpp 纳入 dnf_game_cxx.sh 的 446h47 路由表（路由表禁自改）。
// 语义/调用点/帧全等。
void CIPGMgr::Finalize()
{
    if (m_workers != 0)
    {
        for (unsigned int i = 0; i < m_workerCount; ++i)
        {
            // [b28d-solo-W6_IPGDeps] ORIG 0x08104618 循环体无 i*0x10 缩放：
            // `mov this; mov (%eax),%eax; mov (%eax),%edx; call *%edx`（vptr+0
            // = stop，见 ThreadInterface.h 槽序修正）⇒ 原码对首元素调用，照抄 P171。
            m_workers->stop();
        }
        // [b28d-solo-W6_IPGDeps] ORIG 0x08104648 段单次 D0（vptr+8）调用、无数组
        // cookie/元素析构循环 ⇒ 原码为单对象 `delete m_workers`（非 delete[]），照抄。
        delete m_workers;
        m_workers = 0;
    }
    else
    {
        // [b28d-solo-W6_IPGDeps] ORIG 0x08104667 尾 `jmp;nop` 岛：4.4 对纯正向
        // if 恒 jump-thread，空 else{return;} 保留体尾岛（P285）。
        return;
    }
}
