#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>
#define BOOST_TEST_MODULE assembler_test

#include <assembler/instruction.hpp>
#include <assembler/operand.hpp>
#include <boost/test/unit_test.hpp>
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
    for ( auto& instruction : m_instructions ) {
      std::string binary_encoding = instruction.getBinaryEncoding();
      file << binary_encoding;
    }
    file.close();
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
      }
      m_instructions.emplace_back( instr );
      current_instr_address += 4;
    }
  }

  void resolveLabels() {
    for ( auto& instruction : m_instructions ) {
      for ( auto operand : instruction.getOperands() ) {
        if ( operand->getOperandType() == Operand::OperandType::LabelOp ) {
          Label* label_operand = (Label*)operand.get();
          label_operand->setAddress(
              m_label_adr_map[label_operand->getName()] );
          label_operand->setOperandLength( 12 );
        }
      }
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

using namespace assembler;

std::string get_examples_dir() { return std::string( EXAMPLES ); }
BOOST_AUTO_TEST_CASE( assembler_test_1 ) {
  turbo_asm asm_engine( get_examples_dir() + "sample1.s" );
  asm_engine.dumpBinary( "sample1.txt" );

  std::fstream file( "sample1.txt" );
  std::stringstream file_stream;
  file_stream << file.rdbuf();

  BOOST_REQUIRE_EQUAL( file_stream.str(), "00000000001000001000001001100011" );
}

BOOST_AUTO_TEST_CASE( assembler_test_2 ) {
  turbo_asm asm_engine( get_examples_dir() + "sample2.s" );
  asm_engine.dumpBinary( "sample2.txt" );

  std::fstream file( "sample2.txt" );
  std::stringstream file_stream;
  file_stream << file.rdbuf();

  BOOST_REQUIRE_EQUAL(
      file_stream.str(),
      "0000000000110001000000001011001101000000001100010000000010110011" );
}

BOOST_AUTO_TEST_SUITE_END()