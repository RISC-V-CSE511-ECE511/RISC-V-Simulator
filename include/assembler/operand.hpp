#pragma once

#include <cctype>
#include <cstdio>
#include <string>

namespace assembler {
struct Operand {
  enum OperandType { RegisterOp, ImmediateOp, LabelOp, RegisterDisplacementOp };
  static OperandType identifyType( std::string_view operand ) {
    return std::isalpha( operand[0] ) ? operand[0] == 'r' ? RegisterOp : LabelOp
           : operand.find( '(' ) == std::string::npos ? ImmediateOp
                                                      : RegisterDisplacementOp;
  }
  virtual OperandType getOperandType() = 0;
  virtual std::string getBinaryValue() = 0;
  virtual ~Operand(){};
};
}  // namespace assembler