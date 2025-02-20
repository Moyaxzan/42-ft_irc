#ifndef GUN_HPP
#define GUN_HPP

#include <iostream>
#include <vector>
#include <algorithm>

class Gun
{
    private:
        std::vector<bool> bullets_;        

    public:
        Gun(void);
        Gun(Gun const & other);
        ~Gun(void);
        Gun & operator=(Gun const &other);

        void shuffleBullets(void);
        bool checkBullet(void);
};

#endif
