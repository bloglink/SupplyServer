#include "Snowflake.h"

Snowflake::Snowflake()
{
    sequence = 0L;
    sequenceBits = 10L;
    sequenceMask = -1L ^ (-1L << sequenceBits);

    machineId = 0L;
    machineBits = 5L;
    machineMask = -1L ^ (-1L << machineBits);
    machineIdShift = sequenceBits;

    datacenterId = 0L;
    datacenterBits = 5L;
    datacenterMask = -1L ^ (-1L << datacenterBits);
    datacenterIdShift = machineBits + sequenceBits;

    lastTimestamp = 0;
    timestampShift = datacenterBits + machineBits + sequenceBits;
}

void Snowflake::setMachineId(quint64 mid, quint64 did)
{
    machineId = mid;
    machineId = machineId & machineMask;

    datacenterId = did;
    datacenterId = datacenterId & datacenterMask;
}

quint64 Snowflake::getTimestamp()
{
    return QDateTime::currentDateTime().toMSecsSinceEpoch();
}

quint64 Snowflake::getNextTimestamp(quint64 lastTimestamp)
{
    quint64 timestamp = getTimestamp();
    while (timestamp <= lastTimestamp)
        timestamp = getTimestamp();
    return timestamp;
}

quint64 Snowflake::getId()
{
    quint64 timestamp = getTimestamp();
    if (lastTimestamp == timestamp) { //同一微妙中生成ID
        sequence = (sequence + 1) & sequenceMask; //用&算计算该微秒内产生的计数是否已经到达上限
        if (sequence == 0) //一微妙内产生的ID计数已达上限，等待下一微妙
            timestamp = getNextTimestamp(lastTimestamp);
    } else { //不同微秒生成ID
        sequence = 0L;
    }
    lastTimestamp = timestamp; //把当前时间戳保存为最后生成ID的时间戳
    quint64 Id = ((timestamp) << (int)timestampShift)
            | (datacenterId << (int)datacenterIdShift)
            | (machineId << (int)machineIdShift)
            | sequence;
    return Id;
}

