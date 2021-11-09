#pragma once

#include <cpu/decoder/instruction_type.hpp>
#include <string>
#include <unordered_map>

namespace cpu {
struct RInstr {
  inline static std::unordered_map<std::string, std::string> m_uncode_instr_map{
      { "0000000000", "add" }, { "0000000010", "sub" }, { "1110000000", "and" },
      { "0110000000", "or" },  { "0010000000", "xor" }, { "1000000000", "sll" },
      { "1010000010", "sra" } };

  static std::string getInstructionName( const std::string& instruction ) {
    std::string fn3( instruction.rbegin() + 12, instruction.rbegin() + 15 );
    std::string fn7( instruction.rbegin() + 25, instruction.rend() );

    std::string unique_code = fn3 + fn7;

    return m_uncode_instr_map[unique_code];
  }
  static std::int32_t getOperand1( const std::string& instruction ) {
    std::string rd( instruction.rbegin() + 7, instruction.rbegin() + 12 );
    std::reverse( rd.begin(), rd.end() );
    return std::stoi( rd, nullptr, 2 );
  }
  static std::int32_t getOperand2( const std::string& instruction ) {
    std::string rs1( instruction.rbegin() + 15, instruction.rbegin() + 20 );
    std::reverse( rs1.begin(), rs1.end() );
    return std::stoi( rs1, nullptr, 2 );
  }
  static std::int32_t getOperand3( const std::string& instruction ) {
    std::string rs2( instruction.rbegin() + 20, instruction.rbegin() + 25 );
    std::reverse( rs2.begin(), rs2.end() );
    return std::stoi( rs2, nullptr, 2 );
  }
};
}  // namespace cpu