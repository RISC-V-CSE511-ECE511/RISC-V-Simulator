#pragma once

#include <cpu/decoder/instruction_type.hpp>
#include <cstdint>
#include <numeric>

namespace cpu {
struct ConnectionInfo {
  InstructionType instr_type;
  std::int32_t operand1 = std::numeric_limits<std::int32_t>::max();
  std::int32_t operand2 = std::numeric_limits<std::int32_t>::max();
  std::int32_t operand3 = std::numeric_limits<std::int32_t>::max();
};
}  // namespace cpu