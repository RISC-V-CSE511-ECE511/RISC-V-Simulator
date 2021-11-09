#pragma once

#include <string>
#include <unordered_map>

namespace cpu {
struct SBInstr {
  inline static std::unordered_map<std::string, std::string> m_uncode_instr_map{
      { "000", "beq" }, { "100", "bne" }, { "001", "blt" }, { "101", "bge" } };

  static std::string getInstructionName( const std::string& instruction ) {
    std::string unique_code( instruction.rbegin() + 12,
                             instruction.rbegin() + 15 );
    return m_uncode_instr_map[unique_code];
  }
  static std::int32_t getOperand1( const std::string& instruction ) {
    std::string rd( instruction.rbegin() + 15, instruction.rbegin() + 20 );
    std::reverse( rd.begin(), rd.end() );
    return std::stoi( rd, nullptr, 2 );
  }
  static std::int32_t getOperand2( const std::string& instruction ) {
    std::string op2( instruction.rbegin() + 20, instruction.rbegin() + 25 );
    std::reverse( op2.begin(), op2.end() );
    return std::stoi( op2, nullptr, 2 );
  }
  static std::int32_t getOperand3( const std::string& instruction ) {
    std::string immediate_value;
    immediate_value.push_back( *( instruction.rbegin() + 31 ) );
    immediate_value.push_back( *( instruction.rbegin() + 7 ) );

    std::string tmp1( instruction.rbegin() + 25, instruction.rbegin() + 30 );
    std::reverse( tmp1.begin(), tmp1.end() );

    immediate_value += tmp1;

    std::string tmp2( instruction.rbegin() + 8, instruction.rbegin() + 11 );
    std::reverse( tmp2.begin(), tmp2.end() );

    immediate_value += tmp2;

    immediate_value.push_back( '0' );  // Add trailing 0

    return std::stoi( immediate_value, nullptr, 2 );
  }
};

}  // namespace cpu