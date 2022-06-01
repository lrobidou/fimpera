#pragma once

#include <tuple>
// #include <findere_lib/indexer.hpp>
#include <fimpera-lib/reader/file_manager.hpp>
namespace fimpera_lib {
namespace generators {
namespace read {
class End_iterator {};

class Iterator {
   public:
    Iterator(FileManager& read_files);
    const std::tuple<const std::string&, const std::string&> operator*() const;
    Iterator& operator++();
    bool operator!=(End_iterator) const;

   private:
    void next();

    FileManager& _read_files;
    std::string _current_read;
    std::string _current_header;
};
}  // namespace read

class ReadReader {
   public:
    explicit ReadReader(const std::string& filename);
    fimpera_lib::generators::read::Iterator begin();
    fimpera_lib::generators::read::End_iterator end();

   private:
    FileManager _read_files;
};
}  // namespace generators
}  // namespace fimpera_lib
