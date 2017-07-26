#ifndef SNOWFLAKE_H
#define SNOWFLAKE_H

#include <qmath.h>
#include <QDebug>
#include <QDateTime>

class Snowflake
{
public:
    Snowflake();
public:
    quint64 getId();
    void setMachineId(quint64 mid,quint64 did);
private:
    quint64 getTimestamp();
    quint64 getNextTimestamp(quint64 lastTimestamp);
private:
    quint64 sequence;
    quint64 sequenceMask;
    quint64 sequenceBits;

    quint64 machineId;
    quint64 machineMask;
    quint64 machineBits;
    quint64 machineIdShift;

    quint64 datacenterId;
    quint64 datacenterMask;
    quint64 datacenterBits;
    quint64 datacenterIdShift;

    quint64 lastTimestamp;
    quint64 timestampShift;


};

#endif // SNOWFLAKE_H
