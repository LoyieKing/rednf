// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StringDictionary/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r StringDictionary（G2 字符串表批次 A）
// ----------------------------------------------------------------------------
// 实现 StringDictionary 全部 10 个 ORIG 强符号（T），语义逐函数对照
// docs/class_func_reports/StringDictionary/*.
//
// 关键事实：
//   * sizeof(StringDictionary) == 0x424（loadStringFile 中 operator_new(0x424)）。
//   * addString：静态 kBufSize = 0x100（0x8e2ca64）。块管理：m_curRemain 不足时
//     分配 max(need, kBufSize) 字节新块并入 blocks_，指针存入 m_curPtr；否则在
//     当前块内划 need 字节。返回首部指针后 memcpy。
//   * getStringHashValue：static，0x8ab21f8；hash = hash*0x10000 + hash*0x40 - hash
//     + (signed char)c，即 hash*65599 + signed char（每个非'\0'字节）。
//   * StringEntity::operator< 仅比较 hash（0x8ab2842）。
//   * freeze：sort(entities_)（默认 operator<）；对桶 i（0..0xff）用合成实体
//     {hash=i<<24,key=0,value=0} 经 lower_bound 定位桶起点写 buckets_[i]；
//     最后 buckets_[0x100] = entities_.end()。
//   * get：hash=hash(key)，桶 = hash>>24；对 [buckets_[桶], buckets_[桶+1])
//     equal_range 后线性 strcmp 命中即返回 value。
// ============================================================================

#include "StringDictionary.h"

#include <algorithm>
#include <cctype>
#include <cstring>

// ORIG getStringHashValue 为文件局部 static（0x8ab21f8，nm t）。
//   文件级 `static`（非匿名 namespace）：mangled 恰为 _ZL18getStringHashValuePKc
//   与 ORIG 一致（匿名 namespace 会错发 _ZN12_GLOBAL__N_1…，Main 修正）。
//   hash = 0; for each non-null byte c: hash = hash*65599 + (signed char)c;
//   return hash.
static unsigned int getStringHashValue(const char* s)
{
    // [b28d-48] ORIG 0x08ab21f8：具名 char c 槽(-0x1) + `c=*s++` 先读后增
    // （@0x05 movzbl → @0x06 mov %al,-0x1 → @0x07 addl $1,0x8(%ebp) →
    //  @0x08 cmpb $0,-0x1 → je），for 形被 4.4 折成 ++s 后置。
    unsigned int hash = 0;
    char c;
    while (true)
    {
        c = *s++;
        if (c == '\0')
            break;
        // hash*0x40 + hash*0x10000 - hash + c = hash*0x40ff + c = hash*65599 + c
        hash = (hash << 6) + (hash << 16) - hash + (unsigned int)(signed char)c;
    }
    return hash;
}

// ----------------------------------------------------------------------------
// StringDictionary::StringDictionary()（0x8ab2240）
//   vector<unsigned char*> ctor；m_curPtr=0；m_curRemain=0；
//   vector<StringEntity> ctor；0x100 个桶迭代器默认构造。
//   注意：ORIG 不构造 buckets_[0x100]（末端迭代器由 freeze 写入），但为
//   与额外默认构造一致无 ABI 影响；这里显式将 0x101 个迭代器 value-init。
// ----------------------------------------------------------------------------
#include "StringDictionary/StringDictionary.inc"
}

// ----------------------------------------------------------------------------
// StringDictionary::~StringDictionary()（0x8ab22be）
//   遍历 blocks_，delete[] 每个非空块；随后 ~vector<StringEntity>、~vector<uchar*>。
// ----------------------------------------------------------------------------
#include "StringDictionary/dtor_StringDictionary.inc"
}

// ----------------------------------------------------------------------------
// addString(const char*)（0x8ab2772）
//   len = strlen(s); need = len + 1;
//   if (m_curRemain < need) {
//       cap = max(need, kBufSize);
//       ptr = new unsigned char[cap];
//       blocks_.push_back(ptr);
//       m_curPtr = (int)ptr + need;
//       m_curRemain = cap - need;
//   } else {
//       ret = m_curPtr; m_curPtr += need; m_curRemain -= need;
//   }
//   memcpy(ret, s, need);
//   return (const char*)ret;
// ----------------------------------------------------------------------------
#include "StringDictionary/addString.inc"
}

// ----------------------------------------------------------------------------
// add(const char* key, const char* value)（0x8ab2398）
//   keyP = addString(key); valP = addString(value);
//   StringEntity e{getStringHashValue(keyP), keyP, valP};
//   entities_.push_back(e);
// ----------------------------------------------------------------------------
#include "StringDictionary/add.inc"
}

// ----------------------------------------------------------------------------
// freeze()（0x8ab240e）
//   std::sort(entities_.begin(), entities_.end());   // 默认 operator<（仅 hash）
//   for (i = 0; i < 0x100; ++i) {
//       StringEntity probe; probe.hash = i << 24; probe.key = 0; probe.value = 0;
//       buckets_[i] = std::lower_bound(entities_.begin(), entities_.end(), probe);
//   }
//   buckets_[0x100] = entities_.end();
// ----------------------------------------------------------------------------
#include "StringDictionary/freeze.inc"
}

// ----------------------------------------------------------------------------
// get(const char* key) const（0x8ab2528）
//   h = getStringHashValue(key); 桶 = h >> 24;
//   pair = equal_range(buckets_[桶], buckets_[桶+1], probe{h,key,0});
//   在 [first, second) 内 strcmp(key) 命中则返回 entity.value，否则 0。
// ----------------------------------------------------------------------------
#include "StringDictionary/get.inc"
}

// ----------------------------------------------------------------------------
// getCount() const（0x8ab2612）
//   return entities_.size();
// ----------------------------------------------------------------------------
#include "StringDictionary/getCount.inc"
}

// ----------------------------------------------------------------------------
// getKey(int index) const（0x8ab2628）
//   越界（<0 或 >= size）返回 0，否则 entities_[index].key。
// ----------------------------------------------------------------------------
#include "StringDictionary/getKey.inc"
}

// ----------------------------------------------------------------------------
// getValue(int index) const（0x8ab2668）
//   越界返回 0，否则 entities_[index].value。
// ----------------------------------------------------------------------------
#include "StringDictionary/getValue.inc"
}

// ----------------------------------------------------------------------------
// setValue(int index, const char* value)（0x8ab26a8）
//   越界跳过；否则重新 addString(key) 与 addString(value)（保持存储内一致性），
//   entities_[index] = { getStringHashValue(newKey), newKey, newValue }。
// ----------------------------------------------------------------------------
#include "StringDictionary/setValue.inc"
}
