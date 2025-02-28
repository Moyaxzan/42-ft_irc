#include "../include/Gun.hpp"

Gun::Gun(void)
{
    for (int i = 0; i < 6; i++)
    {
        if (i == 5)
            this->bullets_.push_back(true);
        else
            this->bullets_.push_back(false);
    }
    std::srand(std::time(0));
    this->shuffleBullets();
}

Gun::Gun(Gun const &other)
{
    *this = other;
}

Gun::~Gun(void) {}

Gun &Gun::operator=(Gun const &other)
{
    this->bullets_ = other.bullets_;
    return *this;
}

void Gun::shuffleBullets(void)
{
    std::random_shuffle(this->bullets_.begin(), this->bullets_.end());
    for (int i = 0; i < 6; i++)
        this->bullets_[i] ? std::cout << "true\n" : std::cout << "false\n";
}

bool Gun::checkBullet(void)
{
    for (int i = 0; i < 6; i++)
        this->bullets_[i] ? std::cout << "true\n" : std::cout << "false\n";
    if (this->bullets_[0])
        return true;
    this->bullets_.erase(this->bullets_.begin());

    return false;
}