#pragma once

#include <algorithm>
#include <bitset>
#include <string>

#include "operand.hpp"

namespace assembler {
struct Register : Operand {
 private:
  std::string m_register_name;

 public:
  Register( const std::string_view reg_name ) : m_register_name( reg_name ) {}

  OperandType getOperandType() override { return OperandType::RegisterOp; }
  std::string getBinaryValue() override {
    std::string_view str( m_register_name.begin() + 1, m_register_name.end() );

    std::string result = std::bitset<5>( std::stoll( str.data() ) ).to_string();
    std::reverse( result.begin(), result.end() );
    return result;
  }
  std::string getName() { return m_register_name; }
};
}  // namespace assembler