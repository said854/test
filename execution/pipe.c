/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <sjoukni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 16:00:37 by sjoukni           #+#    #+#             */
/*   Updated: 2025/05/05 16:56:32 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution.h"
#include <sys/wait.h>

void handle_redirections(t_cmd *cmd)
{
	int	fd;
	int	flags;

	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		if (fd < 0)
		{
			perror(cmd->infile);
			exit(EXIT_FAILURE);
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
			perror(cmd->outfile);
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}	// int status, exit_code;
	// waitpid(pid, &status, 0);
	// if (WIFEXITED)
	// 	exit_code = WEXITSTATUS(status);
}

void exec_pipeline_cmd(t_cmd *cmd, t_env **env_list, char **envp, char **paths, int in_fd, int out_fd)
{
	char *cmd_path;

	set_child_signals();
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	handle_redirections(cmd);

	if (!cmd->args || !cmd->args[0])
		exit(EXIT_SUCCESS);

	if (is_builtin_name(cmd->args[0]))
		exit(exec_builtin(cmd, env_list));

	cmd_path = check_cmd(paths, cmd->args[0]);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: command not found\n", cmd->args[0]);
		exit(127);
	}
	execve(cmd_path, cmd->args, envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

void pipex(t_cmd *cmd, t_env *env_list, char **envp)
{
	int		pipe_fd[2];
	int		in_fd = dup(STDIN_FILENO);
	int		prev_fd = in_fd;
	pid_t	pid;
	char	**paths = get_paths(env_list);
	t_cmd	*current = cmd;

	while (current && current->has_pipe)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("pipe");
			exit(EXIT_FAILURE);
		}
		pid = fork();
		if (pid == 0)
		{
			close(pipe_fd[0]);
			exec_pipeline_cmd(current, &env_list, envp, paths, prev_fd, pipe_fd[1]);
		}
		close(pipe_fd[1]);
		if (prev_fd != STDIN_FILENO)
			close(prev_fd);
		prev_fd = pipe_fd[0];
		current = current->next;
	}

	// Last command in pipeline
	if (current)
	{
		pid = fork();
		if (pid == 0)
			 exec_pipeline_cmd(current, &env_list, envp, paths, prev_fd, STDOUT_FILENO);
		if (prev_fd != STDIN_FILENO)
			close(prev_fd);
	}
	// int status, exit_code;
	// waitpid(pid, &status, 0);
	// if (WIFEXITED)
	// 	exit_code = WEXITSTATUS(status);
	while (wait(NULL) > 0)
		;
	free_array(paths);
}
