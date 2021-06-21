/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/21 20:07:15 by iidzim            #+#    #+#             */
/*   Updated: 2021/06/21 20:24:03 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../pipex.h"

void	exec_cmd1(t_cmd *c, char **env, int pipe[2])
{
	c->fd1 = open(c->f1, O_RDONLY);
	if (c->fd1 < -1)
	{
		perror("file1: ");
		exit(1);
	}
	c->path_cmd1 = is_valid_cmd(c->cmd1, env);
	if (c->path_cmd1 == NULL)
		exit (1);
	close(pipe[0]);
	dup2(pipe[1], 1);
	dup2(c->fd1, 0);
	close(pipe[1]);
	execve(c->path_cmd1, c->cmd1, env);
	close(c->fd1);
}

void	exec_cmd2(t_cmd *c, char **env, int pipe[2])
{
	c->fd2 = open(c->f2, O_CREAT | O_RDWR, S_IRWXU);
	if (c->fd2 < -1 || access(c->f2, W_OK))
	{
		perror("file2: ");
		exit(1);
	}
	c->path_cmd2 = is_valid_cmd(c->cmd2, env);
	if (c->path_cmd2 == NULL)
		exit(127);
	close(pipe[1]);
	dup2(pipe[0], 0);
	dup2(c->fd2, 1);
	close(pipe[0]);
	execve(c->path_cmd2, c->cmd2, env);
	close(c->fd2);
}

int	main(int argc, char **argv, char **env)
{
	int		fd_pipe[2];
	int		status_ptr;
	pid_t	pid1;
	pid_t	pid2;
	t_cmd	c;

	if (argc != 5)
		print_err("pipex: syntax error\n", NULL);
	c = (t_cmd){.f1 = argv[1], .f2 = argv[4], .cmd1 = ft_split(argv[2], 32),
		.cmd2 = ft_split(argv[3], 32)};
	if (access(c.f1, F_OK) || access(c.f1, R_OK))
	{
		perror("pipex: ");
		exit(1);
	}
	if (pipe(fd_pipe) == -1)
		print_err("Pipe : fail\n", NULL);
	pid1 = fork();
	if (pid1 < 0)
		print_err("Fork : fail\n", NULL);
	else if (pid1 == 0)
		exec_cmd1(&c, env, fd_pipe);
	pid2 = fork();
	if (pid2 < 0)
		print_err("Fork : fail\n", NULL);
	else if (pid2 == 0)
		exec_cmd2(&c, env, fd_pipe);
	close(fd_pipe[0]);
	close(fd_pipe[1]);
	waitpid(pid1, &status_ptr, 0);
	waitpid(pid2, &status_ptr, 0);
	//? printf(">> %d\n", WEXITSTATUS(status_ptr));
	//? system("leaks pipex");
	if (WIFEXITED(status_ptr))
		exit(WEXITSTATUS(status_ptr));
	return (0);
}

//! ./pipex infile ls "cat -e"  file2
//? WIFEXITED → Query status to see if a child process ended normally.
//? WEXITSTATUS → Return exit status.
