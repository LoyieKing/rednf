// ----------------------------------------------------------------------------
// ORIG T 0x081305f6（onClearReward.md 全文）：(-1 < e && 2 < e) && e == 3 时
//   ProcStage::setState(6) + Timer_StageControl::RegistTimerEPLP(getIndex(), 3)
//   （第二参为字面 3，非透传）。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] onClearReward：ORIG 30 / ours 30。P223 多岛形（三条独立否定
//   守卫 goto + 尾标签串 island1/2/3）逐条复现 ORIG 三岛（js/jle/jne 分落 nop+jmp 岛，
//   第三岛直落；body 尾 jmp join 同形）。唯一残差 = 4.4 语句级重载形参多一条
//   `mov 0xc(%ebp),%eax` + 第三比较 `cmpl $3,mem` vs ORIG `cmp $3,%eax`（P85 跨语句
//   %eax 保活；P0 禁具名局部/绑定形）。语义逐值等价。N 封顶（P85 族）。
// [round5-A29 · D08/C05③ 闸门复核，2026-09-10] ORIG 三守卫落 +0x44/+0x47/+0x4a（**三址
//   互异**：+0x44/+0x47 为 `nop;jmp leave` 岛、+0x4a 为直落 `nop`）⇒ 不属「多守卫**同
//   目标**」族；且我方岛数（3）− ORIG 岛数（3）= 0 < 2，未达 Main 的「差≥2 才试」闸门
//   ⇒ **不套用 D08 合并 `||`**（同 TU 内 `_IsInArea` 0x82f0298 才是该钥匙的正向实例）。
// [round1-L29 2026-09-10 复核] 判据① CLEAR；P286 单 case switch 已实测（27→28 条反劣，已回退）。
//   残差同 onEplp（P85 族）⇒ N（ORIG 0x081305f6）。
void StageControl::onClearReward(StageEndType::T endType)
{
    if (endType < 0)
        goto island1;
    if (endType <= 2)
        goto island2;
    // [b28d-sweep] P286 单 case switch 实测：得 `mov 0xc(%ebp),%eax; cmp $0x3,%eax`
    //   但整函数 27→28 条（第二次比较前仍重载形参，ORIG 26 条为单次装载三连比较）
    //   ⇒ 反向回退，维持 P85「4.1 跨语句 %eax 保活不可复现」封顶。
    // [S4AltarStageMisc · 试2 复核 2026-09-11] case 0/1/2 空体三路 switch 实测：**单次
    //   %eax 装载三连比较复现**（`mov 0xc(%ebp),%eax; test; js; cmp $0x2,%eax; jle;
    //   cmp $0x3,%eax; jne`），但 4.4 把 ORIG 的三个独立空岛（`nop;jmp leave`×3，
    //   0x0813063a/0x0813063d/0x08130640）合并为单出口 ⇒ 25 条 vs ORIG 30（丢 ORIG
    //   独有 5 条），ndiff 3→7 ⇒ 反向回退，维持 goto 岛形（30/30，唯一残差 = 重载）。
    if (endType != 3)
        goto island3;
    // ORIG 0x0813060e：this+4 直访问 m_procStage 槽（无 getProcStage call）、
    // mov (%eax) 直读 m_index（无 getIndex call）、第二参透传 endType（0x81330629）。
    // [BigP3D 成员化 2026-09-12] 真实成员直调（等价 this+4 直访问，无 getProcStage call）。
    m_procStage.setState(ProcStage::State::S_N6);
    Timer_StageControl::RegistTimerEPLP(m_index, endType);
island1:
    ;
island2:
    ;
island3:
    ;
}