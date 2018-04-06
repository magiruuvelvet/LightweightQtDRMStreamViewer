#ifndef PROVIDERBUTTON_HPP
#define PROVIDERBUTTON_HPP

#include <QPushButton>

#include <Core/StreamingProviderStore.hpp>

namespace ProviderButton
{
    QPushButton *create(const Provider &provider);
};

#endif // PROVIDERBUTTON_HPP
