#pragma once

namespace Joby {
// The Interface (Contract)
class ISensor {
    public:
        virtual float read() = 0;
        virtual ~ISensor() = default;
};
}