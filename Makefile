NAME = webserv
CXX = c++
CXXFLAGS = -Wextra -Werror -Wall -std=c++98 #-fsanitize=address
RM = rm -f
SRC = webserv.cpp main.cpp parsingConfigFile.cpp server.cpp \
location.cpp utils.cpp error.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	clear

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
	clear
