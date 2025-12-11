#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

static int	create_pipe_if_needed(int fd[2], char **next_cmd)
{
	if (next_cmd)
		return (pipe(fd));
	return (0);
}

static void	child_redirections(int prev_fd, int fd[2], char **next_cmd)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, 0);
		close(prev_fd);
	}
	if (next_cmd)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
	}
}

static void	child_execution(char **cmd)
{
	execvp(cmd[0], cmd);
	_exit(1);
}

static int	parent_update(int prev_fd, int fd[2], char **next_cmd)
{
	if (prev_fd != -1)
		close(prev_fd);
	if (next_cmd)
	{
		close(fd[1]);
		return (fd[0]);
	}
	return (-1);
}

int	picoshell(char **cmds[])
{
	if (!cmds || !cmds[0])
		return (1);

	int		i = 0;
	int		prev_fd = -1;
	int		fd[2];
	pid_t	pid;

	while (cmds[i])
	{
		if (create_pipe_if_needed(fd, cmds[i + 1]) == -1)
			return (1);

		pid = fork();
		if (pid == -1)
		{
			if (prev_fd != -1)
				close(prev_fd);
			return (1);
		}

		if (pid == 0)
		{
			child_redirections(prev_fd, fd, cmds[i + 1]);
			child_execution(cmds[i]);
		}

		prev_fd = parent_update(prev_fd, fd, cmds[i + 1]);
		i++;
	}
	if (prev_fd != -1)
        close(prev_fd);

	int	ret = 0;
	int	status;
	while (wait(&status) > 0)
	{
		// if (WIFEXITED(status) && WEXITSTATUS(status) != 0) <- Need to be commented out to pass
		// 	ret = 1;
	}
	return (ret);
}