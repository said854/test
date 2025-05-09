/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hakader <hakader@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 16:00:37 by sjoukni           #+#    #+#             */
/*   Updated: 2025/05/07 11:13:18 by hakader          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution.h"

// static void	exec_first_child(t_cmd *cmd, int *pipe_fd, char **envp)
// {
// 	int	fd_in;

// 	set_child_signals();
// 	if (cmd->infiles)
// 	{
// 		fd_in = open(cmd->infiles, O_RDONLY);
// 		if (fd_in < 0)
// 		{
// 			perror("open infiles");
// 			exit(1);
// 		}
// 		dup2(fd_in, STDIN_FILENO);
// 		close(fd_in);
// 	}
// 	dup2(pipe_fd[1], STDOUT_FILENO);
// 	close(pipe_fd[0]);
// 	close(pipe_fd[1]);
// 	execve(cmd->args[0], cmd->args, envp);
// 	perror("execve");
// 	exit(1);
// }

// static void	exec_second_child(t_cmd *next_cmd, int *pipe_fd, char **envp)
// {
// 	int (fd_out), (flags);
// 	set_child_signals();
// 	dup2(pipe_fd[0], STDIN_FILENO);
// 	close(pipe_fd[1]);
// 	close(pipe_fd[0]);
// 	if (next_cmd->outfiles)
// 	{
// 		flags = O_WRONLY | O_CREAT;
// 		if (next_cmd->append)
// 			flags |= O_APPEND;
// 		else
// 			flags |= O_TRUNC;
// 		fd_out = open(next_cmd->outfiles, flags, 0644);
// 		if (fd_out < 0)
// 		{
// 			perror("open outfiles");
// 			exit(1);
// 		}
// 		dup2(fd_out, STDOUT_FILENO);
// 		close(fd_out);
// 	}
// 	execve(next_cmd->args[0], next_cmd->args, envp);
// 	perror("execve");
// 	exit(1);
// }

// void	pipex(t_cmd *cmd, char **envp)
// {
// 	int		pipe_fd[2];
// 	pid_t	pid1;
// 	pid_t	pid2;

// 	if (pipe(pipe_fd) == -1)
// 	{
// 		perror("pipe");
// 		return ;
// 	}
// 	pid1 = fork();
// 	if (pid1 == 0)
// 		exec_first_child(cmd, pipe_fd, envp);
// 	pid2 = fork();
// 	if (pid2 == 0)
// 		exec_second_child(cmd->next, pipe_fd, envp);
// 	close(pipe_fd[0]);
// 	close(pipe_fd[1]);
// 	waitpid(pid1, NULL, 0);
// 	waitpid(pid2, NULL, 0);
// }

#include <sys/wait.h>

void handle_redirections(t_cmd *cmd)
{
	int fd = -1;
	int i = 0;

	// Handle input redirections
	while (cmd->infiles && cmd->infiles[i])
	{
		fd = open(cmd->infiles[i], O_RDONLY);
		if (fd < 0)
		{
			perror(cmd->infiles[i]);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	if (fd != -1)
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}

	// Handle output redirections
	fd = -1;
	i = 0;
	while (cmd->outfiles && cmd->outfiles[i])
	{
		int flags = O_WRONLY | O_CREAT;
		if (cmd->append_flags && cmd->append_flags[i] == 1)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;

		fd = open(cmd->outfiles[i], flags, 0644);
		if (fd < 0)
		{
			perror(cmd->outfiles[i]);
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


// void exec_pipeline_cmd(t_cmd *cmd, t_env **env_list, char **envp, char **paths, int in_fd, int out_fd)
void exec_pipeline_cmd(t_shell *shell, char **paths, int in_fd,
						int out_fd, t_list *alloc_list)
{
	char *cmd_path;

	set_child_signals();
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	handle_redirections(shell->cmds);

	if (!shell->cmds->args || !shell->cmds->args[0])
		exit(EXIT_SUCCESS);

	if (is_builtin_name(shell->cmds->args[0]))
		exit(exec_builtin(&shell, alloc_list));

	cmd_path = check_cmd(paths, shell->cmds->args[0], alloc_list);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: command not found\n", shell->cmds->args[0]);
		exit(127);
	}
	execve(cmd_path, shell->cmds->args, shell->envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

// void pipex(t_cmd *cmd, t_env *env_list, char **envp)
void pipex(t_shell **shell, t_list *alloc_list)
{
	int		pipe_fd[2];
	int		in_fd = dup(STDIN_FILENO);
	int		prev_fd = in_fd;
	pid_t	pid;
	char	**paths = get_paths(shell, alloc_list);
	t_cmd	*current = (*shell)->cmds;

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
			exec_pipeline_cmd((*shell), paths, prev_fd, pipe_fd[1], alloc_list);
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
			 exec_pipeline_cmd((*shell), paths, prev_fd, STDOUT_FILENO, alloc_list);
		if (prev_fd != STDIN_FILENO)
			close(prev_fd);
	}
	// int status, exit_code;
	// waitpid(pid, &status, 0);
	// if (WIFEXITED)
	// 	exit_code = WEXITSTATUS(status);
	while (wait(NULL) > 0)
		;
	// free_array(paths);
}