/*
 * Player-Online.cpp
 *
 */

#include "Processor/config.h"
#include "Protocols/Share.h"
#include "GC/TinierSecret.h"
#include "Math/gfp.hpp"
#include "Processor/FieldMachine.h"

#define TURBOPREP

int main(int argc, const char** argv)
{
    cout<<"Running turbospeedz function dependent preprocessing protocol"<<endl;

    ez::ezOptionParser opt;
    DishonestMajorityFieldMachine<Share>(argc, argv, opt, false);
}
