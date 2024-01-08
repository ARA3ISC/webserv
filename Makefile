NAME = webserv
CXX = c++
CXXFLAGS = -Wextra -Werror -Wall -std=c++98
RM = rm -f
SRC = parsingConfigFile/webserv.cpp main.cpp parsingConfigFile/parsingConfigFile.cpp parsingConfigFile/server.cpp \
parsingConfigFile/location.cpp parsingConfigFile/utils.cpp parsingConfigFile/error.cpp serverSetUp/setUpServer.cpp
OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:%.cpp=%.o))
OBJ_SUBDIRS = $(sort $(dir $(OBJ)))

all: $(OBJ_SUBDIRS) $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	clear

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_SUBDIRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_SUBDIRS):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all
	clear
