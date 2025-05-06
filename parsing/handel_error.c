/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handel_error.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <sjoukni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:46:03 by sjoukni           #+#    #+#             */
/*   Updated: 2025/05/06 11:52:28 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

int is_redirection(t_token_type type)
{
    return (type == REDIR_OUT || type == REDIR_IN || type == APPEND || type == HEREDOC);
}
int  print_error(char *message)
{
    printf("%s\n", message);
    return 0;
}

int check_syntax(t_token *token_list)
{
    t_token *current = token_list;
    t_token *prev = NULL;

    if (!current)
        return 0;
    if (current->type == PIPE)
        return print_error("syntax error near unexpected token `|`");
    while (current)
    {
        if (prev && prev->type == PIPE && current->type == PIPE)
            return print_error("syntax error near unexpected token `|`");

        if (is_redirection(current->type))
        {
            if (!current->next || current->next->type != WORD)
                return print_error("syntax error: missing filename after redirection");
        }

        prev = current;
        current = current->next;
    }
    if (prev && prev->type == PIPE)
        return print_error("syntax error: unexpected end of input after `|`");
    
    return 1;
}

