// ============================================================================
// DeleteInvalidItemScript 生命周期与查询。
// C1/D1（0x8ac1d40/0x8ac1dce）：构造 string +0x0/string +0x4/map +0x8 后
// clearScript()；IsInvalidItem2Delete（0x8ac2096）：map.find(idx) != end。
// ============================================================================
DeleteInvalidItemScript::DeleteInvalidItemScript()
{
    // ORIG C1 0x8ac1d40：构造 string/map 后调 clearScript()（+0x35 call）
    clearScript();
}