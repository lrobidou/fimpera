#include <fimpera-lib/generators/ReadReader.hpp>

fimpera_lib::generators::read::Iterator::Iterator(FileManager& read_files) : _read_files{read_files} {
    next();
}

const std::string& fimpera_lib::generators::read::Iterator::operator*() const {
    return _current_read;
}

fimpera_lib::generators::read::Iterator& fimpera_lib::generators::read::Iterator::operator++() {
    next();
    return *this;
}

bool fimpera_lib::generators::read::Iterator::operator!=(End_iterator) const {
    return !_current_read.empty();
}

void fimpera_lib::generators::read::Iterator::next() {
    _current_read = _read_files.get_next_read();
}

fimpera_lib::generators::ReadReader::ReadReader(const std::string& filename) {
    _read_files = FileManager();
    _read_files.addFile(filename);
}

fimpera_lib::generators::read::Iterator fimpera_lib::generators::ReadReader::begin() {
    return read::Iterator{_read_files};
}

fimpera_lib::generators::read::End_iterator fimpera_lib::generators::ReadReader::end() {
    return fimpera_lib::generators::read::End_iterator{};
}