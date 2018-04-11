#include "RandomString.hpp"

#include <algorithm>
#include <random>
#include <chrono>

QString RandomString::Hex(quint16 length)
{
    if (length == 0)
        return QString();

    QString rand;
    for (auto i = 0; i < length; i++)
        rand.append(QString::number(randuint(0x0A, 0x63), 16));
    return rand;
}

quint32 RandomString::randuint(quint32 min, quint32 max)
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    std::uniform_int_distribution<quint32> dist(min, max);
    return dist(mt);
}
