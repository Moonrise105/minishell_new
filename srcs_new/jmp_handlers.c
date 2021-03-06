/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jmp_handlers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctobias <ctobias@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/05 20:02:51 by ctobias           #+#    #+#             */
/*   Updated: 2021/03/06 16:54:09 by ctobias          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			semicolon_handler(t_command *command)
{
	int		code;

	code = execute_handler(command);
	return (code);
}

t_list		*pipe_handler(t_list *commands)
{
	int pid;
	int count;
	int *pipe_fd;

	count = pipe_count(commands);
	pid = fork();
	if (pid > 0)
	{
		signal(SIGINT, sig_int);
		waitpid(-1, &g_status, 0);
		if (WIFSIGNALED(g_status))
			g_status = WTERMSIG(g_status) + 128;
		else
			g_status = WEXITSTATUS(g_status);
		return (skip_jmps(commands, "123"));
	}
	else if (pid == 0)
		my_pipe(commands, count, 0, pipe_fd);
	return (NULL);
}

t_list		*right_redir_handler(t_list *commands, int *pipe_fd, int prev_jmp)
{
	t_command	*command;
	t_command	*first_cmd;
	int			fd;
	t_command	*prev_command;
	t_list		*prev_commands;

	command = commands->content;
	first_cmd = command;
	commands = skip_jmps(commands, "4");
	prev_commands = commands;
	commands = commands->next;
	command = commands->content;
	while (commands)
	{
		if (right_redir(pipe_fd, prev_jmp, first_cmd, prev_commands))
			return (commands);
		prev_commands = commands;
		prev_command = command;
		commands = commands->next;
		command = commands->content;
	}
	return (NULL);
}

t_list		*left_redir_handler(t_list *commands)
{
	t_command	*command;
	t_list		*first_commands;
	int			err;
	int			fd;
	int			dup_fd[2];

	commands = left_redir(commands, &err, &first_commands);
	command = commands->content;
	if (err)
		return (commands);
	dup_fd[0] = open(command->args->content, O_RDONLY);
	dup_fd[1] = 0;
	if (ft_strchr("23", command->jmp_type + '0'))
		commands = right_redir_handler(first_commands, dup_fd, 4);
	else
		fork_exec(first_commands->content, dup_fd);
	close(dup_fd[0]);
	return (commands);
}
