void RestrictGeolocation::addAllowCountry(std::string country)
{
    // ORIG 0x08170a9e：op!= 实参 arg0=-0x14(end)/arg1=-0x18(find) ⇒ 源写
    // `end() != it`（it 命名局部固定 find 先求值，4.4 对 != 右起算 it 再算
    // end 临时，槽序与 ORIG 一致）；test/jne 空 then 岛 + insert 尾 jmp/nop
    // （P223 goto 复现形）。
    boost::unordered_set<std::string>::const_iterator it =
        m_allowCountry.find(country);
    if (m_allowCountry.end() != it)
        goto DONE;
    m_allowCountry.insert(country);
DONE:
    ;
}