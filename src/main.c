/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: tbruinem <tbruinem@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/04/25 13:51:13 by tbruinem      #+#    #+#                 */
/*   Updated: 2021/04/26 13:20:14 by tbruinem      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
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
		if (entry->d_type != DT_DIR)
		{
			length = strlen(entry->d_name);
			if (bstree_insert(tree, entry->d_name, length + 1, (void*)length) == -1)
				ret = 0;
		}
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

void	fill_entry_vector(t_vec *vec, t_vec *lengths, t_bstree *tree)
{
	t_node	*node;
	size_t	val;

	node = node_lowest(tree);
	while (node)
	{
		val = (size_t)node->val;
		vec_add(vec, &node->key, 1);
		vec_add(lengths, &val, 1);
		node = node_next(node);
	}
}

bool	print_colrow(t_bstree *tree)
{
//	struct winsize	size;
	t_vec	strings;
	t_vec	lengths;

//	size_t			rows = 1;

//	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	if (vec_new(&strings, sizeof(char*)) == -1)
		return (false);
	if (vec_new(&lengths, sizeof(size_t)) == -1)
		return (false);
	fill_entry_vector(&strings, &lengths, tree);
	for (size_t i = 0; i < strings.len; i++)
		printf("%s : %ld\n", *(char**)vec_getref(&strings, i), *(size_t*)vec_getref(&lengths, i));


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

//	printf("row: %d | col: %d\n", size.ws_row, size.ws_col);
	fd = opendir(path);
	if (!fd)
		return (1);
	if (!get_directory_entries(&tree, fd))
		return (1);
//	print_entries(&tree);
	print_colrow(&tree);
	bstree_clear(&tree);
	return (0);
}
