// compress_zip / uncompress_zip 全局原语共享声明（2026-08-27 协调裁决：
// 唯一实现归 APSystemOnlyOrig.cpp（APSystemOnlyOrig 批次）；其余域仅 include 调用，
// 不得重复定义）。
#pragma once

// ORIG T _Z12compress_zipPcRjPKcj @0x086b201f（0xe3 字节）
//   if(srcLen<=0) assert 日志后返回 false；destLen = srcLen + srcLen/5 + 12；
//   compress2(out+4,&destLen,src,srcLen,Z_BEST_COMPRESSION(-1))；
//   要求调用方 *outLen >= destLen+4，成功后写 *(unsigned*)out = srcLen&0x3fffffff、
//   outLen = destLen+4。
bool compress_zip(char* out, unsigned int& outLen, const char* src, unsigned int srcLen);

// ORIG T _Z14uncompress_zipPcRjPKcj @0x086b2102（0x6b 字节）
//   srcLen==0 → *outLen=0 返回 true；rawLen = *(unsigned*)src & 0x3fffffff；
//   uncompress(out,&rawLen,src+4,srcLen)；*outLen = rawLen；zlib rc==0 → true。
bool uncompress_zip(char* out, unsigned int& outLen, const char* src, unsigned int srcLen);
