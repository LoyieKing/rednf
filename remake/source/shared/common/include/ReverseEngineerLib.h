//
// Created by Loyie King on 2024/3/6.
//

#pragma once

typedef unsigned short ushort;
typedef unsigned int uint;

typedef char _ptr_size_check[sizeof(int*) == 4 ? 1 : -1];

#define TEST_CLASS_SIZE(classname, size) typedef char classname##_size_check[(sizeof(classname) == (size)) ? 1 : -1];
