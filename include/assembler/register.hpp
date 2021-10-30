#include <string>

#include "operand.hpp"

namespace assembler {
struct Register : Operand {
 private:
  std::string m_register_name;

 public:
  std::string getBinaryValue() override {
    // TODO : Implement the register name to binary logic
    return "00101";
  }
  void setName( const std::string& register_name ) {
    m_register_name = register_name;
  }
  std::string getName() { return m_register_name; }
};
}  // namespace assembler