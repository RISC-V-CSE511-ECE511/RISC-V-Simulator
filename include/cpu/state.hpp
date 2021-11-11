#pragma once

#include <string>

namespace cpu {
struct State {
  std::string memory = "";
  std::int32_t register_file[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0 };
  std::int32_t PC = 0;
  std::string IR = "";
  std::int32_t halt_adr;
};
}  // namespace cpu