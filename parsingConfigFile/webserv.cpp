/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 16:05:20 by maneddam          #+#    #+#             */
/*   Updated: 2024/02/14 16:05:21 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

webserv::webserv() {}

webserv::webserv(int server_count) {
    this->_serverCount = server_count;
}

webserv::webserv(const webserv &rhs) {
    this->_serverCount = rhs._serverCount;
    this->_servers = rhs._servers;
}

webserv& webserv::operator=(const webserv &rhs) {
    if (&rhs != this)
    {
        this->_serverCount = rhs._serverCount;
        this->_servers = rhs._servers;
    }
    return *this;
}
webserv::~webserv() {
}


int webserv::get_serverCount() {
    return this->_serverCount;
}

server* webserv::createServer() {
    server *s = new server;
    return s;
}

void webserv::addServer(server &newserv) {
    this->_servers.push_back(newserv);
}

void    webserv::set_serverCount(int i) {
    this->_serverCount = i;
}

std::vector<server>& webserv::getServers() {
    return this->_servers;
}
