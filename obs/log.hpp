#pragma once
#include <iostream>
#include <string>

namespace mm {
inline void log_info(const std::string& m) { std::cout << "[INFO] " << m << "\n"; }
inline void log_warn(const std::string& m) { std::cout << "[WARN] " << m << "\n"; }
inline void log_err (const std::string& m) { std::cerr << "[ERROR] " << m << "\n"; }
}
