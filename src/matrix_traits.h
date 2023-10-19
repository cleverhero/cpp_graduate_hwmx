#pragma once

namespace hwmx {
    template<typename T>
    class MatrixTraits {
    public:
        static double to_double(T value) noexcept {
            return static_cast<double>(value);
        }
    };
}