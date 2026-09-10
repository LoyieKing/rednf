bool importCraneMinigameScript(CraneMinigameScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                 // ORIG -0xe4
    CraneItemInfo* currentItem = 0;    // ORIG -0xe8（+0x3e movl $0）
    bool ok = false;                   // ORIG -0xe9（+0x48 movb $0；仅
                                       // [need material] 首 ScanInt 取址）

    for (;;)
    {
        if (!ScanType(token, true))    // +0x4f（xor$1 物化 + P164 scan_exit 岛）
        {
            break;                     // +0x70c：mov $1,%ebx → 尾部 dtor → return true
        }

    redo_lookup:                       // +0x70：两条重派发边（+0x250/+0x701）落点
        static LookupMap __lookup;     // guard + __cxa_atexit(map::~map)
        static SpinLock __init_lock;   // guard + __cxa_atexit(SpinLock::~SpinLock)
        static bool __inited = false;  // 零初始化 plain bool（无 guard）

        int tagId;                     // ORIG -0x1c
        if (__inited)
        {
            const char* key = token.c_str();            // +0x173
            LookupMap::const_iterator it = __lookup.find(key);   // +0x17c
            // +0x1d3 end() → +0x206 operator!= → 命中 operator->(+0x218) 取
            // second（+0x220 单点汇聚），未命中 +0x222 mov $-1
            tagId = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                        // +0x22c
            if (__inited)
            {
                __init_lock.leave();                    // +0x24a
                goto redo_lookup;                       // +0x250 → guard 重查
            }
            tagId = 0;                                  // +0x255 注册链入口
        }

        switch (tagId)
        {
        case 0:
        case 0x36:                        // [viewCnt]（+0x274 与 case 0 共 leaf）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[viewCnt]", 0x36));
            }
            else
            {
                // +0x2d1：ScanInt(NULL) → 16 位存 script+0x00
                script->m_viewCnt = static_cast<unsigned short>(ScanInt(static_cast<bool*>(0)));
                break;
            }
            /* fall through（+0x2cf jmp +0x2ea） */
        case 0x3b:                        // [item]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[item]", 0x3b));
            }
            else
            {
                // +0x347：new CraneItemInfo → +0x04 int → +0x00 word = size()
                currentItem = new CraneItemInfo();
                currentItem->m_itemIdx = ScanInt(static_cast<bool*>(0));
                currentItem->m_id =
                    static_cast<unsigned short>(script->m_items.size());
                break;
            }
            /* fall through（+0x345 jmp +0x398） */
        case 0x42:                        // [cnt]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[cnt]", 0x42));
            }
            else
            {
                // +0x3f5：ORIG 无判空，直接 16 位写 currentItem+0x08
                currentItem->m_cnt = static_cast<unsigned short>(ScanInt(static_cast<bool*>(0)));
                break;
            }
            /* fall through（+0x3f3 jmp +0x410） */
        case 0x47:                        // [viewRatio]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[viewRatio]", 0x47));
            }
            else
            {
                currentItem->m_viewRatio = ScanFloat(static_cast<bool*>(0));   // +0x46d fstps +0x0c
                break;
            }
            /* fall through（+0x46b jmp +0x487） */
        case 0x4c:                        // [pickRatio]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[pickRatio]", 0x4c));
            }
            else
            {
                currentItem->m_pickRatio = ScanInt(static_cast<bool*>(0));     // +0x4d3 32 位 +0x10
                break;
            }
            /* fall through（+0x4d3 jmp +0x4ef） */
        case 0x51:                        // [/item]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/item]", 0x51));
            }
            else
            {
                // +0x53d：无条件 push_back(&currentItem)，ORIG 不复位指针
                script->m_items.push_back(currentItem);
                break;
            }
            /* fall through（+0x53b jmp +0x55a） */
        case 0x56:                        // [material]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[material]", 0x56));
            }
            else
            {
                // +0x5a8：script+0x10 int、script+0x14 word
                script->m_material = ScanInt(static_cast<bool*>(0));
                script->m_field14 = static_cast<short>(ScanInt(static_cast<bool*>(0)));
                break;
            }
            /* fall through（+0x5a6 jmp +0x5d4） */
        case 0x5c:                        // [send mail]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[send mail]", 0x5c));
            }
            else
            {
                importActionMailScript(&script->m_sendMail);   // +0x622
                break;
            }
            /* fall through（+0x620 jmp +0x635） */
        case 0x61:                        // [need material]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[need material]", 0x61));
            }
            else
            {
                // +0x683 内层环：pair 每轮默认构造（+0x68c）→ first=ScanInt(&ok)
                // → !ok 跳出（+0x6b1）→ second=ScanInt(NULL) → push_back(引用)
                while (true)
                {
                    std::pair<int, int> material;
                    material.first = ScanInt(&ok);
                    if (ok != true)
                        break;
                    material.second = ScanInt(static_cast<bool*>(0));
                    script->m_needMaterials.push_back(material);
                }
            }
        }

        if (!__inited)                   // 取反短路物化（P32 xor$1 岛）
        {
            __init_lock.leave();         // +0x6f5
            __inited = true;             // +0x6fe
            goto redo_lookup;            // +0x701 → guard 重派发同 token
        }
        else
        {
            goto scan_next;              // +0x706 nop 岛 → +0x4f 回边
        }

    scan_next:;
    }
    return true;                         // +0x70d mov $1,%ebx（dtor 后统一出口）
