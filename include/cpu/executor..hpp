#pragma once
#include <bitset>
#include <cpu/decoder/connection_info.hpp>
#include <cpu/state.hpp>
#include <functional>
#include <unordered_map>

namespace cpu {
struct Executor {
  // R Type
 private:
  static void add_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] + rf[conn_info.operand3];
  }
  static void sub_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] - rf[conn_info.operand3];
  }
  static void xor_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] ^ rf[conn_info.operand3];
  }
  static void and_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] & rf[conn_info.operand3];
  }
  static void or_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] | rf[conn_info.operand3];
  }
  static void sll_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] << rf[conn_info.operand3];
  }
  static void sra_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;
    rf[conn_info.operand1] = rf[conn_info.operand2] >> rf[conn_info.operand3];
  }

  // SB Type

  static std::int32_t translateAddress( std::int32_t address ) {
    return address * 8;
  }

  static std::int32_t sext( const std::int32_t value,
                            const std::int32_t msb_position ) {
    std::int32_t result = value;

    // Extract 20 bit value
    std::int32_t msb_value = ( value & ( 1 << ( msb_position - 1 ) ) );

    // Check if the 20 bit is 1
    if ( msb_value ) {
      // Mask out bits after msb as 1
      std::int32_t mask = ( ~0 ) << msb_position;
      // Apply the mask
      result = result | mask;
      return value;
    }
    return value;  // Same value is returned in case the msb is 0
  }

  static void beq_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 == rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }

  static void blt_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 < rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }
  static void bge_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 >= rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }
  static void bne_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs1 = rf[conn_info.operand1];
    std::int32_t rs2 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    if ( rs1 != rs2 ) {
      sys_state.PC -= 32;  // Reverse the change made by fetch
      sys_state.PC += translateAddress( offset );
    }
  }

  // U Type Support
  static void lui_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t immediate = conn_info.operand2;

    immediate = ( immediate << 12 ) & ( ( ~0 ) << 12 );

    rd = sext( immediate, 32 );
  }

  // I Type Support
  static void addi_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t immediate = sext( conn_info.operand3, 12 );

    rd = rs1 + immediate;
  }

  static void lw_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    rd = sext( loadFromMemory( sys_state.memory, rs1 + offset, 4 ), 32 );
  }

  // S Type
  static void sw_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t rs2 = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    storeToMemory( rs2, sys_state.memory, rs1 + offset );
  }

  static void jalr_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t rs1 = rf[conn_info.operand2];
    std::int32_t offset = sext( conn_info.operand3, 12 );

    std::int32_t t = sys_state.PC;
    sys_state.PC = rs1 + sext( translateAddress( offset ), 12 );
    rd = sys_state.PC;
  }

  static void jal_func( State& sys_state, const ConnectionInfo& conn_info ) {
    int* rf = sys_state.register_file;

    std::int32_t& rd = rf[conn_info.operand1];
    std::int32_t offset = sext( translateAddress( conn_info.operand2 ), 12 );

    rd = sys_state.PC;  // Already updated by fetch

    sys_state.PC += offset - 32;  // Already updated by fetch
  }

  static std::int32_t loadFromMemory( const std::string& mem,
                                      std::int32_t address,
                                      std::int32_t num_bytes_to_read ) {
    std::int32_t actual_address = address * 8;
    std::string val( mem.begin() + actual_address,
                     mem.begin() + actual_address + ( num_bytes_to_read * 8 ) );
    std::reverse( val.begin(), val.end() );
    return std::stoi( val, nullptr, 2 );
  }

  static void storeToMemory( std::int32_t value, std::string& mem,
                             std::int32_t address ) {
    std::int32_t actual_address = address * 8;
    std::string binary_value = std::bitset<32>( value ).to_string();
    std::reverse( binary_value.begin(), binary_value.end() );
    std::copy( binary_value.begin(), binary_value.end(),
               mem.begin() + actual_address );
  }

 private:
  static inline std::unordered_map<
      std::string, std::function<void( State&, const ConnectionInfo& )>>
      m_instr_func_map{ { "add", std::bind( &add_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "sub", std::bind( &sub_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "xor", std::bind( &xor_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "and", std::bind( &and_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "or", std::bind( &or_func, std::placeholders::_1,
                                           std::placeholders::_2 ) },
                        { "sll", std::bind( &sll_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "sra", std::bind( &sra_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "addi", std::bind( &addi_func, std::placeholders::_1,
                                             std::placeholders::_2 ) },
                        { "lw", std::bind( &lw_func, std::placeholders::_1,
                                           std::placeholders::_2 ) },
                        { "sw", std::bind( &sw_func, std::placeholders::_1,
                                           std::placeholders::_2 ) },
                        { "jalr", std::bind( &jalr_func, std::placeholders::_1,
                                             std::placeholders::_2 ) },
                        { "jal", std::bind( &jal_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "beq", std::bind( &beq_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "bne", std::bind( &bne_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "blt", std::bind( &blt_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "bge", std::bind( &bge_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "lui", std::bind( &lui_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "jal", std::bind( &jal_func, std::placeholders::_1,
                                            std::placeholders::_2 ) },
                        { "jalr", std::bind( &jalr_func, std::placeholders::_1,
                                             std::placeholders::_2 ) } };

 public:  // API
  static void execute( State& sys_state, const ConnectionInfo& conn_info ) {
    m_instr_func_map[conn_info.instr_name]( sys_state, conn_info );
  }
};
}  // namespace cpu