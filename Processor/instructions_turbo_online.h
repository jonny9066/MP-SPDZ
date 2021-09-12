
// replaces instructions.h for turbospeedz FD prep

//contains some definitions of functions for ARITHMETIC_INSTRUCTIONS
//  REGINT_INSTRUCTIONS CLEAR_GF2N_INSTRUCTIONS, later used
// in switch case statements in files instruction.cpp and instruction.hpp
// format is X(NAME,PRE,CODE) or X(NAME,CODE)
// @TZ tz online modifies only addition part
#define ARITHMETIC_INSTRUCTIONS \
    X(ADDS, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; auto op2 = &Procp.get_S()[r[2]]; \
            auto edest = &Procp.get_E()[r[0]]; auto eop1 = &Procp.get_E()[r[1]]; auto eop2 = &Procp.get_E()[r[2]]; \
            auto pdest = &Procp.get_Perm()[r[0]]; auto pop1 = &Procp.get_Perm()[r[1]]; auto pop2 = &Procp.get_Perm()[r[2]], \
            *dest++ = *op1++ + *op2++; *edest++ = *eop1++ + *eop2++; *pdest++ = *pop1++ + *pop2++; ) \
    X(SUBS, auto dest = &Procp.get_S()[r[0]]; auto op1 = &Procp.get_S()[r[1]]; auto op2 = &Procp.get_S()[r[2]]; \
            auto edest = &Procp.get_E()[r[0]]; auto eop1 = &Procp.get_E()[r[1]]; auto eop2 = &Procp.get_E()[r[2]]; \
            auto pdest = &Procp.get_Perm()[r[0]]; auto pop1 = &Procp.get_Perm()[r[1]]; auto pop2 = &Procp.get_Perm()[r[2]], \
            *dest++ = *op1++ - *op2++; *edest++ = *eop1++ - *eop2++; *pdest++ = *pop1++ - *pop2++; ) \
    X(LDMS, auto dest = &Procp.get_S()[r[0]]; auto source = &Proc.machine.Mp.MS[n], \
            *dest++ = *source++) \
    X(GLDMS, auto dest = &Proc2.get_S()[r[0]]; auto source = &Proc.machine.M2.MS[n], \
            *dest++ = *source++) \
    X(LDSI, auto dest = &Procp.get_S()[r[0]]; \
            auto tmp = sint::constant(int(n), Proc.P.my_num(), Procp.MC.get_alphai()), \
            *dest++ = tmp) \

           
#define ARITHMETIC_INSTRUCTIONS_DISABLED

#define REGINT_INSTRUCTIONS \
    X(LDMINT, auto dest = &Proc.get_Ci()[r[0]]; auto source = &Mi[n], \
            *dest++ = (*source).get(); source++) \
    X(PRINTSTR, Proc.out << string((char*)&n,sizeof(n)) << flush,) \
    X(PRINTCHR, Proc.out << string((char*)&n,1) << flush,) \

#define REGINT_INSTRUCTIONS_DISABLED 
//@TZ online nothing here

#define CLEAR_GF2N_INSTRUCTIONS \
    X(GLDMC, auto dest = &C2[r[0]]; auto source = &M2C[n], \
            *dest++ = (*source).get(); source++) \


#define CLEAR_GF2N_INSTRUCTIONS_DISABLED