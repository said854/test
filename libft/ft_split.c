/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 15:26:10 by sjoukni           #+#    #+#             */
/*   Updated: 2024/10/27 15:26:16 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_word(const char *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if ((s[i] != c) && (i == 0 || s[i - 1] == c))
			count++;
		i++;
	}
	return (count);
}

static int	ft_len(const char *s, char c, int *start)
{
	int	len;

	len = 0;
	while (s[*start] == c)
		(*start)++;
	while (s[*start + len] && s[*start + len] != c)
		len++;
	return (len);
}

static char	**ft_allocate_words(char const *s, char c, char **ptr, int count)
{
	int		start;
	int		index;
	int		word_len;

	index = 0;
	start = 0;
	while (index < count)
	{
		word_len = ft_len(s, c, &start);
		ptr[index] = ft_substr(s, start, word_len);
		if (!ptr[index])
		{
			while (index >= 0)
			{
				free(ptr[index]);
				--index;
			}
			free(ptr);
			return (NULL);
		}
		start += word_len;
		index++;
	}
	ptr[count] = NULL;
	return (ptr);
}

char	**ft_split(char const *s, char c)
{
	char	**ptr;
	int		count;

	if (!s)
		return (NULL);
	count = count_word(s, c);
	ptr = (char **)malloc((count + 1) * sizeof(char *));
	if (!ptr)
		return (NULL);
	return (ft_allocate_words(s, c, ptr, count));
}
