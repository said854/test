/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parce_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hakader <hakader@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 16:54:25 by sjoukni           #+#    #+#             */
/*   Updated: 2025/04/29 11:02:16 by hakader          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

t_cmd *create_cmd()
{
    t_cmd *new_cmd = malloc(sizeof(t_cmd));
    if (!new_cmd)
        return NULL;

    new_cmd->args = NULL;
    new_cmd->infile = NULL;
    new_cmd->outfile = NULL;
    new_cmd->append = 0;
    new_cmd->has_pipe = 0;
    new_cmd->next = NULL;
    new_cmd->heredoc_delim = NULL;    
    new_cmd->heredoc_expand = 0;       

    return new_cmd;
}


static int calculate_args(t_cmd *cmd)
{
    int i = 0;
    if (!cmd->args)
        return 0;
    while (cmd->args[i])
        i++;
    return i;
}
char *remove_quotes(const char *str)
{
    int i = 0, j = 0;
    int in_single_quote = 0, in_double_quote = 0;
    char *result = malloc(ft_strlen(str) + 1);
    if (!result)
        return NULL;

    while (str[i])
    {
        if (str[i] == '"' && !in_single_quote)
        {
            in_double_quote = !in_double_quote;
            i++;
        }
        else if (str[i] == '\'' && !in_double_quote)
        {
            in_single_quote = !in_single_quote;
            i++;
        }
        else if (str[i] == '\\')
        {
            if (in_single_quote)
            {
                result[j++] = str[i++];
            }
            else if (in_double_quote)
            {
                if (str[i + 1] == '"' || str[i + 1] == '\\' ||
                    str[i + 1] == '$' || str[i + 1] == '`')
                {
                    result[j++] = str[i + 1];
                    i += 2;
                }
                else
                {
                    result[j++] = str[i++];
                }
            }
            else
            {
                if (str[i + 1])
                {
                    result[j++] = str[i + 1];
                    i += 2;
                }
                else
                    break;
            }
        }
        else
        {
            result[j++] = str[i++];
        }
    }

    result[j] = '\0';
    return result;
}

int is_cmd_empty(t_cmd *cmd)
{
    return (!cmd->args && !cmd->infile && !cmd->outfile && !cmd->heredoc_delim);
}

static void add_arg_to_cmd(t_cmd *cmd, char *arg)
{
    char *cleaned = remove_quotes(arg);
    if (!cleaned)
        return;

    int old_len = calculate_args(cmd);
    char **args = malloc(sizeof(char *) * (old_len + 2));
    if (!args)
    {
        free(cleaned);
        return;
    }

    int i = 0;
    while (i < old_len)
    {
        args[i] = ft_strdup(cmd->args[i]);
        i++;
    }

    args[i++] = ft_strdup(cleaned);
    args[i] = NULL;

    if (cmd->args)
        free_array(cmd->args);
    cmd->args = args;

    free(cleaned);
}

void add_cmd_to_list(t_cmd **head, t_cmd *new_cmd)
{
    t_cmd *temp;

    if (!*head)
    {
        *head = new_cmd;
        return;
    }
    temp = *head;
    while (temp->next)
        temp = temp->next;
    temp->next = new_cmd;
}

int handle_token_redirection_or_arg(t_token **current, t_cmd *cmd)
{
    t_token *token = *current;

    if (token->type == WORD)
    {
        add_arg_to_cmd(cmd, token->value);
    }
    else if (token->type == REDIR_IN || token->type == REDIR_OUT || token->type == APPEND || token->type == HEREDOC)
    {
        if (!token->next || token->next->type != WORD)
        {
            printf("syntax error near unexpected token\n");
            return 0;
        }

        char *target = ft_strdup(token->next->value);

        if (token->type == REDIR_IN)
            cmd->infile = target;
        else if (token->type == REDIR_OUT)
        {
            cmd->outfile = target;
            cmd->append = 0;
        }
        else if (token->type == APPEND)
        {
            cmd->outfile = target;
            cmd->append = 1;
        }
        else if (token->type == HEREDOC)
        {
            cmd->heredoc_delim = remove_quotes(target);
            cmd->heredoc_expand = !is_quote(*token->next->value);
            free(target);
        }

        *current = token->next; 
    }

    return 1;
}

t_cmd *build_cmd_list(t_token *tokens)
{
    t_cmd *cmd_list = NULL;
    t_cmd *current_cmd = create_cmd();
    t_token *current = tokens;

    while (current)
    {
        if (current->type == PIPE)
        {
            current_cmd->has_pipe = 1;
            add_cmd_to_list(&cmd_list, current_cmd);
            current_cmd = create_cmd();
        }
        else if (current->type == SEMICOLON)
        {
            if (is_cmd_empty(current_cmd))
            {
                printf("minishell: syntax error near unexpected token `;'\n");
                free_cmd(current_cmd);
                free_cmd_list(cmd_list);
                return NULL;
            }
            add_cmd_to_list(&cmd_list, current_cmd);
            current_cmd = create_cmd();
        }
        else
        {
            if (!handle_token_redirection_or_arg(&current, current_cmd))
            {
                free_cmd(current_cmd);
                free_cmd_list(cmd_list);
                return NULL;
            }
        }

        current = current->next;
    }
    if (!is_cmd_empty(current_cmd))
        add_cmd_to_list(&cmd_list, current_cmd);
    else
        free_cmd(current_cmd);
    return cmd_list;
}
