// ============================================================================
// importSeriaRoomDecoEventObjectList —— ORIG T 0x0895e79b
//   bool (seriaRoomDecoEventObjectListScript*, const char*)
// ============================================================================
// [b28d-L8] N82 | 槽平移 + ScanType 调用序（ORIG 0x08xxxxxx）
bool importSeriaRoomDecoEventObjectList(seriaRoomDecoEventObjectListScript* s,
                                        const char* path)
{
    // [round5 收口实测] ndiff=195（263/262）且 shift.py=MIXED：主体为**整体 +8 平移**
    // （token -0x5c→-0x54、tmpStr -0x60→-0x58、info -0x78→-0x70、key -0x7c→-0x74；
    // 两侧帧同为 0x90）⇒ 4.4 outgoing 对齐区/首槽基址常数族（N115/N126）；
    // 余项为 STL 临时/分配器槽（-0x51 vs -0x75，P86/P89 族）。p167：32 调用点
    // （ScanType/ScanStr/ScanInt/insert pair/dtor 链）两侧 CLEAR ⇒ 无语义缺口，封 N。
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    std::string tmpStr;
    int unusedInt = 0;      // ORIG -0x10（movl $0 实证，未使用）
    bool unusedFlag = false;  // ORIG -0xa（movb $0 实证，未使用）

    while (ScanType(token, true))
    {
        if (token == "[event object list]")
        {
            // [laneE1 的 if 改动已由父泳道复核否证并回滚] ORIG 0x0895eacb `jmp 0895e833`
            //   回跳 = 循环体头 ⇒ 原码确为 `while (ScanType(token, true))`（每轮重调用），
            //   info(-0x78)/key(-0x7c) 构造在**循环体内**每轮执行（0x0895e850/0895e85b）。
            //   改成 if 后本 TU identical 44→43 劣化 ⇒ 回滚（ORIG 证据：0x0895eacb 回跳）。
            while (ScanType(token, true))
            {
                seriaDecoObjectInfo info;
                int key = -1;

                if (token == "[object index]")
                {
                    key = -1;
                    info.clear();
                    ScanInt(&key);
                    bool inObject = true;
                    while (inObject)
                    {
                        if (!ScanType(token, true))
                            break;
                        if (token == "[stay animation]")
                        {
                            ScanStr(&info.m_stayAnimation);
                        }
                        else if (token == "[walk animation]")
                        {
                            ScanStr(&info.m_walkAnimation);
                        }
                        else if (token == "[run animation]")
                        {
                            ScanStr(&info.m_runAnimation);
                        }
                        else if (token == "[ramdom animation]")
                        {
                            while (ScanStr(&tmpStr))
                            {
                                std::allocator<char> alloc;
                                std::string tmp(tmpStr.c_str(), alloc);
                                info.m_ramdomAnimations.push_back(std::move(tmp));
                            }
                        }
                        else if (token == "[/object index]")
                        {
                            inObject = false;
                            if (key != -1)
                            {
                                s->m_map.insert(
                                    std::make_pair(key, info));
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}