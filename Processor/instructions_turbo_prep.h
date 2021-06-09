
// replaces instructions.h for turbospeedz FD prep

//@TZ contains some definitions of functions for ARITHMETIC_INSTRUCTIONS
//  REGINT_INSTRUCTIONS CLEAR_GF2N_INSTRUCTIONS, later used
// in switch case statements in files instruction.cpp and instruction.hpp
// format is X(NAME,PRE,CODE) or X(NAME,CODE)
#define ARITHMETIC_INSTRUCTIONS \
    X(ADDS, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; \
            auto op2 = &Procp.get_S()[r[2]], \
            *dest++ = *op1++ + *op2++) \
    X(LDMS, auto dest = &Procp.get_S()[r[0]]; auto source = &Proc.machine.Mp.MS[n], \
            *dest++ = *source++) \
    X(GLDMS, auto dest = &Proc2.get_S()[r[0]]; auto source = &Proc.machine.M2.MS[n], \
            *dest++ = *source++) \
            
#define REGINT_INSTRUCTIONS \
    X(LDMINT, auto dest = &Proc.get_Ci()[r[0]]; auto source = &Mi[n], \
            *dest++ = (*source).get(); source++) \
    X(PRINTSTR, ,) \
    X(PRINTCHR, ,) \
    
#define CLEAR_GF2N_INSTRUCTIONS \
    X(GLDMC, auto dest = &C2[r[0]]; auto source = &M2C[n], \
            *dest++ = (*source).get(); source++) \
    X(GPRINTREGPLAIN, , ) \
    


