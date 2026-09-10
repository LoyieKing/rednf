bool is_Victory_in_a_row(std::bitset<32> bits, short victory)
{
    std::bitset<32> maskA((1 << victory) - 1);
    std::bitset<32> maskB((1 << victory) - 1);
    std::bitset<32> res = bits & maskB;
    return res == maskA;
