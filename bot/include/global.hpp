/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lekix <lekix@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:40:51 by lekix             #+#    #+#             */
/*   Updated: 2025/03/26 16:20:56 by lekix            ###   ########.fr       */
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

// Effets sympas
#define BOLD    "\033[1m"   // Gras
#define UNDERLINE "\033[4m" // Souligné
#define BLINK   "\033[5m"   // Clignotant
#define REVERSE "\033[7m"   // Inversion des couleurs

extern std::string channel;

#endif