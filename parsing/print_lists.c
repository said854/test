/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_lists.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <sjoukni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 19:13:18 by sjoukni           #+#    #+#             */
/*   Updated: 2025/04/20 16:31:03 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.h"

void print_list_env(t_env **head)
{
    t_env *tmp = *head;

    while (tmp)
    {
        printf("key: %s | value: %s\n", tmp->key, tmp->value);
        tmp = tmp->next;
    }
}

void print_cmd_list(t_cmd *cmd_list)
{
    int i;
    int cmd_num = 1;

    while (cmd_list)
    {
        printf("🔹 CMD %d:\n", cmd_num++);

        // Print args
        printf("  args    = ");
        if (cmd_list->args)
        {
            i = 0;
            while (cmd_list->args[i])
                printf("\"%s\" ", cmd_list->args[i++]);
        }
        else
            printf("(none)");
        printf("\n");

        // Print infiles
        printf("  infiles  = ");
        if (cmd_list->infiles)
        {
            i = 0;
            while (cmd_list->infiles[i])
                printf("\"%s\" ", cmd_list->infiles[i++]);
        }
        else
            printf("(none)");
        printf("\n");

        // Print outfiles and append flags
        printf("  outfiles = ");
        if (cmd_list->outfiles)
        {
            i = 0;
            while (cmd_list->outfiles[i])
                printf("\"%s\" ", cmd_list->outfiles[i++]);
        }
        else
            printf("(none)");
        printf("\n");

        printf("  append flags = ");
        if (cmd_list->append_flags)
        {
            i = 0;
            while (cmd_list->append_flags[i] || (cmd_list->outfiles && cmd_list->outfiles[i]))
                printf("%d ", cmd_list->append_flags[i++]);
        }
        else
            printf("(none)");
        printf("\n");

        // Other fields
        printf("  pipe    = %s\n", cmd_list->has_pipe ? "true" : "false");
        printf("  heredoc = %s\n", cmd_list->heredoc_delim ? cmd_list->heredoc_delim : "(null)");
        printf("  heredoc expand = %d\n", cmd_list->heredoc_expand);
        printf("\n");

        cmd_list = cmd_list->next;
    }
}


void print_list(t_token *head)
{
	t_token *tmp = head;

	while (tmp)
	{
		printf("value: %s || type: %d\n", tmp->value, tmp->type);
		tmp = tmp->next;
	}
}