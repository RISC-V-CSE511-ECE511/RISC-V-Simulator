#include <algorithm>
#include <assembler/operand.hpp>
#include <cstddef>
#include <cstdint>
#include <range/v3/all.hpp>
#include <string>
#include <string_view>

namespace assembler {
struct Label : Operand {
 private:
  std::bitset<64> binary_value;
  std::int32_t m_operand_length;
  std::int32_t m_address = 0;
  std::string m_label_name;

 public:
  Label( const std::string_view& label_name ) : m_label_name( label_name ) {}
  std::string getName() { return m_label_name; }
  void setAddress( std::int32_t address ) { m_address = address; }
  std::string getAddress() {
    if ( m_address ) {
      return std::to_string( m_address );
    } else {
      return "";
    }
  }
  std::int32_t getOperandLength() { return m_operand_length; }
  void setOperandLength( std::int32_t operand_length ) {
    m_operand_length = operand_length;
  }

  OperandType getOperandType() override { return OperandType::LabelOp; }

  std::string getBinaryValue() override {
    binary_value = m_address;
    std::string result =
        m_address != 0 ? binary_value.to_string() |
                             ranges::actions::drop( 64 - m_operand_length )
                       : "";
    std::reverse( result.begin(), result.end() );
    return result;
  }
};
}  // namespace assembler
