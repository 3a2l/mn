#include <mn/Thread.h>
#include <mn/Defer.h>
#include <mn/Fabric.h>

int main()
{
	auto f = mn::fabric_new({});
	mn_defer{mn::fabric_free(f);};

	auto mtx1 = mn::mutex_new("mtx1");
	mn_defer{mn::mutex_free(mtx1);};

	auto mtx2 = mn::mutex_new("mtx2");
	mn_defer{mn::mutex_free(mtx2);};

	mn::Auto_Waitgroup wg;
	wg.add(2);

	mn::go(f, [&]{
		mn::mutex_lock(mtx1);
		mn_defer{mn::mutex_unlock(mtx1);};

		mn::thread_sleep(500);

		mn::mutex_lock(mtx2);
		mn::mutex_unlock(mtx2);

		wg.done();
	});

	mn::go(f, [&]{
		mn::mutex_lock(mtx2);
		mn_defer{mn::mutex_unlock(mtx2);};

		mn::thread_sleep(500);

		mn::mutex_lock(mtx1);
		mn::mutex_unlock(mtx1);

		wg.done();
	});

	wg.wait();

	return 0;
}