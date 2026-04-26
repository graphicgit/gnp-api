#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace gnp::utils {

class CsvParser {
public:
    struct Row {
        std::vector<std::string> columns;
        
        std::string get(size_t index) const {
            if (index < columns.size()) {
                return columns[index];
            }
            return "";
        }
    };

    static std::vector<Row> parse(const std::string& csvData) {
        std::vector<Row> rows;
        std::stringstream ss(csvData);
        std::string line;

        while (std::getline(ss, line)) {
            // Trim trailing carriage return if exists (from Windows CRLF)
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            if (line.empty()) continue;

            Row row;
            std::string cell;
            bool inQuotes = false;

            for (size_t i = 0; i < line.length(); ++i) {
                char c = line[i];
                if (c == '"') {
                    inQuotes = !inQuotes;
                } else if (c == ',' && !inQuotes) {
                    row.columns.push_back(cell);
                    cell.clear();
                } else {
                    cell += c;
                }
            }
            row.columns.push_back(cell); // add last cell
            rows.push_back(row);
        }

        return rows;
    }
};

}
