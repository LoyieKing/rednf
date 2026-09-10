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
