/** Copyright 2026 Finalverse contributors. SPDX-License-Identifier: LGPL-2.1-only */
#include "linden_common.h"
#include "llfile.h"
#include <filesystem>
#include <iostream>

// A distribution acceptance tool, not a viewer component. Invoke against both
// the staged bundle and the mounted image. A protected AppTranslocation mount
// reproduces read/create flag failures that a writable developer copy hides.
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: finalverse_resource_probe /absolute/App.app/Contents/Resources\n";
        return 2;
    }
    const std::filesystem::path resources(argv[1]);
    if (!resources.is_absolute()) return 2;
    bool failed = false;
    for (const char* relative : {
        "app_settings/settings_files.xml", "app_settings/settings.xml",
        "app_settings/settings_per_account.xml", "app_settings/settings_crash_behavior.xml",
        "app_settings/ignorable_dialogs.xml", "skins/default/xui/en/strings.xml",
        "skins/default/colors.xml", "ca-bundle.crt"})
    {
        std::error_code ec;
        const auto path = resources / relative;
        const auto bytes = LLFile::getContents(path, ec);
        if (ec || bytes.empty())
        {
            std::cerr << "READ_FAILED " << relative << ": " << ec.message()
                      << " (" << ec.value() << ")\n";
            failed = true;
            continue;
        }
        const auto size = std::filesystem::file_size(path, ec);
        if (ec || bytes.size() != size)
        {
            std::cerr << "INCOMPLETE_READ " << relative << '\n';
            failed = true;
        }
        else std::cout << "READ_OK " << relative << " (" << size << " bytes)\n";
    }
    return failed ? 1 : 0;
}
