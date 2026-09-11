// ---------------------------------------------------------------------------
// 21. CUser::update_record @0x08676768→0x08676876(map find/highscore/insert)
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08676876：p167=M0/E4:_ZNSt4pairIi14GameResultTypeED1Ev;_ZSt9make_pairIRiR14GameResultTypeESt4pairINSt17__decay_and_stripIT_E6__typeENS4_IT0_E6__typeEEOS5_OS8_;_ZNSt4pairIKi14GameResultTypeEC1IiS1_EEOS_IT_T0_E，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08676876：N86/P135 封顶——p167 差集：MISSING
// getBattleRecordRefW×2 + `_Rb_tree_iterator::C1Ev` + `pair<const int,GameResultType>::C1`
// 右值版，EXTRA make_pair/`pair::D1`×2（4.1 临时槽+拷贝 vs 4.4 直构）。
// [b28d-PoolCUOO2 修成 2026-09-09] 按上述差集逐项翻形（RefW×3 重调 / iterator 默认构造
// 后赋值 / 具名 value_type 块作用域 D1 / 单出口 ret 槽 / 右值 pair 实参 / `GameResultType&`
// 具名引用物化）⇒ **p167 CLEAR**、gap 26→0、ndiff 92→32（残余=setcc/装载序 T5 族，
// N82 维持封顶）。
int CUser::update_record(GameResultType& result)
{
    // [b28d-PoolCUOO2 修法 2026-09-09] ORIG 0x08676876 序列（objdump 逐条）：
    // ①`movl $0x0,-0x10` 返回槽单出口；②iterator 默认构造(-0x7c C1Ev)后 find 结果
    // sret(-0x8c)拷贝赋值（P50 先声明后赋值）；③find/end/insert 三处各自
    // `getBattleRecordRefW()->` 重调（RefW×3，无缓存，技巧37）；④key(-0x78)/
    // newKey(-0x14) 两具名槽；⑤else 支具名 value_type(-0x68) 转发 ctor
    // `pair<Ki,GRT>::C1IiRS1_EEOT_OT0_` + insert(const value_type&)，块尾作用域
    // 自动 D1（+EH cleanup D1 共 ×2）⇒ 删 make_pair/records 缓存形。
    int ret = 0;
    // [A06 2026-09-10 P309/P212 试行记录] ORIG 槽向 it(-0x7c 深)/key(-0x78)/end(-0x74)，
    // 4.4 实测 it(-0x28)/key(-0x24)/end(-0x20) ⇒ 方向相反；把 key 拆「裸声明 + 赋值」
    // 令声明序翻转（key→it）**实测无效**（4.4 按首次使用序分配，N98 成立）⇒ 已回退。
    std::map<int, GameResultType>::iterator it;          // 默认构造（C1Ev）
    int key = result.GetUniqueKey();
    it = getBattleRecordRefW()->find(key);
    if (it != getBattleRecordRefW()->end())
    {
        // ORIG 0x0867690a-0x0867691f：ptEv→`add $0x4`→**落具名槽 -0xc**→回读作 this，
        // ⇒ 具名引用局部（P0：有槽即有名字；4.4 直调形折 lea 少两拍）。
        GameResultType& high = it->second;
        ret = high.UpdateHighScore(result);
    }
    else
    {
        // [b28d-PoolCUOO2] 第一实参传**右值**（ORIG `pair<C1 IiRS1_>` A1=int；具名
        // newKey lvalue 推 A1=int& 反成 `C1 IRiRS0_`）。-0x14 槽是右值临时物化，非具名。
        std::map<int, GameResultType>::value_type record(result.GetUniqueKey(), result);
        getBattleRecordRefW()->insert(record);
        ret = 0xf;
    }
    // [G6Sub3 2026-09-10 复核判 N82附/N93/N98 封顶] 94/94、帧 0xa0 全等；残差=**槽图分区差**
    // （语义等价、无源码解）：ORIG it(-0x7c)/key(-0x78)/end(-0x74)/insert-sret(-0x70) 集中在
    // **深区**，value_type pair record(-0x68..-0x18) 与 newKey(-0x14) 在**浅区**；4.4 恰相反——
    // 大聚合 record 优先占最低槽(-0x7c..-0x2c)，it/key/end 退到浅区(-0x28/-0x24/-0x20)、
    // insert-sret(-0x18)、newKey(-0x1c)。**组内相对序两侧一致**（it 深于 key 深于 end），
    // 只有「it 组 ↔ record 组」的相对位置互换 ⇒ N82附「4.4 聚合优先占低槽 vs 4.1 按生命周期
    // 穿插」。P326 的「显式转换临时」三级槽阶梯不适用（本件 ORIG 比的是**非 const**
    // `_ZNKSt17_Rb_tree_iterator…neERKS4_`，无 const 转换临时）。A06 已试 key 拆「裸声明+赋值」
    // 翻声明序 = 无效（4.4 按首次使用序分配，N98）；record 是 non-POD（pair）⇒ 提前声明会
    // 提前发射 ctor call（P331 负例）⇒ 封顶。
    return ret;
}