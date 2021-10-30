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

  std::string getBinaryValue() override {
    binary_value = m_value;
    return binary_value.to_string() |
           ranges::action::drop( 64 - m_operand_length );
  }
};
}  // namespace assembler