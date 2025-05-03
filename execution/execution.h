/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sjoukni <sjoukni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 09:49:02 by hakader           #+#    #+#             */
/*   Updated: 2025/05/03 17:39:24 by sjoukni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTION_H
# define EXECUTION_H

# include "../minishell.h"
# include "../libft/libft.h"

/* Execution Core */
void	execution_part(t_cmd *cmd_list, t_env *env_list, char **envp);
void	print_arr(char **arr);
void	put_error(char *msg);
void	mini_display(void);
void	print_this(t_env **env_list, char *sch);

/* String Utilities */
char	**ft_split(char const *s, char c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
int		ft_strcmp(const char *str1, const char *str2);

/* Command Path Resolution */
char	**get_paths(t_env *env_list);
char	*check_cmd(char **paths, char *cmd);
int		count_args(char **args);

/* Built-in Commands */
int	    is_builtin_name(const char *name);
int	    exec_builtin(t_cmd *cmd, t_env **envp);
int		execute_echo(t_cmd *cmd);
int		execute_pwd(t_cmd *cmd);
int		execute_cd(t_cmd *cmd, t_env **env_list);
void	update_env(t_env **env, char *key, char *replace);
int 	execute_env(t_cmd *cmd, t_env *envp);
int	execute_exit(t_cmd *cmd);

/* Export */
// int		execute_export(t_cmd *cmd, t_env **envp);
// int		is_valid_key(const char *str);
// t_env	*find_env(t_env *env, const char *key);
// char	*extract_key(const char *str);
// char	*extract_value(const char *str);
// void	add_or_update_env(t_env **env, char *arg);
int execute_export(t_cmd *cmd, t_env **env);
t_env *ft_new_env(char *key, char *value);

/* Unset */
// int     execute_unset(t_cmd *cmd, t_env **envp);
// void	remove_env_var(t_env **envp, const char *key);
int	excute_unset(t_cmd *cmd, t_env **env);
void ft_envadd_back(t_env **env, char *key, char *value);
int	check_exp(t_cmd *cmd, t_env **env);

/* Built-in Helpers */
int		is_new_line(char *arg);
void	env_path(t_env **env_list, t_cmd *cmd);

/* Memory Management */
void	*ft_malloc(size_t size, t_list *alloc_list);
void	free_all(t_list *alloc_list);

/* Redirections */
void	outfile(const char *filename);
void	infile(const char *filename);

/* Echo Helpers */
int		open_and_write(t_cmd *cmd, int flag, int index);
void	pipex(t_cmd *cmd, t_env *env_list, char **envp);


#endif
