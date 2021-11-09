#pragma once

#include <string>
#include <unordered_map>

namespace cpu {
struct UInstr {
  inline static std::unordered_map<std::string, std::string> m_uncode_instr_map{
      { "1110110", "lui" } };

  static std::string getInstructionName( const std::string& instruction ) {
    std::string unique_code( instruction.rbegin(), instruction.rbegin() + 7 );
    return m_uncode_instr_map[unique_code];
  }
  static std::int32_t getOperand1( const std::string& instruction ) {
    std::string rd( instruction.rbegin() + 7, instruction.rbegin() + 12 );
    std::reverse( rd.begin(), rd.end() );
    return std::stoi( rd, nullptr, 2 );
  }
  static std::int32_t getOperand2( const std::string& instruction ) {
    std::string op2( instruction.rbegin() + 12, instruction.rbegin() + 32 );
    std::reverse( op2.begin(), op2.end() );
    return std::stoi( op2, nullptr, 2 );
  }
  static std::int32_t getOperand3( const std::string& instruction ) {
    return std::numeric_limits<std::int32_t>::max();
  }
};

}  // namespace cpu