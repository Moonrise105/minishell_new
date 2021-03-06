/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moonrise <moonrise@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/05 19:59:55 by ctobias           #+#    #+#             */
/*   Updated: 2021/03/09 18:30:53 by moonrise         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int g_status = 0;

void		shell_newline(void)
{
	ft_putstr("minishell% ");
}

int			read_command(t_parser *parser_struct)
{
	char	input;
	int		read_bytes;
	int		code;

	input = '\0';
	while (1)
	{
		signal(SIGINT, signal_interrupt);
		signal(SIGQUIT, signal_quit);
		read_bytes = read(0, &input, 1);
		if (!read_bytes && input)
			eof_ign();
		else
		{
			code = parser(parser_struct, input);
			if (read_bytes && !code)
				return (READ_END);
			else if (!read_bytes && !input)
				return (READ_EXIT);
		}
	}
}

void		shell(char **env)
{
	t_list		*dict;
	t_parser	parser_struct;

	dict = dict_create(env, '=');
	env_get(&dict);
	while (1)
	{
		parser_null(&parser_struct);
		shell_newline();
		if (read_command(&parser_struct) == READ_EXIT)
		{
			dict_free(&dict);
			commands_free(&parser_struct.commands);
			my_exit(NULL);
		}
		if (parser_check(parser_struct.commands) >= 0)
			manager(parser_struct.commands);
		commands_free(&(parser_struct.commands));
	}
}

int			main(int ac, char **av, char **en)
{
	if (ac > 0 && av)
		shell(en);
	return (0);
}
