/*
 * Player-Online.cpp
 *
 */

#include "Processor/config.h"
#include "Protocols/Share.h"
#include "GC/TinierSecret.h"
#include "Math/gfp.hpp"
#include "Processor/FieldMachine.h"


int main(int argc, const char** argv)
{   
#ifdef TZDEBUG
    std::cout<<"DEBUG: Running TZ Prep"<<std::endl;
#endif
    ez::ezOptionParser opt;
    DishonestMajorityFieldMachine<Share>(argc, argv, opt, false);
}
