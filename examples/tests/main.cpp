#include <fecs/fecs.h>
#include <iostream>

int main()
{
    FECS::Entity id = FECS::Entities()->Create();
    FECS::Components()->Attach<std::uint32_t>(id, 32);

    std::cout << FECS::Components()->Get<std::uint32_t>(id) << std::endl;

    return 0;
}
