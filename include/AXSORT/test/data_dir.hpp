#pragma once
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem.hpp>

namespace axsort {namespace test {

boost::filesystem::path get_executable_path()
{
    return boost::dll::program_location();
}
boost::filesystem::path get_executable_dir()
{
    return get_executable_path().parent_path();
}
auto& data_dir()
{
    static boost::filesystem::path dd( 
        get_executable_dir() / ".." / "unit_test" / "data" 
    );
    return dd;
}

}}
