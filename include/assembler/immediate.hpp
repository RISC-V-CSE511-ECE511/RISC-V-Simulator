#pragma once

#include <bitset>
#include <range/v3/all.hpp>

#include "operand.hpp"

namespace assembler {

struct Immediate : Operand {
 private:
  std::bitset<64> binary_value;
  std::int64_t m_value;
  std::int32_t m_operand_length;

 public:
  Immediate() = default;
  Immediate( std::int64_t value, std::int32_t operand_length = 12 )
      : m_value( value ), m_operand_length( operand_length ) {}
  std::int64_t getValue() { return m_value; }

  std::int32_t getOperandLength() { return m_operand_length; }

  OperandType getOperandType() override { return OperandType::ImmediateOp; }

  std::string getBinaryValue() override {
    binary_value = m_value;
    std::string result = binary_value.to_string() |
                         ranges::actions::drop( 64 - m_operand_length );
    std::reverse( result.begin(), result.end() );
    return result;
  }
};
}  // namespace assembler