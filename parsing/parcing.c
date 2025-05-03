/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <sjoukni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 13:36:31 by sjoukni           #+#    #+#             */
/*   Updated: 2025/05/03 16:19:10 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "parsing.h"

t_token *create_token(char *str, t_token_type type)
{
    t_token *new_token = malloc(sizeof(t_token));
    if (!new_token)
        return NULL;
    new_token->value = ft_strdup(str);
    new_token->type = type;
    new_token->next = NULL;
    return new_token;
}

void append_token(t_token **head, t_token *new)
{
    if (!*head)
    {
        *head = new;
        return;
    }
    t_token *temp = *head;
    while (temp->next)
        temp = temp->next;
    temp->next = new;
}

int get_token_length(char *line, int i)
{
    int start;
    int in_single_quote = 0;
    int in_double_quote = 0;

    while (line[i] && ft_isspace(line[i]))
        i++;
    start = i;

    if (line[i] == ';')
    {
        if (line[i + 1] == ';')
            return -2;
        return 1;
    }
    if (is_operator(line[i]))
    {
        if (is_operator(line[i + 1]) && line[i] == line[i + 1] && line[i] != '|')
            return 2;
        return 1;
    }

    while (line[i])
    {
        if (line[i] == '`')
            return -4;
        if (line[i] == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote, i++;
        else if (line[i] == '"' && !in_single_quote)
            in_double_quote = !in_double_quote, i++;
        else if (line[i] == '\\')
        {
            if (!line[i + 1])
                return -3;
            i += 2;
        }
        else
        {
            if (!in_single_quote && !in_double_quote &&
                (line[i] == ';' || ft_isspace(line[i]) || is_operator(line[i])))
                break;
            i++;
        }
    }
    if (in_single_quote || in_double_quote)
        return -1;
    return i - start;
}

t_token_type get_token_type(char *str)
{
    if (!strcmp(str, "|")) return PIPE;
    if (!strcmp(str, ">")) return REDIR_OUT;
    if (!strcmp(str, ">>")) return APPEND;
    if (!strcmp(str, "<")) return REDIR_IN;
    if (!strcmp(str, "<<")) return HEREDOC;
    if (!strcmp(str, ";")) return SEMICOLON;
    return WORD;
}

t_token *tokenize_line(char *line, t_env *env, int last_exit_status)
{
    int i = 0, len;
    t_token *head = NULL;
    char *token_str;
    t_token_type type;

    while (line[i])
    {
        while (ft_isspace(line[i]))
            i++;
        if (!line[i])
            break;

        len = get_token_length(line, i);
        if (len < 0)
        {
            if (len == -1)
                printf("syntax error: unclosed quote\n");
            else if (len == -2)
                printf("syntax error near `;;'\n");
            else if (len == -3)
                printf("syntax error near `\\'\n");
            else if (len == -4)
                printf("syntax error near ``'\n");
            free_token_list(head);
            return NULL;
        }

        token_str = strndup(line + i, len);
        type = get_token_type(token_str);
        
        if (type == WORD && ft_strchr(token_str, '$'))
        {
            char *expanded = expand_token_value(token_str, env, last_exit_status);
            append_token(&head, create_token(expanded, type));
            free(expanded);
        }
        else
        {
            append_token(&head, create_token(token_str, type));
        }
        

        free(token_str);
        i += len;
    }
    return head;
}


