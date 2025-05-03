/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <sjoukni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 16:00:37 by sjoukni           #+#    #+#             */
/*   Updated: 2025/05/03 17:45:07 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution.h"

void	handle_redirections(t_cmd *cmd)
{
	int	fd;
	int	flags;

	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		if (fd < 0)
		{
			perror("open infile");
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if (cmd->outfile)
	{
		flags = O_WRONLY | O_CREAT;
		flags |= cmd->append ? O_APPEND : O_TRUNC;

		fd = open(cmd->outfile, flags, 0644);
		if (fd < 0)
		{
			perror("open outfile");
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
}

void	exec_pipeline_cmd(t_cmd *cmd, char **envp, char **paths, int in_fd, int out_fd)
{
	char	*cmd_path;

	set_child_signals();
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	handle_redirections(cmd);

	if (!cmd->args || !cmd->args[0])
		exit(0);

	if (is_builtin_name(cmd->args[0]))
	{
		exec_builtin(cmd, NULL); 
		exit(0);
	}

	cmd_path = check_cmd(paths, cmd->args[0]);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: command not found\n", cmd->args[0]);
		exit(127);
	}
	execve(cmd_path, cmd->args, envp);
	perror("execve");
	exit(1);
}

void	pipex(t_cmd *cmd, t_env *env_list, char **envp)
{
	int		pipe_fd[2];
	int		in_fd = STDIN_FILENO;
	pid_t	pid;
	char	**paths = get_paths(env_list);

	while (cmd && cmd->has_pipe)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("pipe");
			exit(1);
		}
		pid = fork();
		if (pid == 0)
		{
			close(pipe_fd[0]);
			exec_pipeline_cmd(cmd, envp, paths, in_fd, pipe_fd[1]);
		}
		close(pipe_fd[1]);
		if (in_fd != STDIN_FILENO)
			close(in_fd);
		in_fd = pipe_fd[0];
		cmd = cmd->next;
	}

	if (cmd)
	{
		pid = fork();
		if (pid == 0)
			exec_pipeline_cmd(cmd, envp, paths, in_fd, STDOUT_FILENO);
		if (in_fd != STDIN_FILENO)
			close(in_fd);
	}

	while (wait(NULL) > 0) ;
	// free_array(paths);
}



