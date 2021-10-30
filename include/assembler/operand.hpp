#include <string>

namespace assembler {
struct Operand {
  virtual std::string getBinaryValue() = 0;
  virtual ~Operand(){};
};
}  // namespace assembler