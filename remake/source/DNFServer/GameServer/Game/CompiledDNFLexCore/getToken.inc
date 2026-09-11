// ---- getToken（ORIG 0x8ad0fa0）----
// 主循环：EOF 处理（PopInputStream(false)，失败保持 EOF）；INCLUDE(11) 时
// 读下一个 STRING 记号作文件名，LoadStream+PushInputStream 切入包含文件。
int CompiledDNFLex::getToken()
{
    // [b28d-solo-CompiledDNFLexCore 判词·N] 封顶：本 TU 路由 4.4.6 -O2。
    // 已按 P157/P183 去掉 endIt/beginIt 迭代器缓存，改为逐点
    // `streamStack_.end()._M_cur/_M_first/_M_node` 直读（与 ORIG 0x8ad0fa0 同源形，
    // 头部 8 条现逐条一致）。剩余差异为跨版本 RA/装载调度：①ORIG 在
    // `cmp 0x2034(%ebx),%eax` 后预载 `mov 0x203c(%ebx),%edx`，4.4 延迟到边界块内；
    // ②ecx↔edx 寄存器互换（N7 族）；③PopInputStream 调用后 ORIG `jne`/我们 `je`
    // 的块序（与 getStr 同族）；④析构内联 vs ORIG out-of-line stream_t D1。
    // EOF→PopInputStream(false)→递归、INCLUDE(11)→getStr→LoadStream→PushInputStream→
    // 递归、sprintf/LexerError 四条错误路径语义全等 ⇒ N 封顶。
    // [b28d-solo 复验] 199 vs 200，ndiff 112。p167 差集 **CLEAR**（ORIG 25 calls =
    // 我们 25 calls，0 MISSING / 0 EXTRA）⇒ 语义闭合，残余纯 -O2 RA/块调度
    // （①-④ 同判词），维持 N 封顶。
    // [J10 复验 2026-09-11] p167 实测 ORIG 25 calls / 我方 24：MISSING `getToken`×1 系
    // **同 section 内自递归 call 无 R_386_PC32 重定位**的统计伪差（我方尾块
    // `call <getToken+0x9b>` 即递归点），MISSING `_ZNSsD2Ev`×3 / EXTRA `_ZNSsD1Ev`×3
    // 为同址别名伪差（P93）⇒ 调用面实际 CLEAR。fresh 205 vs 211（gap −6）。语义逐块核对：
    // EOF→`lastToken_=0`→Pop(false) 失败返回、成功尾递归；INCLUDE(11)→getStr→toTString→
    // LoadStream→(new[]+istream::read)→toTString→PushInputStream→虚调用释放(sd.stream)→
    // 尾递归；四条 sprintf/LexerError 错误路径全等 ⇒ 维持 N（-O2 RA/块调度）。
    stream_t* p = streamStack_.end()._M_cur;
    if (p != streamStack_.begin()._M_cur)
    {
        if (p == streamStack_.end()._M_first)
            p = *(streamStack_.end()._M_node - 1) + STREAM_NODE_ELEMS;
        p = p - 1;
        if (p != 0)
        {
            if (offset_ >= p->buffer + p->size)
            {
                lastToken_ = DNF_TOK_EOF;
                if (!PopInputStream(false))
                    return lastToken_;
                goto recurse;
            }
        }
    }

    {
    int c = readCharFromBuffer(offset_);
    lastToken_ = c;
    if (flag2040_ == 0)
        return lastToken_;
    if (c != DNF_TOK_INCLUDE)
        return lastToken_;

    // ORIG 内联计算当前流指针（栈顶 = finish._M_cur - 1），不调 GetInputStream()；
    // 边界节点回退 node[-1]+0x1f4-1，栈空时为 0。
    stream_t* incl = streamStack_.end()._M_cur;
    if (incl != streamStack_.begin()._M_cur)
    {
        if (incl == streamStack_.end()._M_first)
            incl = *(streamStack_.end()._M_node - 1) + STREAM_NODE_ELEMS;
        incl = incl - 1;
    }
    else
    {
        incl = 0;
    }
    int next = readCharFromBuffer(offset_);
    lastToken_ = next;
    if (next != DNF_TOK_STRING)
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s/%s, invalid inclusion.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    std::string include_path;
    if (!getStr(include_path))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, invalid inclusion.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    DNFLex::stream_data_t sd;
    if (!LoadStream("", toTString(include_path), &sd))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, LoadStream failed.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    char* sd_buf = sd.buffer;
    if (sd_buf == 0)
    {
        sd_buf = new char[sd.size];
        reinterpret_cast<std::istream*>(sd.stream)->read(sd_buf, sd.size);
    }

    if (!PushInputStream("", toTString(include_path), sd_buf, sd.size))
    {
        std::sprintf(reinterpret_cast<char*>(tokenBuffer_),
                     "%s - %s%s, PushInputStream failed.", __FUNCTION__,
                     toMbcs(incl->dir), toMbcs(incl->filename));
        LexerError(reinterpret_cast<char*>(tokenBuffer_));
        return DNF_TOK_ERROR;
    }

    if (sd.stream != 0)
    {
        void (**vtable)(void*) =
            *reinterpret_cast<void (***)(void*)>(sd.stream);
        vtable[1](sd.stream);
    }
    }
recurse:
    return getToken();
}