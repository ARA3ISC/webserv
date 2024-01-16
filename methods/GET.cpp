/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlarabi <rlarabi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:56:32 by rlarabi           #+#    #+#             */
/*   Updated: 2024/01/16 13:57:21 by rlarabi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "../inc/request.hpp"
#include <unistd.h>

void sendResponse(int clientSocket, int statusCode, std::string statusMessage, std::string content, std::string contentType) {
    // Construct the HTTP response
    std::ostringstream httpResponse;
    httpResponse << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    httpResponse << "Content-Type: " << contentType << "\r\n";
    httpResponse << "Content-Length: " << content.length() << "\r\n";
    httpResponse << "\r\n";
    httpResponse << content;



    std::cout << "httpResponse : " << httpResponse.str() << std::endl;
    // Send the response back to the client
    write(clientSocket, httpResponse.str().c_str(), httpResponse.str().length());
}

void GET(int sockfd, request *req){
    (void)req;
    try{
        // checkMatchingLocations(req->getStartLine().path);
    }catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
    if (req->getStartLine().path != "/")
    {
        std::ostringstream err;
        err << "<!DOCTYPE html>\n";
        err << "<html lang=\"en\">\n";
        err << "<head>\n";
        err << "    <meta charset=\"UTF-8\">\n";
        err << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        err << "    <title>Document</title>\n";
        err << "</head>\n";
        err << "<body>\n";
        err << "    <div> Error 404 </div>\n";
        err << "</body>\n";
        err << "</html>\n";
        sendResponse(sockfd, 404, "Not Found", err.str(), "text/html; charset=utf-8");
        return ;
    }
    std::ostringstream cont;
    cont << "<!DOCTYPE html>\n";
    cont << "<html lang=\"en\">\n";
    cont << "<head>\n";
    cont << "    <meta charset=\"UTF-8\">\n";
    cont << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    cont << "    <title>Document</title>\n";
    cont << "</head>\n";
    cont << "<body>\n";
    cont << "    <form action=\"\" method=\"get\">\n";
    cont << "        <input type=\"text\" name=\"text\" id=\"\">\n";
    cont << "        <input type=\"submit\" value=\"submit\">\n";
    cont << "    </form>\n";
    cont << "</body>\n";
    cont << "</html>\n";

    sendResponse(sockfd, 200, "OK", cont.str(), "text/html; charset=utf-8");
    // std::cout << "body : " << req->getBody() << std::endl; 
}

// void GET(int sockfd, request *req){
//     (void)req;
//     std::ostringstream cont;
//     cont << "<!DOCTYPE html>\n";
//     cont << "<html lang=\"en\">\n";
//     cont << "<head>\n";
//     cont << "    <meta charset=\"UTF-8\">\n";
//     cont << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
//     cont << "    <title>Document</title>\n";
//     cont << "</head>\n";
//     cont << "<body>\n";
//     cont << "    <form action=\"\" method=\"get\">\n";
//     cont << "        <input type=\"text\" name=\"text\" id=\"\">\n";
//     cont << "        <input type=\"submit\" value=\"submit\">\n";
//     cont << "    </form>\n";
//     cont << "</body>\n";
//     cont << "</html>\n";
    
//     sendResponse(sockfd, 200, "OK", cont.str());
//     // std::cout << "body : " << req->getBody() << std::endl; 
// }