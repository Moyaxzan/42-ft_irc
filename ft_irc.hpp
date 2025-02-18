/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camelia <camelia@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:38:22 by camelia           #+#    #+#             */
/*   Updated: 2025/02/17 19:22:53 by camelia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h> // socket(), bind(), listen(), accept(), recv(), send()

#include <iostream>
#include <string>

#include <string.h>
#include <unistd.h> // close()

#define BACKLOG 10