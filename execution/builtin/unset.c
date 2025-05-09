/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hakader <hakader@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 02:53:42 by hakader           #+#    #+#             */
/*   Updated: 2025/05/05 10:04:59 by hakader          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../execution.h"

void	ft_env_remove_if(t_env **env, void *data_ref,
			int (*cmp)(void *, void *), void (*del)(void *))
{
	t_env	*prev;
	t_env	*curr;

	prev = NULL;
	curr = *env;
	while (curr)
	{
		if (cmp(curr->key, data_ref) == 0)
		{
			if (prev)
				prev->next = curr->next;
			else
				*env = curr->next;
			if (del)
			{
				del(curr->key);
				del(curr->value);
			}
			free(curr);
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
}

int	cmp_key(void *a, void *b)
{
	return (strncmp((char *)a,
			(char *)b, ft_strlen((char *)b) + 1));
}

int	excute_unset(t_cmd *cmd, t_env **env, t_shell **shell, t_list *alloc_list)
{
	(void)shell;
	char	**check;
	int	i;

	i = 1;
	if (cmd->args)
	{
		check = ft_split(cmd->args[i], '=', alloc_list);
		if (!check)
			return (1);
		else if (ft_strcmp(check[0], "USER") == 0)
			return ((put_error("can't unset USER")), 1);
	}
	while (cmd->args[i])
	{
		ft_env_remove_if(env, cmd->args[i], cmp_key, free);
		i++;
	}
	return (1);
}
