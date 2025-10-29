#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int	picoshell(char *cmds[])
{
	int		i = 0;
	int		prev = -1;
	int		p[2];
	pid_t	pid;
	int		status;
	int		ret = 0;

	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(p) == -1)
			return (1);
		pid = fork();
		if (pid == -1)
		{
			if (prev != -1)
				close(prev);
			if (cmds[i + 1])
			{
				close(p[0]);
				close(p[1]);
			}
			return (1);
		}
		if (pid == 0)
		{
			if (prev != -1 && dup2(prev, 0) == -1)
				exit(1);
			if (cmds[i + 1])
			{
				close(p[0]);
				if (dup2(p[1], 1) == -1)
					exit(1);
				close(p[1]);
			}
			if (prev != -1)
				close(prev);
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (prev != -1)
			close(prev);
		if (cmds[i + 1])
		{
			close(p[1]);
			prev = p[0];
		}
		i++;
	}
	while (wait(&status) > 0)
		if (WIFEXITED(status) && WEXITSTATUS(status))
			ret = 1;
	return (ret);
}