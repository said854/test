/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hakader <hakader@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 09:49:04 by hakader           #+#    #+#             */
/*   Updated: 2025/05/07 11:12:11 by hakader          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution.h"

void open_all_infiles(char **infiles)
{
	int fd = -1;
	int i = 0;

	while (infiles && infiles[i])
	{
		fd = open(infiles[i], O_RDONLY);
		if (fd < 0)
		{
			perror(infiles[i]);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	if (fd != -1)
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

void open_all_outfiles(char **outfiles, int *append_flags)
{
	int fd = -1;
	int i = 0;
	int flags;

	while (outfiles && outfiles[i])
	{
		flags = O_WRONLY | O_CREAT;
		if (append_flags[i])
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;

		fd = open(outfiles[i], flags, 0644);
		if (fd < 0)
		{
			perror(outfiles[i]);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	if (fd != -1)
	{
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
}



int	path_cmd(t_shell **shell)
{
	pid_t	pid;

	if ((*shell)->cmds->args[0][0] == '/')
	{
		if (access((*shell)->cmds->args[0], X_OK) == 0)
		{
			pid = fork();
			if (pid == 0)
			{
				execve((*shell)->cmds->args[0],
					&(*shell)->cmds->args[0], (*shell)->envp);
			}
			else
				waitpid(pid, NULL, 0);
		}
		return (1);
	}
	return (0);
}

// static void	exec_child(t_cmd *f_cmd, char *cmd, char **envp)
static void	exec_child(t_shell *shell, char *cmd)
{
	if (shell->cmds->infiles)
	open_all_infiles(shell->cmds->infiles);
	if (shell->cmds->outfiles)
	open_all_outfiles(shell->cmds->outfiles, shell->cmds->append_flags);

	execve(cmd, &shell->cmds->args[0], shell->envp);
	perror("execve failed");
	//free_all
	// exit(1);
}

// static void	exec_command(t_cmd *f_cmd, char **paths,
// 						char **envp, t_list *alloc_list)
// {
// 	pid_t	pid;
// 	char	*cmd;

// 	cmd = check_cmd(paths, f_cmd->args[0], alloc_list);
// 	if (cmd)
// 	{
// 		pid = fork();
// 		if (pid == 0)
// 			exec_child(f_cmd, cmd, envp);
// 		else
// 			waitpid(pid, NULL, 0);
// 	}
// 	else
// 		printf("%s: command not found\n", f_cmd->args[0]);
// }

// void	execution_part(t_shell **shell, t_list *alloc_list)
// {
// 	char	**paths;

// 	paths = get_paths(shell, alloc_list);
// 	while ((*shell)->cmds)
// 	{
// 		if (path_cmd(shell))
// 			(*shell)->cmds = (*shell)->cmds->next;
// 		else if (is_builtin(shell, alloc_list))
// 			(*shell)->cmds = (*shell)->cmds->next;
// 		else
// 		{
// 			exec_command((*shell)->cmds, paths, (*shell)->envp, alloc_list);
// 			if ((*shell)->env->next)
// 				(*shell)->cmds = (*shell)->cmds->next;
// 			else
// 				(*shell)->cmds = (*shell)->cmds->next;
// 		}
// 	}
// }

static void exec_command(t_shell *shell, char **paths, t_list **alloc_list)
{
	pid_t	pid;
	char	*cmd;
	// t_list	*shel = *()


	if (is_builtin_name(shell->cmds->args[0]))
	{
		if (shell->cmds->has_pipe)
		{
			pid = fork();
			if (pid == 0)
			{
				if (shell->cmds->infiles)
					open_all_infiles(shell->cmds->infiles);
				if (shell->cmds->outfiles)
					open_all_outfiles(shell->cmds->outfiles, shell->cmds->append_flags);

				exit(exec_builtin(&shell, (*alloc_list)));
			}
			else
				waitpid(pid, NULL, 0);
		}
		else
		{
			exec_builtin(&shell, (*alloc_list));
		}
		return;
	}

	cmd = check_cmd(paths, shell->cmds->args[0], (*alloc_list));
	if (cmd)
	{
		pid = fork();
		if (pid == 0)
			exec_child(shell, cmd);
		else
			waitpid(pid, NULL, 0);
	}
	else
		printf("%s: command not found\n", shell->cmds->args[0]);
}

// void execution_part(t_cmd *f_cmd, t_env *env_list, char **envp, t_list **alloc_list)
void execution_part(t_shell *shell, t_list **alloc_list)
{
	char **paths;

	if (!shell->cmds)
		return;

	paths = get_paths(&shell, (*alloc_list));
	while (shell->cmds)
	{
		if (shell->cmds->has_pipe)
		{
			pipex(&shell, (*alloc_list));
			while (shell->cmds && shell->cmds->has_pipe)
				shell->cmds = shell->cmds->next;
			if (shell->cmds)
				shell->cmds = shell->cmds->next;
			continue;
		}

		exec_command(shell, paths, alloc_list);
		shell->cmds = shell->cmds->next;
	}
}
