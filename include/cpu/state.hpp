#pragma once

#include <string>
#include <vector>

namespace cpu {
struct State {
  static const inline std::int32_t memory_access_latency = 10;
  static const inline std::int32_t decode_time = 1;
  static const inline std::int32_t execute_time = 1;

  std::string memory = "";
  std::int32_t register_file[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0 };
  std::int32_t PC = 0;
  std::string IR = "";
  std::int32_t halt_adr;
  std::int32_t total_instructions = 0;

  std::int32_t cycles_consumed = 0;

  std::vector<std::int32_t> instr_cycles_consumed;
};
}  // namespace cpu