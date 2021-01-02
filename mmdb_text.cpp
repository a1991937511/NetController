//
// Created by 70725 on 2020/12/27.
//

#include <maxminddb.h>
#include <iostream>
#include <cstring>

int main() {
    std::cout << "version: " << MMDB_lib_version() << std::endl;

    MMDB_s mmdb;
    int status = MMDB_open("/tmp/tmp.XwtbuOfu1E/mmdb/GeoLite2-Country.mmdb", MMDB_MODE_MMAP, &mmdb);

    if (MMDB_SUCCESS != status) {
        fprintf(stderr, "\n  Can't open %s\n", MMDB_strerror(status));

        if (MMDB_IO_ERROR == status) {
            fprintf(stderr, "    IO error: %s\n", strerror(errno));
        }
        exit(1);
    }

    int gai_error, mmdb_error;
    auto ip_address = "216.24.177.74";
    MMDB_lookup_result_s result =
            MMDB_lookup_string(&mmdb, ip_address, &gai_error, &mmdb_error);

    if (0 != gai_error) {
        fprintf(stderr,
                "\n  Error from getaddrinfo for %s - %s\n\n",
                ip_address, gai_strerror(gai_error));
        exit(2);
    }

    if (MMDB_SUCCESS != mmdb_error) {
        fprintf(stderr,
                "\n  Got an error from libmaxminddb: %s\n\n",
                MMDB_strerror(mmdb_error));
        exit(3);
    }

    MMDB_entry_data_list_s *entry_data_list = nullptr;

    int exit_code = 0;
    if (result.found_entry) {
        int status = MMDB_get_entry_data_list(&result.entry,
                &entry_data_list);

        if (MMDB_SUCCESS != status) {
            fprintf(
                    stderr,
                    "Got an error looking up the entry data - %s\n",
                    MMDB_strerror(status));
            exit_code = 4;
            goto end;
        }

        if (NULL != entry_data_list) {
            MMDB_dump_entry_data_list(stdout, entry_data_list, 2);
        }

        MMDB_entry_data_s entryData{};
        status = MMDB_get_value(&result.entry, &entryData, "country", "names", "zh-CN", nullptr);
        if (MMDB_SUCCESS != status) {
            std::cout << "get value error" << std::endl;
        }
        if (entryData.has_data) {
            std::cout << std::string(entryData.utf8_string, entryData.data_size) << std::endl;
        }
    } else {
        fprintf(
                stderr,
                "\n  No entry for this IP address (%s) was found\n\n",
                ip_address);
        exit_code = 5;
    }

    end:
    MMDB_free_entry_data_list(entry_data_list);
    MMDB_close(&mmdb);
    exit(exit_code);
}