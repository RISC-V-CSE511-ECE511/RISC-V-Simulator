#pragma once

#include <limits.h>

#include <algorithm>
#include <cpu/decoder/connection_info.hpp>
#include <cpu/decoder/i_instr.hpp>
#include <cpu/decoder/instruction_type.hpp>
#include <cpu/decoder/r_instr.hpp>
#include <cpu/decoder/s_instr.hpp>
#include <cpu/decoder/sb_instr.hpp>
#include <cpu/decoder/u_instr.hpp>
#include <cpu/decoder/uj_instr.hpp>
#include <string>
#include <unordered_map>

namespace cpu {
struct Decoder {
 private:
  inline static std::unordered_map<std::string, InstructionType> m_type_map{
      { "1100110", InstructionType::R },  { "1100100", InstructionType::I },
      { "1100000", InstructionType::I },  { "1100010", InstructionType::S },
      { "1100011", InstructionType::SB }, { "1110110", InstructionType::U },
      { "1111011", InstructionType::UJ }, { "1110011", InstructionType::UJ } };

 public:
  static ConnectionInfo decode( const std::string& instruction ) {
    InstructionType instr_type = getType( instruction );
    ConnectionInfo info{ instr_type,
                         getInstructionName( instruction, instr_type ),
                         getOperand1( instruction, instr_type ),
                         getOperand2( instruction, instr_type ),
                         getOperand3( instruction, instr_type ) };
    return info;
  }
  static InstructionType getType( const std::string& instruction ) {
    std::string opcode( instruction.rbegin(), instruction.rbegin() + 7 );
    return m_type_map[opcode];
  }
  static std::string getInstructionName( const std::string& instruction,
                                         InstructionType instr_type ) {
    std::string instruction_name;

    switch ( instr_type ) {
      case InstructionType::R:
        instruction_name = RInstr::getInstructionName( instruction );
        break;
      case InstructionType::I:
        instruction_name = IInstr::getInstructionName( instruction );
        break;
      case InstructionType::S:
        instruction_name = SInstr::getInstructionName( instruction );
        break;
      case InstructionType::SB:
        instruction_name = SBInstr::getInstructionName( instruction );
        break;
      case InstructionType::U:
        instruction_name = UInstr::getInstructionName( instruction );
        break;
      case InstructionType::UJ:
        instruction_name = UJInstr::getInstructionName( instruction );
        break;
    }
    return instruction_name;
  }
  static std::int32_t getOperand1( const std::string& instruction,
                                   InstructionType instr_type ) {
    std::int32_t op1;
    switch ( instr_type ) {
      case InstructionType::R:
        op1 = RInstr::getOperand1( instruction );
        break;
      case InstructionType::I:
        op1 = IInstr::getOperand1( instruction );
        break;
      case InstructionType::S:
        op1 = SInstr::getOperand1( instruction );
        break;
      case InstructionType::SB:
        op1 = SBInstr::getOperand1( instruction );
        break;
      case InstructionType::U:
        op1 = UInstr::getOperand1( instruction );
        break;
      case InstructionType::UJ:
        op1 = UJInstr::getOperand1( instruction );
        break;
    }
    return op1;
  }

  static std::int32_t getOperand2( const std::string& instruction,
                                   InstructionType instr_type ) {
    std::int32_t op2;
    switch ( instr_type ) {
      case InstructionType::R:
        op2 = RInstr::getOperand2( instruction );
        break;
      case InstructionType::I:
        op2 = IInstr::getOperand2( instruction );
        break;
      case InstructionType::S:
        op2 = SInstr::getOperand2( instruction );
        break;
      case InstructionType::SB:
        op2 = SBInstr::getOperand2( instruction );
        break;
      case InstructionType::U:
        op2 = UInstr::getOperand2( instruction );
        break;
      case InstructionType::UJ:
        op2 = UJInstr::getOperand2( instruction );
        break;
    }
    return op2;
  }

  static std::int32_t getOperand3( const std::string& instruction,
                                   InstructionType instr_type ) {
    std::int32_t op3;
    switch ( instr_type ) {
      case InstructionType::R:
        op3 = RInstr::getOperand3( instruction );
        break;
      case InstructionType::I:
        op3 = IInstr::getOperand3( instruction );
        break;
      case InstructionType::S:
        op3 = SInstr::getOperand3( instruction );
        break;
      case InstructionType::SB:
        op3 = SBInstr::getOperand3( instruction );
        break;
      case InstructionType::U:
        op3 = UInstr::getOperand3( instruction );
        break;
      case InstructionType::UJ:
        op3 = UJInstr::getOperand3( instruction );
        break;
    }
    return op3;
  }

 private:
};

}  // namespace cpu