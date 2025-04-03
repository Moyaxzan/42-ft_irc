/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kipouliq <kipouliq@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:40:51 by lekix             #+#    #+#             */
/*   Updated: 2025/04/03 17:01:33 by kipouliq         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_HPP
#define GLOBAL_HPP
#include <iostream>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define ORANGE  "\033[33m"
#define BLUE    "\033[34m"
#define PURPLE  "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"   // Réinitialiser la couleur

#define BOLD    "\033[1m"   
#define UNDERLINE "\033[4m" 
#define BLINK   "\033[5m"   
#define REVERSE "\033[7m"   

extern std::string channel;
extern bool        quit;

#endif