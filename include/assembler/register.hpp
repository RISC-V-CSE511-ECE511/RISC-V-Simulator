#pragma once

#include <string>

#include "operand.hpp"

namespace assembler {
struct Register : Operand {
 private:
  std::string m_register_name;

 public:
  Register( const std::string_view reg_name ) : m_register_name( reg_name ) {}
  std::string getBinaryValue() override {
    // TODO : Implement the register name to binary logic
    return "00101";
  }
  std::string getName() { return m_register_name; }
};
}  // namespace assembler