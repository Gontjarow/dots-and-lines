#	https://www.gnu.org/software/make/manual/html_node/Flavors.html
NAME		:=	dots_and_lines

#	http://nuclear.mutantstargoat.com/articles/make/#multiple-source-directories
SOURCES		:=	$(wildcard *.c)
OBJECTS		:=	$(SOURCES:.c=.o)

#	http://nuclear.mutantstargoat.com/articles/make/#automatic-include-dependency-tracking
DEPENDENCY	:=	$(OBJECTS:.o=.d)

#LIBFT_PATH	:= ./libft
#LIBFT		:= -I $(LIBFT_PATH) -L $(LIBFT_PATH) -l ft

#	http://nuclear.mutantstargoat.com/articles/make/#handling-cross-platform-differences
#	https://www.gnu.org/software/make/manual/html_node/Shell-Function.html
SHELL_NAME	:= $(shell uname -s)

ifeq ($(SHELL_NAME), CYGWIN_NT-10.0)
SDL_PATH	:= ./libsdl-win
SDL_MAIN	:= -I $(SDL_PATH) -L $(SDL_PATH) -l SDL2
SDL_IMAGE	:= -I $(SDL_PATH) -L $(SDL_PATH) -l SDL2_image
SDL_MIXER	:= -I $(SDL_PATH) -L $(SDL_PATH) -l SDL2_mixer
endif

#	http://nuclear.mutantstargoat.com/articles/make/#improved-automatic-dependency-tracking
FLAGS		:= -Wall -Wextra -MMD -g
INCLUDES	:= -I.

ifeq ($(SHELL_NAME), CYGWIN_NT-10.0)
FLAGS		:= $(INCLUDES) -lmingw32 -lSDL2main $(SDL_MAIN)
endif

#MSG = \033[38;5;214m
#END = \033[0m

.PHONY: all clean fclean re

#	SDL framework requires that it exists in Library. (See SDL README)
#	Only if frameworks are MISSING, copy them. Otherwise cp outputs errors.
#	https://stackoverflow.com/a/20566812/12215093
all:
	gcc $(SOURCES) -ISDL2\include -LSDL2\lib -lmingw32 -lSDL2main -lSDL2 -o $(NAME)

#	https://www.gnu.org/software/make/manual/html_node/Include.html
-include $(DEPENDENCY)

$(NAME): $(OBJECTS)
	@gcc $(OBJECTS) -o $(NAME) $(FLAGS)
	@echo "Done! (Shell: $(SHELL_NAME))"

%.o: %.c
	@printf "gcc %25s ==> %s\n" $< $@
	@gcc $(FLAGS) -w -c $< -o $@

clean:
	@make -C libft clean
	@rm -f $(OBJECTS)
	@rm -f $(DEPENDENCY)
	@echo "Objects removed!"

fclean: clean
	@make -C libft fclean
	@rm -f $(NAME)
	@echo "Targets removed!"

re: fclean all

shellname:
	@echo "$(SHELL_NAME)"
