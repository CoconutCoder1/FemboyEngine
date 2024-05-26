#pragma once

#include <memory>

namespace fe {

template<typename T>
using ScopedPtr = std::unique_ptr<T>;

template<typename T>
using RefPtr = std::shared_ptr<T>;

}