// ============================================================================
// ScriptManager（ORIG LoadScript 0x81a11dc / DestroyScript 0x81a12f8 /
// RegistScript 0x81a136c / Init 0x81a13f6）
// ============================================================================
// b28d-solo-ARAD verdict=IDENTICAL/AE：P13 end 缓存 + P221 全表达式临时链
//（`cMyTrace(...)("Fail"/"Success")`）——具名 cMyTrace 时 4.4 把聚合对象排到低槽
//（fail -0x30/success -0x20、标量 it -0xc/e -0x10），临时链后槽位与 ORIG 逐一对齐
//（it -0x2c / e -0x30 / fail -0x28 / success -0x18）。残余 diff 全为 .o 未 relink
// 的 call/jmp 与 .rodata 重定位显示（§6.3 假阳性）。
bool ScriptManager::LoadScript()
{
    // ORIG 0x81a11dc：Init() 后遍历 set 调用各脚本 Load()；全成功返回 1，
    // 否则 cMyTrace "Fail" 返回 0；每次成功也记 "Success"。
    Init();
    // b28d-60：P13 end 缓存（ORIG end() 在循环外一次 → -0x30 槽）；
    // 成功路径 cMyTrace 第4/5参 ORIG 为 (0,1) ⇒ 源码 (false, true)（原 true,false 反）。
    for (std::set<ScriptInterface*>::const_iterator it = m_scripts.begin(),
                                                     e = m_scripts.end();
         it != e; ++it)
    {
        if (!(*it)->Load())
        {
            // ORIG 0x81a1276：失败记 cMyTrace(pretty, 0x12, 9, true, true) + "Fail"。
            // b28d-solo-ARAD：P221 全表达式临时链（槽序试探）。
            cMyTrace(__PRETTY_FUNCTION__, 0x12, 9, true, true)("Fail");
            return false;
        }
        // ORIG 0x81a12f4：每次成功记 cMyTrace(pretty, 0x15, 9, true, false) + "Success"。
        cMyTrace(__PRETTY_FUNCTION__, 0x15, 9, false, true)("Success");
    }
    return true;
}