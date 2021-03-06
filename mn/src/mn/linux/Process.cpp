#include "mn/Process.h"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

namespace mn
{
	Process
	process_id()
	{
		return Process{ getpid() };
	}

	Process
	process_parent_id()
	{
		return Process{ getppid() };
	}

	bool
	process_kill(Process p)
	{
		return kill(p.id, SIGTERM) == 0;
	}

	bool
	process_alive(Process p)
	{
		return kill(p.id, 0) == 0;
	}
}