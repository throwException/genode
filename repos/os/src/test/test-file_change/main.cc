
// Genode includes
#include <libc/component.h>
#include <base/heap.h>
#include <util/string.h>

// libc includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <dirent.h>

namespace Test {

	class Main;

	using Genode::Env;
	using Genode::Heap;
	using Genode::error;
	using Genode::log;

	using File_name = Genode::String<64>;
	using Data = Genode::String<1024>;
}


struct Test::Main
{
	Env&                         _env;
	Heap                         _heap   { _env.ram(), _env.rm() };

	void delete_file(File_name name)
	{
		Libc::with_libc([&] () {
			remove(name.string());
		});
	}

	bool modify_file(File_name name, Data data, int flags)
	{
		bool result = false;

		Libc::with_libc([&] () {

			int fd = open(name.string(), flags);
			if (fd < 0) {
				error("couldn't open ", name, " for writing");
				return;
			}

			auto written = write(fd, data.string(), data.size());

			if (written < 0) {
				error("writing to ", name, " failed");
				return;
			}

			close(fd);
			result = true;
		});

		return result;
	}

	void test()
	{
		while (true) {
			delete_file("/dir/file");
			modify_file("/dir/file", "blubb", O_CREAT | O_RDWR);
		}
	}

	Main(Env &env) : _env(env)
	{
		test();
	}
};


void Libc::Component::construct(Libc::Env& env)
{
	static Test::Main main(env);
}
