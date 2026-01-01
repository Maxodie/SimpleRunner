#pragma once
#include <limits>
#include <regex>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <memory>
#include <vector>
#include <queue>
#include <array>
#include <stack>
#include <filesystem>
#include <iterator>
#include <string>
#include <assert.h>
#include <cstddef>
#include <iomanip>
#include <fstream>
#include <iostream>

#include "Log/Log.hpp"

using TypeID = std::type_index;
using TypeIDptr = const std::type_info*;

#define SR_INLINE inline
#define SR_FORCEINLINE __forceinline

#ifdef SR_ASSERT
#define CORE_ASSERT(test, msg) do{ if(!(test)) { CORE_LOG_ERROR(msg); assert(0); } } while(0)
#define CORE_ASSERT_PARAMS(test, msg, ...) do{ if(!(test)) { CORE_LOG_ERROR(msg, __VA_ARGS__); assert(0); } } while(0)
#define CLIENT_ASSERT(test, msg) do{ if(!(test)) { CLIENT_LOG_ERROR(msg); assert(0); } } while(0)
#define CLIENT_ASSERT_PARAMS(test, msg, ...) do{ if(!(test)) { CLIENT_LOG_ERROR(msg, __VA_ARGS__); assert(0); } } while(0)
#else
#define CORE_ASSERT(test, msg)
#define CORE_ASSERT_PARAMS(test, msg, ...)
#define CLIENT_ASSERT(test, msg)
#define CLIENT_ASSERT_PARAMS(test, msg, ...)
#endif

#define BIND_FUNCTION_1(function) std::bind(&function, this, std::placeholders::_1)
#define BIND_FUNCTION_2(function) std::bind(&function, this, std::placeholders::_1, std::placeholders::_2)
#define BIND_FUNCTION_3(function) std::bind(&function, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#define BIT(x) (1 << x)

template<typename T>
SR_INLINE TypeID GetTypeID()
{
    return typeid(T);
}

template<typename T>
SR_INLINE TypeIDptr GetTypeIDptr()
{
    return &typeid(T);
}
