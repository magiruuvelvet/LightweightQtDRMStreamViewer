#ifndef RANDOMSTRING_HPP
#define RANDOMSTRING_HPP

#include <QString>

class RandomString
{
public:
    static QString Hex(quint16 length = 8);

private:
    static quint32 randuint(quint32 min, quint32 max);

    RandomString() {}
};

#endif // RANDOMSTRING_HPP
