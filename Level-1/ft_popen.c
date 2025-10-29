#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];
	pid_t	pid;

	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	if (pid == 0) // Fils
	{
		if (type == 'r')
		{
			close(fd[0]);
			if (dup2(fd[1], 1) == -1)
			{
				close(fd[1]);
				exit(1);
			}
			close(fd[1]);
		}
		else
		{
			close(fd[1]);
			if (dup2(fd[0], 0) == -1)
			{
				close(fd[0]);
				exit(1);
			}
			close(fd[0]);
		}
		execvp(file, argv);
		exit (1);
	}
	if (type == 'r') // Parent
	{
		close(fd[1]);
		return (fd[0]);
	}
	else
	{
		close(fd[0]);
		return (fd[1]);
	}
}