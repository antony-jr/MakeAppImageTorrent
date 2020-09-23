#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/create_torrent.hpp>

#include <functional>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Trackers.hpp"

#ifndef GIT_COMMIT
#define GIT_COMMIT "none"
#endif

#ifndef BUILD_TIME
#define BUILD_TIME "Unknown Date/Time"
#endif

namespace {

using namespace std::placeholders;

std::vector<char> load_file(std::string const& filename)
{
  std::fstream in;
  in.exceptions(std::ifstream::failbit);
  in.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
  in.seekg(0, std::ios_base::end);
  size_t const size = size_t(in.tellg());
  in.seekg(0, std::ios_base::beg);
  std::vector<char> ret(size);
  in.read(ret.data(), int(ret.size()));
  return ret;
}

std::string branch_path(std::string const& f)
{
  if (f.empty()) return f;
  
  if (f == "/") return "";

  auto len = f.size();
  // if the last character is / or \ ignore it
  if (f[len-1] == '/' || f[len-1] == '\\') --len;
  while (len > 0) {
    --len;
    if (f[len] == '/' || f[len] == '\\')
      break;
  }

  if (f[len] == '/' || f[len] == '\\') ++len;
  return std::string(f.c_str(), len);
}

bool file_filter(std::string const& f)
{
  if (f.empty()) return false;

  char const* first = f.c_str();
  char const* sep = strrchr(first, '/');
#if defined(TORRENT_WINDOWS) || defined(TORRENT_OS2)
  char const* altsep = strrchr(first, '\\');
  if (sep == nullptr || altsep > sep) sep = altsep;
#endif
  // if there is no parent path, just set 'sep'
  // to point to the filename.
  // if there is a parent path, skip the '/' character
  if (sep == nullptr) sep = first;
  else ++sep;

  // return false if the first character of the filename is a .
  if (sep[0] == '.') return false;

  std::cerr << f << "\n";
  return true;
}
} // anonymous namespace

int main(int ac, char **av) try
{
  std::string creator_str = std::string("MakeAppImageTorrent ") + std::string(GIT_COMMIT);
  std::string comment_str = "created by MakeAppImageTorrent";

  //// We will not be inserting the web seed here but 
  //// at the client side itself.
  //// Because the zsync file is the one which decides the 
  //// location of target file.

  std::cout << "MakeAppImageTorrent (commit " << GIT_COMMIT << "), built on "
	    << BUILD_TIME << "\n";
  std::cout << "Copyright (C) 2020, Antony Jr.\n\n";
  
  if(ac == 1) {
  	std::cout << "Usage:  " << av[0] << " [APPIMAGE]\n";
	std::exit(0);
  }

  int piece_size = 0;
  lt::create_flags_t flags = {};

  std::string file(av[1]);
  std::string outfile = file + ".torrent";
  std::string full_path = file;

  std::cout << "Outfile: " << outfile << std::endl;
  std::cout << "Target File: " << full_path << std::endl;

  lt::file_storage fs;
  if (full_path[0] != '/')
  {
    char cwd[2048];
    char const* ret = getcwd(cwd, sizeof(cwd));
    if (ret == nullptr) {
      std::cerr << "failed to get current working directory: "
        << strerror(errno) << "\n";
      return 1;
    }

    full_path = cwd + ("/" + full_path);
  }

  lt::add_files(fs, full_path, file_filter, flags);
  if (fs.num_files() == 0) {
    std::cerr << "no files specified.\n";
    return 1;
  }

  lt::create_torrent t(fs, piece_size, flags);
  int tier = 0;
  for (std::string const& tr : Trackers::All) {
    if (tr == "-") ++tier;
    else t.add_tracker(tr, tier);
  }

  auto const num = t.num_pieces();
  lt::set_piece_hashes(t, branch_path(full_path)
    , [num] (lt::piece_index_t const p) {
      std::cerr << "\r" << p << "/" << num;
    });

  std::cerr << "\n";
  t.set_creator(creator_str.c_str());
  if (!comment_str.empty()) {
    t.set_comment(comment_str.c_str());
  }


  ///// Write the torrent to disk.
  std::vector<char> torrent;
  lt::bencode(back_inserter(torrent), t.generate());
  std::fstream out;
  out.exceptions(std::ifstream::failbit);
  out.open(outfile.c_str(), std::ios_base::out | std::ios_base::binary);
  out.write(torrent.data(), int(torrent.size()));

  std::cout << "\nFinished.\n";
  return 0;
}
catch (std::exception& e) {
  std::cerr << "ERROR: " << e.what() << "\n";
  return 1;
}
