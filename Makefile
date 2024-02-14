NAME = webserv
CXX = c++
CXXFLAGS = -Wextra -Werror -Wall -std=c++98
RM = rm -f

SRC = parsingConfigFile/webserv.cpp main.cpp parsingConfigFile/parsingConfigFile.cpp parsingConfigFile/server.cpp \
parsingConfigFile/location.cpp utils/utils.cpp parsingConfigFile/error.cpp  \
dataCenter/client.cpp utils/utils2.cpp utils/utils3.cpp \
dataCenter/dataCenter.cpp dataCenter/parsingRequests.cpp methods/get.cpp  methods/post.cpp  \
CGI/cgi.cpp methods/delete.cpp dataCenter/response.cpp dataCenter/sending.cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ_SUBDIRS) $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	clear


clean:
	$(RM) -r $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
	clear
