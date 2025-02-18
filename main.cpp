/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camelia <camelia@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 10:36:08 by camelia           #+#    #+#             */
/*   Updated: 2025/02/17 19:30:22 by camelia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

int	main(int ac, char **av) {
	
	addrinfo			hints;
	addrinfo			*res;
	int					status;
	int					socketFd;
	int					clientFd;
	sockaddr_storage	clientAddr;// structure pas associée à une famille particulière, permet de récupérer ipv4 ou ipv6 contrairement à sockaddr_in
	socklen_t			addrLen;

	//PREPARATION ADRESSE ET PORT POUR SOCKET SERVER
	memset(&hints, 0, sizeof hints); // initialisation de la structure
	hints.ai_family = AF_UNSPEC; //Ipv4 ou Ipv6
	hints.ai_socktype = SOCK_STREAM; // Connexion TCP
	hints.ai_flags = AI_PASSIVE; // permet d'écouter sur toutes les interfaces, remplit l'IP automatiquement
	
	status = getaddrinfo(NULL, av[1], &hints, &res);
	if (status != 0)
		return (std::cout << "getaddrinfo() error" << std::endl, 1);
	
	// On crée la socket, on la lie et on ecoute dessus:
	// CREATION SOCKET
	socketFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socketFd == -1)
		return (freeaddrinfo(res), std::cout << "socket() error" << std::endl, 1);
	
	//LIAISON SOCKET (à adresse IP et un port local avec bind())
	int	opt = 1;
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	status = bind(socketFd, res->ai_addr, res->ai_addrlen);
	if (status != 0)
		return (freeaddrinfo(res), std::cout << "bind() error" << std::endl, 1);
	
	// ECOUTE SUR SOCKET
	status = listen(socketFd, BACKLOG);
	if (status != 0)
		return (freeaddrinfo(res), std::cout << "listen() error" << std::endl, 1);
	
	// ACCEPTER CONNEXION ENTRANTE
	addrLen = sizeof clientAddr;
	clientFd = accept(socketFd, (sockaddr *)&clientAddr, &addrLen);
	if (clientFd == -1)
		return (freeaddrinfo(res), std::cout << "accept() error" << std::endl, 1);
	std::cout << "New client connected successfully ! socketFd = " << socketFd << ", clientFd = " << clientFd << std::endl;
	return (0);
}

/*
RQES TESTS LANCEMENT PGME :
--> int listen(int fd, int backlog) : permet de marquer la socket comme étant "passive" = utilisée pour accepter les demandes de connexion entrantes sur l'adresse et le port auxquels elle a été liée avec bind()
	backlog = entier représentant le nombre de connexions autorisées dans la file d'attente en attendant d'être acceptées

--> le pgme semble tourner dans le vide -> appel à accept() bloque l'execution en attendant une connexion
client, à prendre en compte lors de la gestion de psieurs connexions client à la fois

--> bind() échoue parfois quand on essaie de relancer le server immédiatement apès l'avoir arrêté à cause de l'état TIME_WAIT du socket
solution = activer SO_REUSEADDR avant d'appeler bind(), ce qui permet au socket d'être réutilisé dans attendre que l'ancien se ferme complètement

--> ssize_t send(int socketFd, const void *buf, size_t len, int flags) : permet d'envoyer des données via une socket de type stream qui utilise une connexion TCP
	buf = contient le message à envoyer
	len = entier représentant la taille en octet du message à envoyer
	flags = entier contenant des drapeaux concernant la transmission de messages, il suffit souvent de mettre 0
	retour = nbre d'octets envoyés avec succès
	ATTENTION : send () peut ne pas tt envoyer d'un coup, il faut donc comparer len au retour de send() ; renvoie -1 cen cas d'erreur et précise l'erreur dans errno

--> ssize_t recv(int socket, void *buf, ssize_t len, int flags) : permet de recevoir des données depuis une socket
	retour = nbre d'octets stockés dans le buffer ; si le retour = 0 ==> l'ordinateur distant a fermé la connexion ; retourne -1 en cas d'erreur (errno)

--> Fermeture de la socket à la fin : avec close(), détruit le fd, empêche toute communication vers cette socket (une erreur sera reçu de l'autre côté en cas de tentative d'envoi)

--> MULTIPLEXAGE = gérer plusieurs sockets sans bloquer
*/