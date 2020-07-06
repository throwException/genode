
// Genode includes
#include <base/component.h>
#include <base/heap.h>
#include <util/string.h>
#include <base/attached_rom_dataspace.h>
#include <os/vfs.h>

namespace Test {

	using namespace Genode;

	class Main;
}


struct Test::Main
{
	Env&                          _env;
	Heap                          _heap   { _env.ram(), _env.rm() };
	Attached_rom_dataspace        _config { _env, "config" };
	Root_directory                _root_dir { _env, _heap, _config.xml().sub_node("vfs") };
	Directory                     _base_dir { _root_dir, "dir/data" };
	Genode::uint64_t              _dummy = 0;

	void _read_file(Directory& dir, String<128> name)
	{
		try
		{
			File_content file(_heap, dir, name, File_content::Limit{4096});
			file.bytes([&] (const char* b, size_t size) {
				_dummy += b[0] + size;
			});
		}
		catch (Genode::Directory::Nonexistent_file)
		{
		}
		catch (Genode::File::Open_failed)
		{
		}
	}

	Main(Env &env) : _env(env)
	{
		while (true) {
			_read_file(_base_dir, "file");
		}
	}
};


void Component::construct(Genode::Env& env)
{
	static Test::Main main(env);
}
