/*
 * HEIF codec.
 * Copyright (c) 2022 Dirk Farin <dirk.farin@gmail.com>
 *
 * This file is part of libheif.
 *
 * libheif is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libheif is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libheif.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "heif_init.h"
#include "heif.h"
#include "heif_error.h"
#include "heif_plugin_registry.h"
#include "heif_common_utils.h"


//
//#if defined(_WIN32)
//#include "heif_plugins_windows.h"
//#else
//#include "heif_plugins_unix.h"
//#endif

using namespace heif;

void heif_unload_all_plugins();


static int heif_library_initialization_count = 0;
static bool default_plugins_registered = true; // because they are implicitly registered at startup



struct heif_error heif_init(struct heif_init_params*)
{


  heif_library_initialization_count++;

  if (heif_library_initialization_count == 1) {

    // --- initialize builtin plugins

    if (!default_plugins_registered) {
      register_default_plugins();
    }

  }

  return {heif_error_Ok, heif_suberror_Unspecified, Error::kSuccess};
}



static void heif_unregister_encoder_plugins()
{
  for (const auto& plugin : heif::s_encoder_descriptors) {
    if (plugin->plugin->cleanup_plugin) {
      (*plugin->plugin->cleanup_plugin)();
    }
  }
  heif::s_encoder_descriptors.clear();
}


void heif_deinit()
{

}


// This could be inside ENABLE_PLUGIN_LOADING, but the "include-what-you-use" checker cannot process this.
#include <vector>
#include <string>


static heif_error heif_error_plugins_unsupported{heif_error_Unsupported_feature, heif_suberror_Unspecified, "Plugins are not supported"};

struct heif_error heif_load_plugin(const char* filename, struct heif_plugin_info const** out_plugin)
{
  return heif_error_plugins_unsupported;
}


struct heif_error heif_unload_plugin(const struct heif_plugin_info* plugin)
{
  return heif_error_plugins_unsupported;
}


void heif_unload_all_plugins() {}

struct heif_error heif_load_plugins(const char* directory,
                                    const struct heif_plugin_info** out_plugins,
                                    int* out_nPluginsLoaded,
                                    int output_array_size)
{
  return heif_error_plugins_unsupported;
}


