# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: tbruinem <tbruinem@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2021/04/25 14:32:38 by tbruinem      #+#    #+#                  #
#    Updated: 2021/04/26 13:07:59 by tbruinem      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = ft_ls
SRCDIR = src/
OBJDIR = obj/
INCDIR = incl
LIBDIR = lib/

LIB = bst vec

SRC =	main.c
OBJ :=	$(SRC:%.c=$(OBJDIR)%.o)
SRC :=	$(SRC:%.c=$(SRCDIR)%.c)

CFLAGS = -Wall -Wextra -Werror

CPPFLAGS = -I $(INCDIR) $(foreach lib, $(LIB),-I $(LIBDIR)$(lib)/$(INCDIR))
LDFLAGS = $(foreach lib, $(LIB),-L $(LIBDIR)$(lib))
LDLIBS = $(foreach lib, $(LIB),-l$(lib))

ifeq ($(DEBUG),1)
	CFLAGS += -g -fsanitize=address
endif

export CFLAGS

all: $(NAME)

$(OBJDIR)%.o : $(SRCDIR)%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o $@

$(LIB):
	@$(MAKE) -s -C $(LIBDIR)$@

$(NAME) : $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $< $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	@rm -f $(OBJ)
	@$(foreach lib, $(LIB), $(MAKE) -sC $(LIBDIR)$(lib) clean;)

fclean: clean
	@rm -f $(NAME)
	@$(foreach lib, $(LIB), $(MAKE) -sC $(LIBDIR)$(lib) fclean;)

re: fclean all

.PHONY: clean fclean re all
