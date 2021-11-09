#pragma once

#include <string>
#include <unordered_map>

namespace cpu {
struct IInstr {
  inline static std::unordered_map<std::string, std::string> m_uncode_instr_map{
      { "000", "addi" }, { "010", "lw" } };

  static std::string getInstructionName( const std::string& instruction ) {
    std::string unique_code( instruction.rbegin() + 12,
                             instruction.rbegin() + 15 );
    return m_uncode_instr_map[unique_code];
  }
  static std::int32_t getOperand1( const std::string& instruction ) {
    std::string rd( instruction.rbegin() + 7, instruction.rbegin() + 12 );
    std::reverse( rd.begin(), rd.end() );
    return std::stoi( rd, nullptr, 2 );
  }
  static std::int32_t getOperand2( const std::string& instruction ) {
    std::string op2( instruction.rbegin() + 15, instruction.rbegin() + 20 );
    std::reverse( op2.begin(), op2.end() );
    return std::stoi( op2, nullptr, 2 );
  }
  static std::int32_t getOperand3( const std::string& instruction ) {
    std::string op3( instruction.rbegin() + 20, instruction.rbegin() + 32 );
    std::reverse( op3.begin(), op3.end() );
    return std::stoi( op3, nullptr, 2 );
  }
};
}  // namespace cpu