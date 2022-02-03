#include <fimpera-lib/generators/ReadReader.hpp>

fimpera_lib::generators::read::Iterator::Iterator(FileManager& read_files) : _read_files{read_files} {
    next();
}

const std::tuple<const std::string&, const std::string&> fimpera_lib::generators::read::Iterator::operator*() const {
    return {_current_read, _current_header};
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
    if (!_current_read.empty()) {
        std::string current_data = _read_files.get_data();
        _current_header = current_data.substr(0, current_data.find('\n'));
    }
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