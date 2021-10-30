#include <string>

namespace assembler {
struct Register {
 private:
  std::string m_register_name;

 public:
  std::string getBinaryRep() {
    // TODO : Implement the register name to binary logic
    return "00101";
  }
  void setName( const std::string& register_name ) {
    m_register_name = register_name;
  }
  std::string getName() { return m_register_name; }
};
}  // namespace assembler