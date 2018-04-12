#include "Color.hpp"

#include <cmath>

const QColor Color::changeSaturation(const QColor &color, double value)
{
    static const double Pr = 0.299;
    static const double Pg = 0.587;
    static const double Pb = 0.114;

    const double P = sqrt(
        color.redF()   * color.redF()   * Pr +
        color.greenF() * color.greenF() * Pg +
        color.blueF()  * color.blueF()  * Pb
    );

    QColor newColor;
    newColor.setRedF  (P + (color.redF()   - P) * value);
    newColor.setGreenF(P + (color.greenF() - P) * value);
    newColor.setBlueF (P + (color.blueF()  - P) * value);
    return newColor;
}
