// ---------------------------------------------------------------------------
// 16) CUser::_updateSaveOption()
//     报告：docs/class_func_reports/CUser/_updateSaveOption.md。ORIG 0x08652dde。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08652dde：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08652dde：N104 封顶——EH 尾块相对序：ORIG 正常路径
// 尾（dtor+epilogue）居函数最末、unwind 块居中；4.4 把 unwind 压最末、正常尾就地内联
// （块内容逐条全等，仅两尾块互换；p167 CLEAR，槽位集相同）。
void CUser::_updateSaveOption()
