/*
 * MascotPrep.h
 *
 */

#ifndef PROTOCOLS_MASCOTPREP_H_
#define PROTOCOLS_MASCOTPREP_H_

#include "ReplicatedPrep.h"
#include "OT/MascotParams.h"

template<class T>
class OTPrep : public virtual BitPrep<T>
{
public:
    typename T::TripleGenerator* triple_generator;

    MascotParams params;

    OTPrep<T>(SubProcessor<T>* proc, DataPositions& usage);
    ~OTPrep();

    void set_protocol(typename T::Protocol& protocol);

    size_t data_sent();
    NamedCommStats comm_stats();
};

template<class T>
class MascotInputPrep : public OTPrep<T>
{
    void buffer_inputs(int player);

public:
    MascotInputPrep(SubProcessor<T> *proc, DataPositions &usage) :
            BufferPrep<T>(usage), BitPrep<T>(proc, usage),
            OTPrep<T>(proc, usage)
    {
    }
};

template<class T>
class MascotTriplePrep : public MascotInputPrep<T>
{
public:
    MascotTriplePrep(SubProcessor<T> *proc, DataPositions &usage) :
            BufferPrep<T>(usage), BitPrep<T>(proc, usage),
            MascotInputPrep<T>(proc, usage)
    {
    }

    void buffer_triples();
};

template<class T>
class MascotDabitOnlyPrep : public virtual MaliciousDabitOnlyPrep<T>,
        public virtual MascotTriplePrep<T>
{
public:
    MascotDabitOnlyPrep(SubProcessor<T>* proc, DataPositions& usage) :
            BufferPrep<T>(usage), BitPrep<T>(proc, usage),
            RingPrep<T>(proc, usage),
            MaliciousDabitOnlyPrep<T>(proc, usage),
            MascotTriplePrep<T>(proc, usage)
    {
    }
    virtual ~MascotDabitOnlyPrep()
    {
    }

    virtual void buffer_bits();
};

template<class T>
class MascotPrep : public virtual MaliciousRingPrep<T>,
        public virtual MascotDabitOnlyPrep<T>
{
    MascotPrep(const MascotPrep&);// @TZ prevent copying
public:
    MascotPrep(SubProcessor<T>* proc, DataPositions& usage) :
            BufferPrep<T>(usage), BitPrep<T>(proc, usage),
            RingPrep<T>(proc, usage),
            MaliciousDabitOnlyPrep<T>(proc, usage),
            MaliciousRingPrep<T>(proc, usage),
            MascotTriplePrep<T>(proc, usage),
            MascotDabitOnlyPrep<T>(proc, usage)
    {
    }
    virtual ~MascotPrep()
    {
    }

    virtual void buffer_bits()
    {
        MascotDabitOnlyPrep<T>::buffer_bits();
    }

    void buffer_edabits(bool strict, int n_bits, ThreadQueues* queues);
};

template<class T>
class MascotFieldPrep : public virtual MascotPrep<T>
{
public:
    MascotFieldPrep<T>(SubProcessor<T>* proc, DataPositions& usage) :
            BufferPrep<T>(usage),
            BitPrep<T>(proc, usage), RingPrep<T>(proc, usage),
            MaliciousDabitOnlyPrep<T>(proc, usage),
            MaliciousRingPrep<T>(proc, usage),
            MascotTriplePrep<T>(proc, usage),
            MascotDabitOnlyPrep<T>(proc, usage),
            MascotPrep<T>(proc, usage)
    {
    }
};

#endif /* PROTOCOLS_MASCOTPREP_H_ */
