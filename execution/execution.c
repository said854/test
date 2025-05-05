/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <sjoukni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 09:49:04 by hakader           #+#    #+#             */
/*   Updated: 2025/05/05 15:52:53 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution.h"

int path_cmd(t_cmd *f_cmd, char **envp)
{
	pid_t pid;

	if (f_cmd->args && f_cmd->args[0] && f_cmd->args[0][0] == '/')
	{
		if (access(f_cmd->args[0], X_OK) == 0)
		{
			pid = fork();
			if (pid == 0)
				execve(f_cmd->args[0], &f_cmd->args[0], envp);
			else
				waitpid(pid, NULL, 0);
		}
		return (1);
	}
	return (0);
}

static void exec_child(t_cmd *f_cmd, char *cmd, char **envp)
{
	if (f_cmd->infile != NULL)
		infile(f_cmd->infile);
	if (f_cmd->outfile != NULL)
		outfile(f_cmd->outfile);
	execve(cmd, &f_cmd->args[0], envp);
	perror("execve failed");
	exit(1);
}

static void exec_command(t_cmd *f_cmd, char **paths, char **envp, t_env **env_list)
{
	pid_t	pid;
	char	*cmd;

	if (is_builtin_name(f_cmd->args[0]))
	{
		if (f_cmd->has_pipe)
		{
			pid = fork();
			if (pid == 0)
			{
				if (f_cmd->infile)
					infile(f_cmd->infile);
				if (f_cmd->outfile)
					outfile(f_cmd->outfile);
				exit(exec_builtin(f_cmd, env_list));
			}
			else
				waitpid(pid, NULL, 0);
		}
		else
		{
			exec_builtin(f_cmd, env_list);
		}
		return;
	}

	cmd = check_cmd(paths, f_cmd->args[0]);
	if (cmd)
	{
		pid = fork();
		if (pid == 0)
			exec_child(f_cmd, cmd, envp);
		else
			waitpid(pid, NULL, 0);
		free(cmd);
	}
	else
		printf("%s: command not found\n", f_cmd->args[0]);
}

void execution_part(t_cmd *f_cmd, t_env *env_list, char **envp)
{
	char **paths;
	t_cmd *tmp;

	if (!f_cmd)
		return;

	paths = get_paths(env_list);
	while (f_cmd)
	{
		tmp = f_cmd;

		if (f_cmd->has_pipe)
		{
			pipex(f_cmd, env_list, envp);
			while (f_cmd && f_cmd->has_pipe)
			{
				tmp = f_cmd;
				f_cmd = f_cmd->next;
				free(tmp);
			}
			if (f_cmd)
			{
				tmp = f_cmd;
				f_cmd = f_cmd->next;
				free(tmp);
			}
			continue;
		}

		exec_command(f_cmd, paths, envp, &env_list);

		f_cmd = f_cmd->next;
		free(tmp);
	}
}





