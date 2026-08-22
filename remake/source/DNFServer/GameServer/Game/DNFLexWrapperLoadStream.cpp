// ============================================================================
// DNFLexWrapper 流装载两方法（G4 脚本批次；ORIG 地址与汇编证据见
// docs/class_func_reports/DNFLexWrapper.md）
// ----------------------------------------------------------------------------
// 本 TU 提供：
//   LoadStream()       _ZN13DNFLexWrapper10LoadStreamEPKcS1_PN6DNFLex13stream_data_tE
//                                                                ORIG 0x8ace2b0
//   setFileStreamData() _ZN13DNFLexWrapper17setFileStreamDataEPN6DNFLex13stream_data_tEPKc
//                                                                ORIG 0x8acede0
//
// 依赖符号（由 DNFLexWrapperStream.cpp / DNFLexCore.cpp 提供）：
//   DNFLex::stream_data_t::clear()   _ZN6DNFLex13stream_data_t5clearEv  ORIG 0x8acd140
//   setStringStreamData()            _ZN13DNFLexWrapper19setStringStreamDataEPN6DNFLex13stream_data_tEPcj
//                                                                ORIG 0x8ace9b0
// ============================================================================

#include <fstream>

#include "DNFLexWrapper.h"

// LoadStream（ORIG 0x8ace2b0）：若 SetHandler 注册了 LoadStream 回调
// （+0x610c 非空），以 (dir, filename, s) 尾调用之并返回其 bool；
// 否则返回 false。ORIG 汇编：mov 0x610c(%eax),%eax → test/je → jmp *%eax。
bool DNFLexWrapper::LoadStream(const char* dir, const char* filename,
                               DNFLex::stream_data_t* s)
{
    if (loadStream_ == 0)
        return false;
    return reinterpret_cast<bool (*)(const char*, const char*, DNFLex::stream_data_t*)>(
        loadStream_)(dir, filename, s);
}

// setFileStreamData（ORIG 0x8acede0）：以 in|binary 打开 path 的 ifstream。
//   - path/s 任一为空 → 直接 false；
//   - 先 stream_data_t::clear(s)（stream/size/buffer 清零）；
//   - new ifstream(path, in|binary)；未打开（open 失败置 failbit）或 badbit
//     置位 → delete 流并返回 false；
//   - seekg(0, end) → tellg 取 size → seekg(0, beg)；
//   - size==0 → delete 流并委托 setStringStreamData(s, 0, 0)（空串流）；
//   - 否则 s->stream = ifs; s->size = size; 返回 true。
bool DNFLexWrapper::setFileStreamData(DNFLex::stream_data_t* s, const char* path)
{
    if (path == 0 || s == 0)
        return false;

    s->clear();

    std::ifstream* ifs = new std::ifstream(path, std::ios::in | std::ios::binary);
    if (!ifs->is_open() || ifs->bad())
    {
        delete ifs;
        return false;
    }

    ifs->seekg(0, std::ios::end);
    std::streampos size = ifs->tellg();
    ifs->seekg(0, std::ios::beg);

    if (size == 0)
    {
        delete ifs;
        return setStringStreamData(s, 0, 0);
    }

    s->stream = ifs;
    s->size = static_cast<int>(size);
    return true;
}
