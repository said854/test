/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hakader <hakader@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 09:49:04 by hakader           #+#    #+#             */
/*   Updated: 2025/04/29 13:17:41 by hakader          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution.h"

void expand_args_in_cmd(t_cmd *cmd, t_env *env, int last_exit)
{
    int i = 0;
    while (cmd->args && cmd->args[i])
    {
        if (ft_strchr(cmd->args[i], '$'))
        {
            char *expanded = expand_token_value(cmd->args[i], env, last_exit);
            free(cmd->args[i]);
            cmd->args[i] = expanded;
        }
        i++;
    }
}

int get_last_exit_status(int status)
{
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        return 128 + WTERMSIG(status);
    else
        return 1;
}


#include "execution.h"

int path_cmd(t_cmd *f_cmd, char **envp, t_shell *shell)
{
    pid_t pid;
    int status;

    if (f_cmd->args[0][0] == '/')
    {
        if (access(f_cmd->args[0], X_OK) == 0)
        {
            pid = fork();
            if (pid == 0)
            {
                execve(f_cmd->args[0], &f_cmd->args[0], envp);
                exit(127);
            }
            else
            {
                waitpid(pid, &status, 0);
                shell->last_exit_status = get_last_exit_status(status);
            }
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
    exit(127);
}

static void exec_command(t_cmd *f_cmd, char **paths, char **envp, t_shell *shell)
{
    pid_t pid;
    int status;
    char *cmd;

    cmd = check_cmd(paths, f_cmd->args[0]);
    if (cmd)
    {
        pid = fork();
        if (pid == 0)
            exec_child(f_cmd, cmd, envp);
        else
        {
            waitpid(pid, &status, 0);
            shell->last_exit_status = get_last_exit_status(status);
        }
        free(cmd);
    }
    else
    {
        printf("%s: command not found\n", f_cmd->args[0]);
        shell->last_exit_status = 127;
    }
}

void execution_part(t_cmd *f_cmd, t_env *env_list, char **envp, t_shell *shell)
{
    char **paths;
    t_cmd *head = f_cmd;

    if (path_cmd(f_cmd, envp, shell))
    {
        free_cmd_list(head);
        return;
    }

    paths = get_paths(env_list);
    while (f_cmd)
    {
        expand_args_in_cmd(f_cmd, shell->env, shell->last_exit_status);

        if (is_builtin(f_cmd, env_list))
        {
            shell->last_exit_status = 0;
            break;
        }

        exec_command(f_cmd, paths, envp, shell);
        f_cmd = f_cmd->next;
    }
    free_cmd_list(head);
}
