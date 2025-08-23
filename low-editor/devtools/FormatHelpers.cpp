#include "FormatHelpers.h"

namespace LowEngine {
    std::string FormatHelpers::ToString(const sf::Time& time) {
        long totalSec = time.asSeconds();
        if (totalSec < 0) totalSec = 0;
        long h = totalSec / 3600;
        long m = (totalSec % 3600) / 60;
        long s = totalSec % 60;

        std::ostringstream oss;
        if (h > 0) {
            oss << h << ':' << std::setw(2) << std::setfill('0') << m
                    << ':' << std::setw(2) << std::setfill('0') << s;
        } else {
            oss << m << ':' << std::setw(2) << std::setfill('0') << s;
        }
        return oss.str();
    }
}
