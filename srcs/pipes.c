/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olydden <olydden@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/05 19:49:38 by ctobias           #+#    #+#             */
/*   Updated: 2021/03/10 16:28:37 by olydden          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		pipe_count(t_list *commands)
{
	int			i;
	t_command	*command;

	i = 0;
	command = commands->content;
	while (commands && command->jmp_type == 1)
	{
		++i;
		commands = commands->next;
		command = commands->content;
	}
	return (i);
}

void	pipe_first(t_list *commands, int *pipe_fd)
{
	t_command *command;

	command = commands->content;
	parse_args(command->args);
	close(pipe_fd[1]);
	waitpid(-1, &g_status, 0);
	if (WIFSIGNALED(g_status))
		g_status = WTERMSIG(g_status) + 128;
	else
		g_status = WEXITSTATUS(g_status);
	if (ft_strchr("23", command->jmp_type + '0'))
	{
		dup2(pipe_fd[0], 0);
		right_redir_handler(commands, pipe_fd, 1);
	}
	else if (command->jmp_type != 4)
	{
		dup2(pipe_fd[0], 0);
		execute_in(command);
	}
}

void	pipe_in(t_command *command, int *pipe_fd, int *pipe_fd_new)
{
	parse_args(command->args);
	close(pipe_fd[0]);
	close(pipe_fd_new[1]);
	waitpid(-1, &g_status, 0);
	if (WIFSIGNALED(g_status))
		g_status = WTERMSIG(g_status) + 128;
	else
		g_status = WEXITSTATUS(g_status);
	dup2(pipe_fd_new[0], 0);
	dup2(pipe_fd[1], 1);
	exit(execute_in(command));
}

void	pipe_last(t_list *commands, int *pipe_fd)
{
	t_command *command;

	command = commands->content;
	close(pipe_fd[0]);
	dup2(pipe_fd[1], 1);
	exit(execute_in(command));
}

int		my_pipe(t_list *commands, int count, int i, int *pipe_fd)
{
	int		pid;
	int		pipe_fd_new[2];

	if (i == 0)
	{
		pipe(pipe_fd_new);
		pid = fork();
		if (pid > 0)
			pipe_first(ft_lstindex(commands, count), pipe_fd_new);
		else
			my_pipe(commands, count, ++i, pipe_fd_new);
	}
	else if (i == count)
		pipe_last(commands, pipe_fd);
	else
	{
		pipe(pipe_fd_new);
		pid = fork();
		if (pid > 0)
			pipe_in(ft_lstindex(commands, count - i)->content,
					pipe_fd, pipe_fd_new);
		else
			my_pipe(commands, count, ++i, pipe_fd_new);
	}
	return (0);
}
