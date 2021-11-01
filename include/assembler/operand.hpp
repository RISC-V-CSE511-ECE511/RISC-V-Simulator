#pragma once

#include <string>

namespace assembler {
struct Operand {
  enum OperandType { RegisterOp, ImmediateOp };
  static OperandType identifyType( std::string_view operand ) {
    return operand[0] == 'r' ? RegisterOp : ImmediateOp;
  }
  virtual std::string getBinaryValue() = 0;
  virtual ~Operand(){};
};
}  // namespace assembler