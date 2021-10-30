#include <string>

struct Operand {
  virtual std::string getBinaryValue() = 0;
  virtual ~Operand(){};
};