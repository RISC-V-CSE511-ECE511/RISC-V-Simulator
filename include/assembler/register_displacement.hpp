#include <assembler/immediate.hpp>
#include <assembler/operand.hpp>
#include <assembler/register.hpp>
#include <cstdlib>
#include <range/v3/all.hpp>

namespace assembler {
struct RegisterDispOp : Operand {
 private:
  std::int32_t m_displacement;
  std::string m_register_name;
  std::string m_operand;

 public:
  RegisterDispOp( const std::string_view operand ) : m_operand( operand ) {
    parseOperand();
  }
  std::string getBinaryValue() override {
    std::string register_binary = Register( m_register_name ).getBinaryValue();
    std::string displacement_binary =
        Immediate( m_displacement, 12 ).getBinaryValue();
    return displacement_binary + register_binary;
  }

  std::string getRegisterBinary() {
    return Register( m_register_name ).getBinaryValue();
  }
  std::string getDisplacementBinary() {
    return Immediate( m_displacement, 12 ).getBinaryValue();
  }

  std::int32_t getDisplacement() { return m_displacement; }
  std::string getRegisterName() { return m_register_name; }

 private:
  void parseOperand() {
    auto components =
        m_operand | ranges::views::split( '(' ) |
        ranges::views::transform( []( auto&& component ) {
          return std::string_view( &*component.begin(),
                                   ranges::distance( component ) );
        } ) |
        ranges::to_vector;

    m_displacement = std::atoi( components[0].data() );

    // Register name
    components[1].remove_suffix( 1 );  // Remove the ending braces
    m_register_name = components[1];
  }
};

}  // namespace assembler
