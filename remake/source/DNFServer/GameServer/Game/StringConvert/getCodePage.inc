// [b28d-Main R3 迁移·检查点13] getCodePage/setCodePage 自 GameScriptEnumHelpers.cpp 迁入
// （ORIG 0x08adea10/0x08adea1a 与本 TU 的 convertTo* 同 TU），与 convertToWideString/
// convertToCharString 共享本 TU 的 gCodePage ⇒ 修复「setCodePage 写他 TU 副本、
// 转换读本 TU 恒 0 副本」的语义分叉（二进制曾实测 3 份 _ZL9gCodePage，ORIG 仅 1 份）。
int getCodePage()
{
    return gCodePage;
}