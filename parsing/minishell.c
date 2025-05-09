/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hakader <hakader@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:23:38 by sjoukni           #+#    #+#             */
/*   Updated: 2025/05/07 11:13:44 by hakader          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

int	is_empty(char *line)
{
	return (line[0] == '\0');
}

int	main(int ac, char **av, char **envp)
{
	(void)ac;
	(void)av;

	char	*line;
	t_list	*alloc_list = NULL;
	t_shell	*shell = ft_malloc(sizeof(t_shell), &alloc_list);
	if (!shell)
		return (1);
	ft_bzero(shell, sizeof(t_shell));
    shell->envp = envp;
	int i;

    i = 0;
	while (envp[i])
	{
		t_env *node = env_copy(envp[i], alloc_list);
		if (node)
			append_env(&shell->env, node);
		i++;
	}
	mini_display();
	while (1)
	{
		set_prompt_signals();
		line = readline(CYAN "minishell$ " RESET);
		if (!line)
		// {
			// free(line);
			// free_shell(shell);
			exit(0);
		// }
		else if (is_empty(line))
		{
			free(line);
		}
		add_history(line);
		shell->tokens = tokenize_line(shell, line, alloc_list);
		if (shell->tokens && check_syntax(shell->tokens))
		{
			shell->cmds = build_cmd_list(shell->tokens, alloc_list);
			if (shell->cmds)
			{
				print_cmd_list(shell->cmds);
				execution_part(shell, &alloc_list);
			}
		}

		// free_token_list(shell->tokens);
		shell->tokens = NULL;
		//free_cmd_list(shell->cmds);
		shell->cmds = NULL;
		free(line);
	}

	// free_shell(shell);
	return (0);
}
