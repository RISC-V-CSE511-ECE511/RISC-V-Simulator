#include <unistd.h>

#include <assembler/instruction.hpp>
#include <assembler/operand.hpp>
#include <fstream>
#include <range/v3/all.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace assembler {
struct turbo_asm {
 private:
  std::string m_file_contents;
  std::vector<Instruction> m_instructions;
  std::unordered_map<std::string, std::int32_t> m_label_adr_map;

 public:
  turbo_asm( const std::string& filename )
      : m_file_contents( readFile( filename ) ) {
    auto raw_instructions = getRawInstructions();

    m_instructions.reserve( raw_instructions.size() );

    parseInstructions( raw_instructions );

    resolveLabels();
  }

  void dumpBinary( const std::string& filename ) {
    std::fstream file( filename, std::fstream::out );
    std::string result_binary = dumpBinary();
    file << result_binary;
    file.close();
  }

  std::string dumpBinary() {
    std::string result_binary;
    for ( auto& instruction : m_instructions ) {
      std::string binary_encoding = instruction.getBinaryEncoding();
      result_binary += binary_encoding;
    }
    return result_binary;
  }

  std::vector<Instruction> getInstructions() { return m_instructions; }

 private:
  std::vector<std::string_view> getRawInstructions() {
    auto raw_instructions =
        m_file_contents | ranges::views::split( '\n' ) |
        ranges::views::transform( []( auto&& component ) {
          return std::string_view( &*component.begin(),
                                   ranges::distance( component ) );
        } ) |
        ranges::to_vector;
    return raw_instructions;
  }

  void parseInstructions( std::vector<std::string_view>& raw_instructions ) {
    std::int32_t current_instr_address = 0;
    for ( auto& raw_instruction : raw_instructions ) {
      Instruction instr = Instruction( std::string( raw_instruction ) );

      if ( instr.hasLabel() ) {
        m_label_adr_map[std::string( instr.getLabelName() )] =
            current_instr_address;

        if ( !instr.getName().empty() ) {
          current_instr_address += 4;
        }

      } else {
        current_instr_address += 4;
      }
      m_instructions.emplace_back( instr );
    }
  }

  void resolveLabels() {
    std::int32_t current_instruction_loc = 0;
    for ( auto& instruction : m_instructions ) {
      for ( auto operand : instruction.getOperands() ) {
        if ( operand->getOperandType() == Operand::OperandType::LabelOp ) {
          Label* label_operand = (Label*)operand.get();
          label_operand->setAddress( m_label_adr_map[label_operand->getName()] -
                                     current_instruction_loc );
          // label_operand->setOperandLength( 12 );
        }
      }
      current_instruction_loc += 4;
    }
  }
  std::string readFile( const std::string& filename ) {
    std::fstream file( filename );
    std::stringstream file_stream;
    file_stream << file.rdbuf();
    std::string file_contents( file_stream.str() );
    return file_contents;
  }
};
}  // namespace assembler