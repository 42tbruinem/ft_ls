/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/04/25 13:51:13 by tbruinem      #+#    #+#                 */
/*   Updated: 2021/04/28 15:48:58 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include "bstree.h"
#include "vector.h"
#include <string.h>

# define BUFFER_SIZE 256
# define FLAGS "la"

int	util_memcmp_rev(void *dst, void *src, size_t n)
{
	size_t	i;

	if (!dst || !src)
		return (0);
	i = 0;
	while (i < n && ((unsigned char *)dst)[i] == ((unsigned char *)src)[i])
		i++;
	if (i == n)
		return (0);
	return ((((unsigned char*)dst)[i] - ((unsigned char*)src)[i]) * -1);
}

char	*set_path(int argc, char **argv)
{
	int i;

	i = 1;
	while (i < argc)
	{
		if (argv[i][0] != '-')
			return(argv[i]);
		i++;
	}
	return (".");
}

int		get_directory_entries(t_bstree *tree, DIR *fd)
{
	size_t			length;
	struct dirent	*entry;
	int				ret = 1;

	while ((entry = readdir(fd)) && ret)
	{
		length = strlen(entry->d_name);
		if (length && entry->d_name[0] == '.')
			continue ;
		if (bstree_insert(tree, entry->d_name, length + 1, (void*)length) == -1)
			ret = 0;
	}
	closedir(fd);
	return (ret);
}

void	print_entries(t_bstree *tree)
{
	t_node	*node;

	node = node_lowest(tree);
	while (node)
	{
		printf("%s : %ld\n", (char *)node->key, (size_t)(void *)node->val);
		node = node_next(node);
	}
}

void	fill_entry_vector(t_vec *vec, size_t* maxwidth, t_bstree *tree)
{
	t_node	*node;
	size_t	val;

	node = node_lowest(tree);
	while (node)
	{
		val = (size_t)node->val;
		vec_add(vec, &node->key, 1);
		if (val > *maxwidth)
			*maxwidth = val;
		node = node_next(node);
	}
}

bool	print_colrow(t_bstree *tree)
{
	struct winsize	size;
	t_vec	strings;
	size_t	maxwidth;
	size_t	maxcols;
	size_t	maxrows;

	maxwidth = 1;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (vec_new(&strings, sizeof(char*)) == -1)
		return (false);
	fill_entry_vector(&strings, &maxwidth, tree);
	maxcols = strings.len > (size.ws_col / (maxwidth + 1)) ? size.ws_col / (maxwidth + 1) : strings.len;
	maxrows = strings.len / maxcols + !!(strings.len % maxcols);
	for (size_t i = 0; i < maxrows; i++)
	{
		for (size_t j = 0; j < maxcols && i + (j * maxrows) < strings.len; j++)
			printf("%-*s", (int)maxwidth + 1, *(char**)vec_getref(&strings, i + (j * maxrows)));
		printf("\n");
	}
	vec_destroy(&strings);
	return (true);
}

int		main(int argc, char **argv)
{
	char			*path;
	DIR				*fd;
	t_bstree		tree;

	if (bstree_init(&tree, sizeof(char*), util_memcmp_rev, NULL) == -1)
		return (1);
	path = set_path(argc, argv);
	//argument and flag handling..

	fd = opendir(path);
	if (!fd)
		return (1);
	if (!get_directory_entries(&tree, fd))
		return (1);
	print_colrow(&tree);
	bstree_clear(&tree);
	return (0);
}
