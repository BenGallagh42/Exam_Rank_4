#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

static pid_t	child;

void	handler(int s)
{
	(void)s;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa = {0};
	pid_t				pid;
	int					status;

	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		return (-1);

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}

	child = pid;
	alarm(t);

	if (waitpid(pid, &status, 0) == -1)
	{
		if (errno == EINTR)
		{
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			if (v)
				printf("Bad function: timed out after %u seconds\n", t);
			return (0);
		}
		return (-1);
	}

	alarm(0);
	
	if (WIFEXITED(status))
	{
		int code = WEXITSTATUS(status);
		if (code == 0)
		{
			if (v)
				printf("Nice function!\n");
			return (1);
		}
		if (v)
			printf("Bad function: exited with code %d\n", code);
		return (0);
	}

	if (WIFSIGNALED(status))
	{
		if (v)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		return (0);
	}
	return (-1);
}