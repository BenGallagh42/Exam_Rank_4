#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

static void	child_read_mode(int fd[2], char *const argv[])
{
	close(fd[0]);
	dup2(fd[1], 1);
	close(fd[1]);
	execvp(argv[0], argv);
	exit(1);
}

static void	child_write_mode(int fd[2], char *const argv[])
{
	close(fd[1]);
	dup2(fd[0], 0);
	close(fd[0]);
	execvp(argv[0], argv);
	exit(1);
}

static int	parent_read_mode(int fd[2])
{
	close(fd[1]);
	return (fd[0]);
}

static int	parent_write_mode(int fd[2])
{
	close(fd[0]);
	return (fd[1]);
}

int	ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];
	pid_t	pid;

	// 1. Vérifications
	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);

	// 2. Pipe
	if (pipe(fd) == -1)
		return (-1);

	// 3. Fork
	pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}

	// 4. LE FILS
	if (pid == 0)
	{
		if (type == 'r')
			child_read_mode(fd, argv);
		else
			child_write_mode(fd, argv);
	}

	// 5. LE PARENT
	if (type == 'r')
		return (parent_read_mode(fd));
	else
		return (parent_write_mode(fd));
}