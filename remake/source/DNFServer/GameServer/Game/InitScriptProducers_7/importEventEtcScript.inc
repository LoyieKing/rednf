// ============================================================================
// importEventEtcScript — ORIG 0x0895e30f（init.md +0x6998，"Event/EventEtc.evt"
// → m_eventEtc）。"[seria room tree material list]" 连续 int 入 m_vec0；
// "[seria room deco material list]" 按 (值, 序号) 写 m_mapC（序号自增）；
// "[christmas event end]" 结束并返回 true。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0895e30f
//   P164 分离形 `while(run){ if(!ScanType) break; }` + p 缓存槽(-0x10) +
//   make_pair<int&,int&> 左值对（P61）。orig/ours=128/128。
bool importEventEtcScript(EventEtcScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    // [b28d-solo-InitScriptProducers_7] ORIG -0x10 具名 p 缓存（store 先于 clear
    // call；后续 push_back/mapC insert 的 this 均从该槽取）。
    EventEtcScript* p = s;
    s->clear();

    std::string token;
    bool run = true;

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0895e30f 分离形：条件头仅测 run
    //（cmpb $0,-0x9; jne 体），ScanType 检查落体内首（P164 同族）。
    while (run)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[seria room tree material list]")
        {
            int value = -1;
            for (;;)
            {
                if (!ScanInt(&value))
                {
                    break;
                }
                p->m_vec0.push_back(value);
            }
        }
        else if (token == "[seria room deco material list]")
        {
            int key = 0;
            int index = 0;
            for (;;)
            {
                if (!ScanInt(&key))
                {
                    break;
                }
                // ORIG 0x0895e402：make_pair<int&,int&>(key,index)（P61 左值性）
                // → value_type 转换构造 pair<const unsigned,unsigned>(pair<int,int>)
                // → map::insert(value_type&)。直构 value_type(u,u) 形不同。
                p->m_mapC.insert(std::make_pair(key, index));
                ++index;
            }
        }
        else if (token == "[christmas event end]")
        {
            run = false;
        }
    }
    return !run;
}